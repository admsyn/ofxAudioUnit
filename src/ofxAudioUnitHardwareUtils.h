#pragma once

#include <vector>
#include <string>
#import <AudioToolbox/AudioToolbox.h>

std::vector<AudioDeviceID> AudioDeviceList();
std::vector<AudioDeviceID> AudioDeviceListForScope(AudioObjectPropertyScope scope);
std::vector<AudioDeviceID> AudioOutputDeviceList(){return AudioDeviceListForScope(kAudioDevicePropertyScopeOutput);}
std::vector<AudioDeviceID> AudioInputDeviceList(){return AudioDeviceListForScope(kAudioDevicePropertyScopeInput);}

AudioDeviceID DefaultAudioInputDevice();
AudioDeviceID DefaultAudioOutputDevice();

std::string AudioDeviceName(AudioDeviceID deviceID);
std::string AudioDeviceManufacturer(AudioDeviceID deviceID);
