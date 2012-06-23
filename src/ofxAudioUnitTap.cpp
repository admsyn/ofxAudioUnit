#include "ofxAudioUnit.h"

static OSStatus silentRenderCallback(void * inRefCon,
									 AudioUnitRenderActionFlags *	ioActionFlags,
									 const AudioTimeStamp *	inTimeStamp,
									 UInt32 inBusNumber,
									 UInt32	inNumberFrames,
									 AudioBufferList * ioData);

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap() :
_trackedSamples(NULL), _sourceUnit(NULL), _destinationUnit(NULL)
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
		OFXAU_PRINT(AudioUnitSetProperty(*_destinationUnit->getUnit(),
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
	c.sourceAudioUnit = *(_sourceUnit->getUnit());
	c.sourceOutputNumber = sourceBus;
	c.destInputNumber = destinationBus;
	AudioUnitSetProperty(*(destination.getUnit()),
						 kAudioUnitProperty_MakeConnection,
						 kAudioUnitScope_Global,
						 destinationBus,
						 &c,
						 sizeof(c));
	
	AudioUnitGetProperty(*(_sourceUnit->getUnit()),
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
	callbackInfo.inputProc = renderAndCopy;
	callbackInfo.inputProcRefCon = &_tapContext;
	destination.setRenderCallback(callbackInfo, destinationBus);
	
	_destinationUnit = &destination;
	_destinationBus  = destinationBus;
}

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitTap::operator>>(ofxAudioUnit &destination)
// ----------------------------------------------------------
{
	connectTo(destination);
	return destination;
}

// ----------------------------------------------------------
void ofxAudioUnitTap::setSource(ofxAudioUnit * source)
// ----------------------------------------------------------
{
	_sourceUnit = source;
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
OSStatus ofxAudioUnitTap::renderAndCopy(void * inRefCon,
										AudioUnitRenderActionFlags * ioActionFlags,
										const AudioTimeStamp * inTimeStamp,
										UInt32 inBusNumber,
										UInt32	inNumberFrames,
										AudioBufferList * ioData)
// ----------------------------------------------------------
{
	TapContext * context = (TapContext *)inRefCon;
	
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
	OFXAU_PRINT(context->sourceUnit->render(ioActionFlags,
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
