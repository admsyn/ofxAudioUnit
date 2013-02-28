#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"
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

string DeviceNameForAudioDeviceID(AudioDeviceID deviceID)
{
	AudioObjectPropertyAddress deviceNameProp = {
		.mSelector = kAudioObjectPropertyName,
		.mScope    = kAudioObjectPropertyScopeGlobal,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	CFStringRef deviceName;
	UInt32 deviceNameDataSize = sizeof(deviceName);
	OFXAU_PRINT(AudioObjectGetPropertyData(deviceID,
										   &deviceNameProp,
										   0,
										   NULL,
										   &deviceNameDataSize,
										   &deviceName),
				"getting device name");
	
	string deviceNameString("Unknown");
	
	if(deviceName) {
		char buffer[255];
		CFStringGetCString(deviceName, buffer, 255, kCFStringEncodingUTF8);
		deviceNameString = string(buffer);
	}
	
	return deviceNameString;
}

// ----------------------------------------------------------
void ofxAudioUnitOutput::listOutputDevices()
// ----------------------------------------------------------
{
	AudioObjectPropertyAddress deviceListProp = {
		.mSelector = kAudioHardwarePropertyDevices,
		.mScope    = kAudioObjectPropertyScopeGlobal,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	UInt32 deviceListDataSize = 0;
	OFXAU_RETURN(AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
												&deviceListProp,
												0,
												NULL,
												&deviceListDataSize),
				 "getting device list size");
	
	const UInt32 deviceCount = deviceListDataSize / sizeof(AudioDeviceID);
	AudioDeviceID deviceList[deviceCount];
	OFXAU_RETURN(AudioObjectGetPropertyData(kAudioObjectSystemObject,
											&deviceListProp,
											0,
											NULL,
											&deviceListDataSize,
											deviceList),
				 "getting device list");
	
	for(int i = 0; i < deviceCount; i++) {
		cout << deviceList[i] << " : " << DeviceNameForAudioDeviceID(deviceList[i]) << endl;
	}
}
