#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"
#include "ofxAudioUnitHardwareUtils.h"
#include <CoreFoundation/CoreFoundation.h>

AudioComponentDescription outputDesc = {
	kAudioUnitType_Output,
#if TARGET_OS_IPHONE
	kAudioUnitSubType_RemoteIO,
#else
	kAudioUnitSubType_HALOutput,
#endif
	kAudioUnitManufacturer_Apple
};


// ----------------------------------------------------------
ofxAudioUnitOutput::ofxAudioUnitOutput()
// ----------------------------------------------------------
{
	_desc = outputDesc;
	initUnit();
}

// ----------------------------------------------------------
bool ofxAudioUnitOutput::start()
// ----------------------------------------------------------
{
	OFXAU_RET_BOOL(AudioOutputUnitStart(*_unit), "starting output unit");
}

// ----------------------------------------------------------
bool ofxAudioUnitOutput::stop()
// ----------------------------------------------------------
{
	OFXAU_RET_BOOL(AudioOutputUnitStop(*_unit), "stopping output unit");
}

// ----------------------------------------------------------
void ofxAudioUnitOutput::listOutputDevices()
// ----------------------------------------------------------
{
	vector<AudioDeviceID> deviceList = AudioOutputDeviceList();
	
	for(int i = 0; i < deviceList.size(); i++) {
		cout << deviceList[i] << " : " << DeviceNameForAudioDeviceID(deviceList[i]) << endl;
	}
}
