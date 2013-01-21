#include "ofxAudioUnit.h"
#include "TPCircularBuffer/TPCircularBuffer.h"

static OSStatus SilentRenderCallback(void * inRefCon,
									 AudioUnitRenderActionFlags *	ioActionFlags,
									 const AudioTimeStamp *	inTimeStamp,
									 UInt32 inBusNumber,
									 UInt32	inNumberFrames,
									 AudioBufferList * ioData);

static OSStatus RenderAndCopy(void * inRefCon,
							  AudioUnitRenderActionFlags *	ioActionFlags,
							  const AudioTimeStamp *	inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32	inNumberFrames,
							  AudioBufferList * ioData);

typedef enum
{
	TapSourceNone,
	TapSourceUnit,
	TapSourceCallback
}
ofxAudioUnitTapSourceType;

struct TapContext
{
	ofxAudioUnitTapSourceType sourceType;
	ofxAudioUnit * sourceUnit;
	UInt32 sourceBus;
	AURenderCallbackStruct sourceCallback;
	vector<TPCircularBuffer> circularBuffers;
	UInt32 samplesToTrack;
	
	void setCircularBufferCount(UInt32 bufferCount) {
		for(int i = 0; i < circularBuffers.size(); i++) {
			TPCircularBufferCleanup(&circularBuffers[i]);
		}
		
		circularBuffers.resize(bufferCount);
		
		for(int i = 0; i < circularBuffers.size(); i++) {
			TPCircularBufferInit(&circularBuffers[i], samplesToTrack * sizeof(AudioUnitSampleType));
		}
	}
};

struct ofxAudioUnitTap::TapImpl
{
	TapContext ctx;
};

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap(unsigned int samplesToTrack) : _impl(new TapImpl)
// ----------------------------------------------------------
{
	_impl->ctx.samplesToTrack = samplesToTrack;
	_impl->ctx.sourceBus = 0;
}

// ----------------------------------------------------------
ofxAudioUnitTap::~ofxAudioUnitTap()
// ----------------------------------------------------------
{
	for(int i = 0; i < _impl->ctx.circularBuffers.size(); i++) {
		TPCircularBufferCleanup(&_impl->ctx.circularBuffers[i]);
	}
}

