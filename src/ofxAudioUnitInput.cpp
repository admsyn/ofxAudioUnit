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


// ----------------------------------------------------------
ofxAudioUnitInput::ofxAudioUnitInput() : _isReady(false)
// ----------------------------------------------------------
{
	_desc = inputDesc;
	initUnit();
}

// ----------------------------------------------------------
ofxAudioUnitInput::~ofxAudioUnitInput()
// ----------------------------------------------------------
{
	stop();
}

// ----------------------------------------------------------
void ofxAudioUnitInput::connectTo(ofxAudioUnit &otherUnit, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
void ofxAudioUnitInput::connectTo(ofxAudioUnitTap &tap)
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
bool ofxAudioUnitInput::start()
// ----------------------------------------------------------
{
	if(!_isReady) _isReady = configureInputDevice();
	if(!_isReady) return false;
	
	OFXAU_RET_BOOL(AudioOutputUnitStart(*_unit), "starting hardware input unit");
}

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
	
	_inputContext.inputUnit  = _unit;
	_inputContext.ringBuffer = _ringBuffer;
	
	AURenderCallbackStruct inputCallback;
	inputCallback.inputProc = ofxAudioUnitInput::renderCallback;
	inputCallback.inputProcRefCon = &_inputContext;
	
	OFXAU_RET_FALSE(AudioUnitSetProperty(*_unit,
										 kAudioOutputUnitProperty_SetInputCallback,
										 kAudioUnitScope_Global,
										 0,
										 &inputCallback,
										 sizeof(inputCallback)),
					"setting hardware input callback");
	
	OFXAU_RET_FALSE(AudioUnitInitialize(*_unit), 
					"initializing hardware input unit after setting it to input mode");
	
	return true;
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
	std::cout << "I happened" << std::endl;
	return 1;
}

// ----------------------------------------------------------
bool ofxAudioUnitInput::stop()
// ----------------------------------------------------------
{
	OFXAU_RET_BOOL(AudioOutputUnitStop(*_unit), "stopping hardware input unit");
}
