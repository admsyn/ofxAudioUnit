//
//  ofxAudioUnitHardwareUtils.cpp
//  example-busses
//
//  Created by Adam on 2013-03-01.
//
//

#include "ofxAudioUnitHardwareUtils.h"
#include "ofxAudioUnitUtils.h"

#pragma mark - Device List

std::vector<AudioDeviceID> AudioDeviceList()
{
	AudioObjectPropertyAddress deviceListProp = {
		.mSelector = kAudioHardwarePropertyDevices,
		.mScope    = kAudioObjectPropertyScopeGlobal,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	UInt32 deviceListDataSize = 0;
	OFXAU_PRINT(AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
											   &deviceListProp,
											   0,
											   NULL,
											   &deviceListDataSize),
				"getting device list size");
	
	if(deviceListDataSize > 0) {
		const UInt32 deviceCount = deviceListDataSize / sizeof(AudioDeviceID);
		AudioDeviceID deviceList[deviceCount];
		OFXAU_PRINT(AudioObjectGetPropertyData(kAudioObjectSystemObject,
											   &deviceListProp,
											   0,
											   NULL,
											   &deviceListDataSize,
											   deviceList),
					"getting device list");
		
		return std::vector<AudioDeviceID>(deviceList, deviceList + deviceCount);
	} else {
		return std::vector<AudioDeviceID>();
	}
}

std::vector<AudioDeviceID> AudioOutputDeviceList()
{
	return AudioDeviceList();
}

std::vector<AudioDeviceID> AudioInputDeviceList()
{
	return AudioDeviceList();
}

#pragma mark - Device Info

std::string DeviceNameForAudioDeviceID(AudioDeviceID deviceID)
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
	
	std::string deviceNameString("Unknown");
	
	if(deviceName) {
		char buffer[255];
		CFStringGetCString(deviceName, buffer, 255, kCFStringEncodingUTF8);
		deviceNameString = std::string(buffer);
		CFRelease(deviceName);
	}
	
	return deviceNameString;
}
