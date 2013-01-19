#include "ofxAudioUnit.h"

AudioComponentDescription inputDesc = {
	kAudioUnitType_Output,
#if TARGET_OS_IPHONE
	kAudioUnitSubType_RemoteIO,
#else
	kAudioUnitSubType_HALOutput,
#endif
	kAudioUnitManufacturer_Apple
};

#pragma mark RingBuffer

// ----------------------------------------------------------
ofxAudioUnitInput::RingBuffer::RingBuffer(UInt32 buffers, UInt32 channels, UInt32 samples) :
_readItrIndex(0), _writeItrIndex(0)
// ----------------------------------------------------------
{
	reserve(buffers);
	
	for(UInt32 i = 0; i < buffers; i++)
	{
		AudioBufferListRef bufferList(allocBufferList(channels,samples), releaseBufferList);
		push_back(bufferList);
	}
	
	_readItr = _writeItr = begin();
}

// ----------------------------------------------------------
ofxAudioUnitInput::RingBuffer::~RingBuffer()
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
void ofxAudioUnitInput::RingBuffer::advanceItr(ofxAudioUnitInput::RingBuffer::iterator &itr)
// ----------------------------------------------------------
{
	++itr;
	if(itr == end()) itr = begin();
}

// ----------------------------------------------------------
bool ofxAudioUnitInput::RingBuffer::advanceReadHead()
// ----------------------------------------------------------
{
	if(_readItrIndex >= _writeItrIndex)
	{
		return false;
	}
	else 
	{
		advanceItr(_readItr);
		_readItrIndex++;
		return true;
	}
}

// ----------------------------------------------------------
void ofxAudioUnitInput::RingBuffer::advanceWriteHead()
// ----------------------------------------------------------
{
	advanceItr(_writeItr);
	_writeItrIndex++;
}


#pragma mark - ofxAudioUnitInput

// ----------------------------------------------------------
ofxAudioUnitInput::ofxAudioUnitInput() : _isReady(false)
// ----------------------------------------------------------
{
	_desc = inputDesc;
	initUnit();
	
	_ringBuffer = RingBufferRef(new ofxAudioUnitInput::RingBuffer());
	
	_renderContext.inputUnit  = _unit;
	_renderContext.ringBuffer = _ringBuffer;
}

// ----------------------------------------------------------
ofxAudioUnitInput::~ofxAudioUnitInput()
// ----------------------------------------------------------
{
	stop();
}

#pragma mark - Connections

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnitInput::connectTo(ofxAudioUnit &otherUnit, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	AURenderCallbackStruct callback;
	callback.inputProc = pullCallback;
	callback.inputProcRefCon = &_renderContext;
	
	AudioStreamBasicDescription ASBD;
	UInt32 ASBDSize = sizeof(ASBD);
	
	OFXAU_RETURN(AudioUnitGetProperty(otherUnit,
									  kAudioUnitProperty_StreamFormat,
									  kAudioUnitScope_Input,
									  destinationBus,
									  &ASBD,
									  &ASBDSize),
				 "getting hardware input destination's format");
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
									  kAudioUnitProperty_StreamFormat,
									  kAudioUnitScope_Output,
									  1,
									  &ASBD,
									  sizeof(ASBD)),
				 "setting hardware input's output format");
	
	otherUnit.setRenderCallback(callback, destinationBus);
	return otherUnit;
}

#pragma mark - Start / Stop

// ----------------------------------------------------------
bool ofxAudioUnitInput::start()
// ----------------------------------------------------------
{
	if(!_isReady) _isReady = configureInputDevice();
	if(!_isReady) return false;
	
	OFXAU_RET_BOOL(AudioOutputUnitStart(*_unit), "starting hardware input unit");
}

// ----------------------------------------------------------
bool ofxAudioUnitInput::stop()
// ----------------------------------------------------------
{
	OFXAU_RET_BOOL(AudioOutputUnitStop(*_unit), "stopping hardware input unit");
}

#pragma mark - Configuration

