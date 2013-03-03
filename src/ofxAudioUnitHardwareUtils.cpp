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

std::vector<AudioDeviceID> AudioDeviceListForScope(AudioObjectPropertyScope scope)
{
	std::vector<AudioDeviceID> allDevices = AudioDeviceList();
	std::vector<AudioDeviceID> validDevices;
	
	AudioObjectPropertyAddress outStreamProp = {
		.mSelector = kAudioDevicePropertyStreamConfiguration,
		.mScope    = scope,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	for(int i = 0; i < allDevices.size(); i++) {
		OSStatus s;
		UInt32 dataSize;
		s = AudioObjectGetPropertyDataSize(allDevices[i], &outStreamProp, 0, NULL, &dataSize);
		if(s != noErr) continue;
		
		AudioBufferList * streamFormat = (AudioBufferList *)malloc(dataSize);
		s = AudioObjectGetPropertyData(allDevices[i], &outStreamProp, 0, NULL, &dataSize, streamFormat);
		
		if(s == noErr && streamFormat->mNumberBuffers > 0) {
			validDevices.push_back(allDevices[i]);
		}
		
		free(streamFormat);
	}
	
	return validDevices;
}

std::vector<AudioDeviceID> AudioOutputDeviceList()
{
	return AudioDeviceListForScope(kAudioDevicePropertyScopeOutput);
}

std::vector<AudioDeviceID> AudioInputDeviceList()
{
	return AudioDeviceListForScope(kAudioDevicePropertyScopeInput);
}

#pragma mark - Device Info

std::string StringForPropertyOnDevice(AudioObjectPropertyAddress prop, AudioDeviceID deviceID)
{
	CFStringRef stringRef;
	UInt32 dataSize = sizeof(stringRef);
	OFXAU_PRINT(AudioObjectGetPropertyData(deviceID, &prop, 0, NULL, &dataSize, &stringRef), "getting device info");
	
	std::string propertyString("Unknown");
	
	if(stringRef) {
		CFIndex bufferSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(stringRef), kCFStringEncodingUTF8);
		char buffer[bufferSize];
		CFStringGetCString(stringRef, buffer, bufferSize, kCFStringEncodingUTF8);
		propertyString = std::string(buffer);
		CFRelease(stringRef);
	}
	
	return propertyString;
}

std::string AudioDeviceName(AudioDeviceID deviceID)
{
	AudioObjectPropertyAddress deviceNameProp = {
		.mSelector = kAudioObjectPropertyName,
		.mScope    = kAudioObjectPropertyScopeGlobal,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	return StringForPropertyOnDevice(deviceNameProp, deviceID);
}

std::string AudioDeviceManufacturer(AudioDeviceID deviceID)
{
	AudioObjectPropertyAddress deviceManuProp = {
		.mSelector = kAudioObjectPropertyManufacturer,
		.mScope    = kAudioObjectPropertyScopeGlobal,
		.mElement  = kAudioObjectPropertyElementMaster
	};
	
	return StringForPropertyOnDevice(deviceManuProp, deviceID);
}
