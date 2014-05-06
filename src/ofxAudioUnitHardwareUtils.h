#pragma once

#include "TargetConditionals.h"
#if !TARGET_OS_IPHONE

#include <vector>
#include <string>
#import <AudioToolbox/AudioToolbox.h>

std::vector<AudioDeviceID> AudioDeviceList();
std::vector<AudioDeviceID> AudioDeviceListForScope(AudioObjectPropertyScope scope);
std::vector<AudioDeviceID> AudioOutputDeviceList();
std::vector<AudioDeviceID> AudioInputDeviceList();

AudioDeviceID DefaultAudioInputDevice();
AudioDeviceID DefaultAudioOutputDevice();

std::string AudioDeviceName(AudioDeviceID deviceID);
std::string AudioDeviceManufacturer(AudioDeviceID deviceID);

#endif // !TARGET_OS_IPHONE