// ----------------------------------------------------------
bool ofxAudioUnitInput::configureInputDevice()
// ----------------------------------------------------------
{
	UInt32 on  = 1;
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioOutputUnitProperty_EnableIO,
										 kAudioUnitScope_Input,
										 1,
										 &on,
										 sizeof(on)), 
					"enabling input on HAL unit");
	
	UInt32 off = 0;
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioOutputUnitProperty_EnableIO,
										 kAudioUnitScope_Output,
										 0,
										 &off,
										 sizeof(off)),
					"disabling output on HAL unit");
	
	AudioDeviceID inputDeviceID = kAudioObjectUnknown;
	UInt32 deviceIDSize = sizeof( AudioDeviceID );
	AudioObjectPropertyAddress prop_addr = {
		kAudioHardwarePropertyDefaultInputDevice,
		kAudioObjectPropertyScopeGlobal,
		kAudioObjectPropertyElementMaster
	};
	
	OFXAU_RET_FALSE(AudioObjectGetPropertyData(kAudioObjectSystemObject,
											   &prop_addr,
											   0,
											   NULL,
											   &deviceIDSize,
											   &inputDeviceID),
					"getting device ID for default input");
	
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioOutputUnitProperty_CurrentDevice,
										 kAudioUnitScope_Global,
										 0,
										 &inputDeviceID,
										 deviceIDSize), 
					"setting HAL unit's device ID");
	
	AudioStreamBasicDescription deviceASBD = {0};
	UInt32 ASBDSize = sizeof(deviceASBD);
	OFXAU_RET_FALSE(AudioUnitGetProperty(*_unit,
										 kAudioUnitProperty_StreamFormat,
										 kAudioUnitScope_Output,
										 1,
										 &deviceASBD,
										 &ASBDSize),
					"getting hardware stream format");
	
	deviceASBD.mSampleRate = 44100;
	
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioUnitProperty_StreamFormat,
										 kAudioUnitScope_Output,
										 1,
										 &deviceASBD,
										 sizeof(deviceASBD)),
					"setting input sample rate to 44100");
	
	AURenderCallbackStruct inputCallback;
	inputCallback.inputProc = ofxAudioUnitInput::renderCallback;
	inputCallback.inputProcRefCon = &_renderContext;
	
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioOutputUnitProperty_SetInputCallback,
										 kAudioUnitScope_Global,
										 0,
										 &inputCallback,
										 sizeof(inputCallback)),
					"setting hardware input callback");
	
	OFXAU_RET_BOOL(AudioUnitInitialize(*_unit), 
				   "initializing hardware input unit after setting it to input mode");
}

#pragma mark - Callbacks / Rendering

// ----------------------------------------------------------
OSStatus ofxAudioUnitInput::render(AudioUnitRenderActionFlags *ioActionFlags, 
								   const AudioTimeStamp *inTimeStamp,
								   UInt32 inOutputBusNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList *ioData)
// ----------------------------------------------------------
{
	return pullCallback(&_renderContext, ioActionFlags, inTimeStamp, 
						inOutputBusNumber, inNumberFrames, ioData);
}

// ----------------------------------------------------------
OSStatus ofxAudioUnitInput::renderCallback(void *inRefCon,
										   AudioUnitRenderActionFlags *ioActionFlags,
										   const AudioTimeStamp *inTimeStamp,
										   UInt32 inBusNumber,
										   UInt32 inNumberFrames,
										   AudioBufferList *ioData)
// ----------------------------------------------------------
{
	RenderContext * ctx = reinterpret_cast<RenderContext *>(inRefCon);
	
	OSStatus s = AudioUnitRender(*(ctx->inputUnit),
								 ioActionFlags,
								 inTimeStamp,
								 inBusNumber,
								 inNumberFrames,
								 ctx->ringBuffer->writeHead());
	
	OFXAU_PRINT(s, "rendering audio input");
	
	if(s == noErr) ctx->ringBuffer->advanceWriteHead();
	
	return s;
}

// ----------------------------------------------------------
OSStatus ofxAudioUnitInput::pullCallback(void *inRefCon,
										 AudioUnitRenderActionFlags *ioActionFlags,
										 const AudioTimeStamp *inTimeStamp,
										 UInt32 inBusNumber,
										 UInt32 inNumberFrames,
										 AudioBufferList *ioData)
// ----------------------------------------------------------
{
	RenderContext * ctx = reinterpret_cast<RenderContext *>(inRefCon);
	
	// If we can't advance the read head, render silence.
	// Otherwise, copy the data from the ring buffer's read head.
	if(!ctx->ringBuffer->advanceReadHead())
	{
		for(int i = 0; i < ioData->mNumberBuffers; i++)
		{
			memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
		}
		
		*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
	}
	else 
	{
		AudioBufferList * bufferedAudio = ctx->ringBuffer->readHead();
		int buffersToCopy = min(ioData->mNumberBuffers, bufferedAudio->mNumberBuffers);
		size_t bytesToCopy = min(ioData->mBuffers[0].mDataByteSize, 
							  bufferedAudio->mBuffers[0].mDataByteSize);
		bytesToCopy = min(bytesToCopy, inNumberFrames * sizeof(AudioUnitSampleType));
		
		for(int i = 0; i < buffersToCopy; i++)
		{
			memcpy(ioData->mBuffers[i].mData, 
				   bufferedAudio->mBuffers[i].mData, 
				   bytesToCopy);
		}
	}
	
	return noErr;
}
