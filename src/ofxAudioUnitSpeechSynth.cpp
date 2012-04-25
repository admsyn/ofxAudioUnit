#include "ofxAudioUnit.h"

AudioComponentDescription speechDesc = {
	kAudioUnitType_Generator,
	kAudioUnitSubType_SpeechSynthesis,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitSpeechSynth::ofxAudioUnitSpeechSynth()
// ----------------------------------------------------------
{
	_desc = speechDesc;
	initUnit();
	UInt32 dataSize = sizeof(SpeechChannel);
	
	ERR_CHK(AudioUnitGetProperty(*_unit,
															 kAudioUnitProperty_SpeechChannel,
															 kAudioUnitScope_Global,
															 0,
															 &_channel,
															 &dataSize),
					"getting speech channel");
}

// ----------------------------------------------------------
void ofxAudioUnitSpeechSynth::say(std::string phrase)
// ----------------------------------------------------------
{
	CFStringRef string = CFStringCreateWithCString(kCFAllocatorDefault, 
																								 phrase.c_str(), 
																								 kCFStringEncodingUTF8);
	
	SpeakCFString(_channel, string, NULL);
	
	CFRelease(string);
}

// ----------------------------------------------------------
void ofxAudioUnitSpeechSynth::stop()
// ----------------------------------------------------------
{
	StopSpeech(_channel);
}

// ----------------------------------------------------------
void ofxAudioUnitSpeechSynth::printAvailableVoices()
// ----------------------------------------------------------
{
	vector<string> voiceNames = getAvailableVoices();
	for(int i = 0; i < voiceNames.size(); i++)
	{
		cout << i+1 << ":\t" << voiceNames.at(i) << endl;
	}
}

// ----------------------------------------------------------
vector<string> ofxAudioUnitSpeechSynth::getAvailableVoices()
// ----------------------------------------------------------
{
	SInt16 numVoices = 0;
	CountVoices(&numVoices);
	vector<string> voiceNames;
	
	// voices seem to be 1-indexed instead of 0-indexed
	for(int i = 1; i <= numVoices; i++)
	{
		VoiceSpec vSpec;
		VoiceDescription vDesc;
		GetIndVoice(i, &vSpec);
		GetVoiceDescription(&vSpec, &vDesc, sizeof(VoiceDescription));
		string name = string((const char *)vDesc.name);
		
		// the first "character" in vDesc.name is actually just the length
		// of the string. We're tossing it out here by making a substring.
		voiceNames.push_back(string(name, 1, name[0]));
	}
	return voiceNames;
}

// ----------------------------------------------------------
bool ofxAudioUnitSpeechSynth::setVoice(int voiceIndex)
// ----------------------------------------------------------
{
	VoiceSpec vSpec;
	OSErr err;
	err = GetIndVoice(voiceIndex, &vSpec);
	
	if(!err)
	{
		StopSpeech(_channel);
		err = SetSpeechInfo(_channel, soCurrentVoice, &vSpec);
	}
	
	return (err == 0);
}

// ----------------------------------------------------------
bool ofxAudioUnitSpeechSynth::setVoice(string voiceName)
// ----------------------------------------------------------
{
	vector<string> voiceNames = getAvailableVoices();
	
	for(int i = 0; i < voiceNames.size(); i++)
	{
		if(voiceNames.at(i) == voiceName)
		{
			return setVoice(i+1);
		}
	}
	
	return false;
}
