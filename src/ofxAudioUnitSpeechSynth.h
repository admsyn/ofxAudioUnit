#pragma once

#include "ofxAudioUnitBase.h"
#include <ApplicationServices/ApplicationServices.h>

// ofxAudioUnitSpeechSynth wraps the AUSpeechSynthesis unit.
// This unit lets you access the Speech Synthesis API
// for text-to-speech on your mac (the same thing that
// powers the VoiceOver utility).

// This unit is not available when compiling in 32 bit mode
// on Yosemite

class ofxAudioUnitSpeechSynth : public ofxAudioUnit
{
	SpeechChannel _channel;
public:
	ofxAudioUnitSpeechSynth();
	
	void say(const std::string &phrase);
	void stop();
	
	void printAvailableVoices();
	std::vector<std::string>getAvailableVoices();
	bool setVoice(int voiceIndex);
	bool setVoice(const std::string &voiceName);
	
	SpeechChannel getSpeechChannel(){return _channel;}
};
