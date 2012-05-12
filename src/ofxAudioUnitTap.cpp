#include "ofxAudioUnit.h"

static OSStatus tapRenderCallback(void * inRefCon,
																	AudioUnitRenderActionFlags *	ioActionFlags,
																	const AudioTimeStamp *	inTimeStamp,
																	UInt32 inBusNumber,
																	UInt32	inNumberFrames,
																	AudioBufferList * ioData);

static OSStatus silentRenderCallback(void * inRefCon,
																		 AudioUnitRenderActionFlags *	ioActionFlags,
																		 const AudioTimeStamp *	inTimeStamp,
																		 UInt32 inBusNumber,
																		 UInt32	inNumberFrames,
																		 AudioBufferList * ioData);

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap() : 
_trackedSamples(NULL)
// ----------------------------------------------------------
{
}

// ----------------------------------------------------------
ofxAudioUnitTap::~ofxAudioUnitTap()
// ----------------------------------------------------------
{
	// just in case, we'll set the destination's callback to a context-less
	// silent render callback
	if(_destinationUnit)
	{
		AURenderCallbackStruct callbackInfo;
		callbackInfo.inputProc = silentRenderCallback;
		callbackInfo.inputProcRefCon = NULL;
		ERR_CHK(AudioUnitSetProperty(*_destinationUnit,
																 kAudioUnitProperty_SetRenderCallback,
																 kAudioUnitScope_Input,
																 _destinationBus,
																 &callbackInfo,
																 sizeof(callbackInfo)),
						"setting tap destination to a silent render callback");
	}
	
	_bufferMutex.lock();
	{
		if(_trackedSamples) releaseBufferList(_trackedSamples);
	}
	_bufferMutex.unlock();
}

#pragma mark - Utilities

// ----------------------------------------------------------
void ofxAudioUnitTap::releaseBufferList(AudioBufferList * bufferList)
// ----------------------------------------------------------
{
	for(int i = 0; i < bufferList->mNumberBuffers; i++)
		free(bufferList->mBuffers[i].mData);
	
	free(bufferList);
}

// ----------------------------------------------------------
AudioBufferList * ofxAudioUnitTap::allocBufferList(int channels, size_t size)
// ----------------------------------------------------------
{
	AudioBufferList * bufferList;
	size_t bufferSize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * channels);
	bufferList = (AudioBufferList *)malloc(bufferSize);
	bufferList->mNumberBuffers = channels;
	
	for(UInt32 i = 0; i < bufferList->mNumberBuffers; i++)
	{
		bufferList->mBuffers[i].mNumberChannels = 1;
		bufferList->mBuffers[i].mDataByteSize = sizeof(AudioUnitSampleType) * size;
		bufferList->mBuffers[i].mData = malloc(sizeof(AudioUnitSampleType)  * size);
		memset(bufferList->mBuffers[i].mData, 0, bufferList->mBuffers[i].mDataByteSize);
	}
	return bufferList;
}

#pragma mark - Connections

// ----------------------------------------------------------
void ofxAudioUnitTap::connectTo(ofxAudioUnit &destination, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	if(_trackedSamples) releaseBufferList(_trackedSamples);
	
	AudioStreamBasicDescription asbd = {0};
	UInt32 dataSize = sizeof(AudioStreamBasicDescription);
	
//	Connect the source to the destination.
//	The only reason for this is so that they can sort out their
//	own ASBDs. The destination unit will be connected to the tap's
//	render callback afterwards.
	AudioUnitConnection c;
	c.sourceAudioUnit = *_sourceUnit;
	c.sourceOutputNumber = sourceBus;
	c.destInputNumber = destinationBus;
	AudioUnitSetProperty(*(destination._unit),
											 kAudioUnitProperty_MakeConnection,
											 kAudioUnitScope_Global,
											 destinationBus,
											 &c,
											 sizeof(c));
	
	AudioUnitGetProperty(*(_sourceUnit),
											 kAudioUnitProperty_StreamFormat,
											 kAudioUnitScope_Output,
											 sourceBus,
											 &asbd,
											 &dataSize);
	
	_trackedSamples = allocBufferList(asbd.mChannelsPerFrame);
	
	_tapContext.sourceUnit     = _sourceUnit;
	_tapContext.bufferMutex    = &_bufferMutex;
	_tapContext.trackedSamples = _trackedSamples;
	
	AURenderCallbackStruct callbackInfo;
	callbackInfo.inputProc = tapRenderCallback;
	callbackInfo.inputProcRefCon = &_tapContext;
	destination.setRenderCallback(callbackInfo, destinationBus);
	
	_destinationUnit = destination._unit;
	_destinationBus  = destinationBus;
}

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitTap::operator>>(ofxAudioUnit &destination)
// ----------------------------------------------------------
{
	connectTo(destination);
	return destination;
}

