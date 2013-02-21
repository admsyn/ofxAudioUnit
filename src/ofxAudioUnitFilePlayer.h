#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitFilePlayer wraps the AUAudioFilePlayer unit.
// This audio unit allows you to play any file that
// Core Audio supports (mp3, aac, caf, aiff, etc)

class ofxAudioUnitFilePlayer : public ofxAudioUnit
{
	AudioFileID _fileID[1];
	ScheduledAudioFileRegion _region;
	
public:
	ofxAudioUnitFilePlayer();
	~ofxAudioUnitFilePlayer();
	
	bool   setFile(const std::string &filePath);
	UInt32 getLength();
	void   setLength(UInt32 length);
	
	enum {
		OFX_AU_LOOP_FOREVER = -1
	};
	
	// You can get the startTime arg from mach_absolute_time().
	// Note that all of these args are optional; you can just
	// call play() / loop() and it will start right away.
	void play(uint64_t startTime = 0);
	void loop(unsigned int timesToLoop = OFX_AU_LOOP_FOREVER, uint64_t startTime = 0);
	void stop();
};
