#pragma once

#include <vector>
#include <string>
#import <AudioToolbox/AudioToolbox.h>

std::vector<AudioDeviceID> AudioDeviceList();
std::vector<AudioDeviceID> AudioDeviceListForScope(AudioObjectPropertyScope scope);
std::vector<AudioDeviceID> AudioOutputDeviceList();
std::vector<AudioDeviceID> AudioInputDeviceList();

std::string AudioDeviceName(AudioDeviceID deviceID);
std::string AudioDeviceManufacturer(AudioDeviceID deviceID);