#pragma mark - Getting samples

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(ofxAudioUnitTapSamples &outData)
// ----------------------------------------------------------
{
	if(!_trackedSamples) return;
	
	outData.left.clear();
	outData.right.clear();
	
	_bufferMutex.lock();
	{
		AudioUnitSampleType * leftSamples = (AudioUnitSampleType *)_trackedSamples->mBuffers[0].mData;
		for(int i = 0; i < _trackedSamples->mBuffers[0].mDataByteSize / sizeof(AudioUnitSampleType); i++)
			outData.left.push_back(leftSamples[i]);
		
		AudioUnitSampleType * rightSamples = (AudioUnitSampleType *)_trackedSamples->mBuffers[1].mData;
		for(int i = 0; i < _trackedSamples->mBuffers[1].mDataByteSize / sizeof(AudioUnitSampleType); i++)
			outData.right.push_back(rightSamples[i]);
	}
	_bufferMutex.unlock();
}

// ----------------------------------------------------------
void ofxAudioUnitTap::waveformForBuffer(AudioBuffer *buffer, float width, float height, ofPolyline &outLine)
// ----------------------------------------------------------
{	
	outLine.clear();
	_bufferMutex.lock();
	{
		float xStep = width / (buffer->mDataByteSize / sizeof(AudioUnitSampleType));
		float x = 0;
		AudioUnitSampleType * samples = (AudioUnitSampleType *)buffer->mData;
		
		for (int i = 0; i < buffer->mDataByteSize / sizeof(AudioUnitSampleType); i++, x += xStep) 
		{
#if TARGET_OS_IPHONE
			SInt16 s = SInt16(samples[i] >> 9);
			float y = ofMap(s, -32768, 32767, height, 0, true);
#else
			float y = ofMap(samples[i], -1, 1, height, 0, true);
#endif
			outLine.addVertex(ofPoint(x, y));
		}
	}
	_bufferMutex.unlock();
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getLeftWaveform(ofPolyline &outLine, float width, float height)
// ----------------------------------------------------------
{
	if(!_trackedSamples) return;
	
	waveformForBuffer(&_trackedSamples->mBuffers[0], width, height, outLine);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getRightWaveform(ofPolyline &outLine, float width, float height)
// ----------------------------------------------------------
{
	if(!_trackedSamples) return;
	
	waveformForBuffer(&_trackedSamples->mBuffers[1], width, height, outLine);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getStereoWaveform(ofPolyline &outLeft, ofPolyline &outRight, float width, float height)
// ----------------------------------------------------------
{
	getLeftWaveform(outLeft, width, height);
	getRightWaveform(outRight, width, height);
}

#pragma mark - Render callbacks

// ----------------------------------------------------------
OSStatus tapRenderCallback(void * inRefCon,
													 AudioUnitRenderActionFlags *	ioActionFlags,
													 const AudioTimeStamp *	inTimeStamp,
													 UInt32 inBusNumber,
													 UInt32	inNumberFrames,
													 AudioBufferList * ioData)
// ----------------------------------------------------------
{
	ofxAudioUnitTapContext * context = (ofxAudioUnitTapContext *)inRefCon;
	
	// if we don't have a source, render silence (or else you'll get an extremely loud
	// buzzing noise when we attempt to render a NULL unit. Ow.)
	if(!(context->sourceUnit))
	{
		for(int i = 0; i < ioData->mNumberBuffers; i++)
		{
			memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[0].mDataByteSize);
			*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
		}
		return noErr;
	}
	
	// if we're all set, render the source unit into the destination unit...
	ERR_CHK(AudioUnitRender(*(context->sourceUnit),
													ioActionFlags,
													inTimeStamp,
													0,
													inNumberFrames,
													ioData),
					"passing source into destination");
	
	// if the tracked sample buffer isn't locked, copy the audio output there as well
	if(context->bufferMutex->tryLock())
	{
		int numChannels = min(ioData->mNumberBuffers, context->trackedSamples->mNumberBuffers);
		size_t bytesToCopy = min(ioData->mBuffers[0].mDataByteSize, 
														 context->trackedSamples->mBuffers[0].mDataByteSize);
		
		for(int i = 0; i < numChannels; i++)
		{
			memcpy(context->trackedSamples->mBuffers[i].mData,
						 ioData->mBuffers[i].mData,
						 bytesToCopy);
			context->trackedSamples->mBuffers[i].mDataByteSize = bytesToCopy;
		}
		
		context->bufferMutex->unlock();
	}
	
	return noErr;
}

// ----------------------------------------------------------
OSStatus silentRenderCallback(void * inRefCon,
															AudioUnitRenderActionFlags *	ioActionFlags,
															const AudioTimeStamp *	inTimeStamp,
															UInt32 inBusNumber,
															UInt32	inNumberFrames,
															AudioBufferList * ioData)
// ----------------------------------------------------------
{
	for(int i = 0; i < ioData->mNumberBuffers; i++)
	{
		memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[0].mDataByteSize);
		*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
	}
	return noErr;
}
