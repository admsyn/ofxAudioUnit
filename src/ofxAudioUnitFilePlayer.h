#pragma once

#include "ofxAudioUnitBase.h"

// ofxAudioUnitFilePlayer wraps the AUAudioFilePlayer unit.
// This audio unit allows you to play any file that
// Core Audio supports (mp3, aac, caf, aiff, etc)

class ofxAudioUnitFilePlayer : public ofxAudioUnit
{
public:
	ofxAudioUnitFilePlayer();
	~ofxAudioUnitFilePlayer();
	
	bool   setFile(const std::string &filePath);
	
	// you can use this pair of functions to make sure two
	// file players stay in sync while looping. Call setLength()
	// on one using the return value of getLength() from another.
	UInt32 getLength() const;
	void   setLength(UInt32 length);
	
	enum {
		OFX_AU_LOOP_FOREVER = -1
	};
	
	// You can get the startTime arg from mach_absolute_time().
	// Note that all of these args are optional; you can just
	// call play() / loop() and it will start right away.
	void play(uint64_t startTime = 0);
	void loop(unsigned int timesToLoop = OFX_AU_LOOP_FOREVER, uint64_t startTime = 0);
	void stop(){reset();}
	AudioTimeStamp pause(); // returns the timestamp the file player is paused at
	
	AudioTimeStamp getCurrentTimestamp() const;
	
private:
	AudioFileID _fileID[1];
	ScheduledAudioFileRegion _region;
	AudioTimeStamp _pauseTimeStamp;
	Float64 _pauseTimeAccumulator;
	unsigned int _loopCount;
};
