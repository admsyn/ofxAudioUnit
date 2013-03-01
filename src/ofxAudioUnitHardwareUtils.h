#pragma once

#include <vector>
#include <string>
#import <AudioToolbox/AudioToolbox.h>

std::vector<AudioDeviceID> AudioDeviceList();
std::vector<AudioDeviceID> AudioOutputDeviceList();
std::vector<AudioDeviceID> AudioInputDeviceList();

std::string DeviceNameForAudioDeviceID(AudioDeviceID deviceID);