#pragma mark - Connections

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitTap::connectTo(ofxAudioUnit &destination, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	if(_impl->ctx.sourceType == TapSourceNone || !_impl->ctx.sourceUnit) {
		std::cout << "Tap can't be connected without a source" << std::endl;
		AURenderCallbackStruct silentCallback = {SilentRenderCallback};
		destination.setRenderCallback(silentCallback);
		return destination;
	}
	
	AURenderCallbackStruct callback = {RenderAndCopy, &_impl->ctx};
	destination.setRenderCallback(callback, destinationBus);
	return destination;
}

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitTap::operator>>(ofxAudioUnit &destination)
// ----------------------------------------------------------
{
	return connectTo(destination);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::setSource(ofxAudioUnit * source)
// ----------------------------------------------------------
{
	_impl->ctx.sourceUnit = source;
	_impl->ctx.sourceType = TapSourceUnit;
	
	AudioStreamBasicDescription ASBD = {0};
	UInt32 ASBD_size = sizeof(ASBD);
	
	OFXAU_PRINT(AudioUnitGetProperty(source->getUnit(),
									 kAudioUnitProperty_StreamFormat,
									 kAudioUnitScope_Output,
									 0,
									 &ASBD,
									 &ASBD_size),
				"getting tap source's ASBD");
	
	_impl->ctx.setCircularBufferCount(ASBD.mChannelsPerFrame);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::setSource(AURenderCallbackStruct callback, UInt32 channels)
// ----------------------------------------------------------
{
	_impl->ctx.sourceCallback = callback;
	_impl->ctx.sourceType = TapSourceCallback;
	
	_impl->ctx.setCircularBufferCount(channels);
}

#pragma mark - Getting samples

// ----------------------------------------------------------
void ExtractSamplesFromCircularBuffer(ofxAudioUnitTap::MonoSamples &outBuffer, TPCircularBuffer * circularBuffer)
// ----------------------------------------------------------
{
	if(!circularBuffer) {
		outBuffer.clear();
	} else {
		int32_t circBufferSize;
		AudioUnitSampleType * circBufferTail = (AudioUnitSampleType *)TPCircularBufferTail(circularBuffer, &circBufferSize);
		AudioUnitSampleType * circBufferHead = circBufferTail + (circBufferSize / sizeof(AudioUnitSampleType));
		
		outBuffer.assign(circBufferTail, circBufferHead);
	}
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData) const
// ----------------------------------------------------------
{
	ExtractSamplesFromCircularBuffer(outData, &_impl->ctx.circularBuffers.front());
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData, unsigned int channel) const
// ----------------------------------------------------------
{
	if(_impl->ctx.circularBuffers.size() > channel) {
		ExtractSamplesFromCircularBuffer(outData, &_impl->ctx.circularBuffers[channel]);
	} else {
		outData.clear();
	}
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(StereoSamples &outData) const
// ----------------------------------------------------------
{
	if(_impl->ctx.circularBuffers.size() >= 2) {
		ExtractSamplesFromCircularBuffer(outData.left,  &_impl->ctx.circularBuffers[0]);
		ExtractSamplesFromCircularBuffer(outData.right, &_impl->ctx.circularBuffers[1]);
	} else if(_impl->ctx.circularBuffers.size() == 1) {
		ExtractSamplesFromCircularBuffer(outData.left, &_impl->ctx.circularBuffers[0]);
		outData.right.clear();
	} else {
		outData.left.clear();
		outData.right.clear();
	}
}

// ----------------------------------------------------------
void WaveformForBuffer(const ofxAudioUnitTap::MonoSamples &buffer, float width, float height, ofPolyline &outLine)
// ----------------------------------------------------------
{	
	outLine.clear();
	
	const float xStep = width / buffer.size();
	float x = 0;
	
	for (int i = 0; i < buffer.size(); i++, x += xStep)
	{
#if TARGET_OS_IPHONE
		SInt16 s = SInt16(buffer[i] >> 9);
		float y = ofMap(s, -32768, 32767, height, 0, true);
#else
		float y = ofMap(buffer[i], -1, 1, height, 0, true);
#endif
		outLine.addVertex(ofPoint(x, y));
	}
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getLeftWaveform(ofPolyline &outLine, float width, float height) const
// ----------------------------------------------------------
{
	MonoSamples leftBuffer;
	getSamples(leftBuffer, 0);
	WaveformForBuffer(leftBuffer, width, height, outLine);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getRightWaveform(ofPolyline &outLine, float width, float height) const
// ----------------------------------------------------------
{
	MonoSamples rightBuffer;
	getSamples(rightBuffer, 1);
	WaveformForBuffer(rightBuffer, width, height, outLine);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getStereoWaveform(ofPolyline &outLeft, ofPolyline &outRight, float width, float height) const
// ----------------------------------------------------------
{
	getLeftWaveform(outLeft, width, height);
	getRightWaveform(outRight, width, height);
}

#pragma mark - Render callbacks

inline void CopyAudioBufferIntoCircularBuffer(TPCircularBuffer * circBuffer, const AudioBuffer &audioBuffer)
{
	int32_t availableBytesInCircBuffer;
	TPCircularBufferHead(circBuffer, &availableBytesInCircBuffer);
	
	if(availableBytesInCircBuffer < audioBuffer.mDataByteSize) {
		TPCircularBufferConsume(circBuffer, audioBuffer.mDataByteSize - availableBytesInCircBuffer);
	}
	
	TPCircularBufferProduceBytes(circBuffer, audioBuffer.mData, audioBuffer.mDataByteSize);
}

// ----------------------------------------------------------
OSStatus RenderAndCopy(void * inRefCon,
					   AudioUnitRenderActionFlags * ioActionFlags,
					   const AudioTimeStamp * inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32 inNumberFrames,
					   AudioBufferList * ioData)
{
	TapContext * ctx = static_cast<TapContext *>(inRefCon);
	
	OSStatus status;
	
	if(ctx->sourceType == TapSourceUnit && ctx->sourceUnit->getUnit()) {
		status = ctx->sourceUnit->render(ioActionFlags, inTimeStamp, ctx->sourceBus, inNumberFrames, ioData);
	} else if(ctx->sourceType == TapSourceCallback) {
		status = (ctx->sourceCallback.inputProc)(ctx->sourceCallback.inputProcRefCon,
												 ioActionFlags,
												 inTimeStamp,
												 ctx->sourceBus,
												 inNumberFrames,
												 ioData);
	} else {
		// if we don't have a source, render silence (or else you'll get an extremely loud
		// buzzing noise when we attempt to render a NULL unit. Ow.)
		status = SilentRenderCallback(inRefCon, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
	}
	
	if(status == noErr) {
		const size_t buffersToCopy = min(ctx->circularBuffers.size(), ioData->mNumberBuffers);
		
		for(int i = 0; i < buffersToCopy; i++) {
			CopyAudioBufferIntoCircularBuffer(&ctx->circularBuffers[i], ioData->mBuffers[i]);
		}
	}
	
	return status;
}

OSStatus SilentRenderCallback(void * inRefCon,
							  AudioUnitRenderActionFlags * ioActionFlags,
							  const AudioTimeStamp * inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32 inNumberFrames,
							  AudioBufferList * ioData)
{
	for(int i = 0; i < ioData->mNumberBuffers; i++) {
		memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[0].mDataByteSize);
	}
	
	*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
	
	return noErr;
}
