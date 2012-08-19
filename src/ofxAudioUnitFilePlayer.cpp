#include "ofxAudioUnit.h"

AudioComponentDescription filePlayerDesc =
{
	kAudioUnitType_Generator,
	kAudioUnitSubType_AudioFilePlayer,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitFilePlayer::ofxAudioUnitFilePlayer()
// ----------------------------------------------------------
{
	_desc = filePlayerDesc;
	initUnit();
}

// ----------------------------------------------------------
ofxAudioUnitFilePlayer::~ofxAudioUnitFilePlayer()
// ----------------------------------------------------------
{
	stop();
	AudioFileClose(_fileID[0]);
}

#pragma mark - Properties

// ----------------------------------------------------------
bool ofxAudioUnitFilePlayer::setFile(const std::string &filePath)
// ----------------------------------------------------------
{
	CFURLRef fileURL;
	fileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
	                                                  (const UInt8 *)filePath.c_str(),
	                                                  filePath.length(),
	                                                  NULL);
	
	if(_fileID[0]) AudioFileClose(_fileID[0]);
	
	OSStatus s = AudioFileOpenURL(fileURL, kAudioFileReadPermission, 0, _fileID);
	
	CFRelease(fileURL);
	
	if(s != noErr)
	{
		cout << "Error " << s << " while opening file at " << filePath << endl;
		return false;
	}
	
	UInt64 numPackets = 0;
	UInt32 dataSize   = sizeof(numPackets);
	
	AudioFileGetProperty(_fileID[0],
	                     kAudioFilePropertyAudioDataPacketCount,
	                     &dataSize,
	                     &numPackets);
	
	AudioStreamBasicDescription asbd = {0};
	dataSize = sizeof(asbd);
	
	AudioFileGetProperty(_fileID[0], kAudioFilePropertyDataFormat, &dataSize, &asbd);
	
	// defining a region which basically says "play the whole file"
	memset(&_region, 0, sizeof(_region));
	_region.mTimeStamp.mFlags       = kAudioTimeStampSampleTimeValid;
	_region.mTimeStamp.mSampleTime  = 0;
	_region.mCompletionProc         = NULL;
	_region.mCompletionProcUserData = NULL;
	_region.mAudioFile              = _fileID[0];
	_region.mLoopCount              = 0;
	_region.mStartFrame             = 0;
	_region.mFramesToPlay           = numPackets * asbd.mFramesPerPacket;
	
	// setting the file ID now since it seems to have some overhead.
	// Doing it now ensures you'll get sound pretty much instantly after
	// calling play()
	OFXAU_RET_BOOL(AudioUnitSetProperty(*_unit,
	                                    kAudioUnitProperty_ScheduledFileIDs,
	                                    kAudioUnitScope_Global,
	                                    0,
	                                    _fileID,
	                                    sizeof(_fileID)),
	               "setting file player's file ID");
}

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::setLength(UInt32 length)
// ----------------------------------------------------------
{
	_region.mFramesToPlay = length;
}

// ----------------------------------------------------------
UInt32 ofxAudioUnitFilePlayer::getLength()
// ----------------------------------------------------------
{
	return _region.mFramesToPlay;
}

#pragma mark - Playback

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::play()
// ----------------------------------------------------------
{
	if(!(_region.mTimeStamp.mFlags & kAudioTimeStampSampleTimeValid))
	{
		cout << "ofxAudioUnitFilePlayer has no file to play" << endl;
		return;
	}
	else if(!_unit)
	{
		return;
	}
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduledFileIDs,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  _fileID,
	                                  sizeof(_fileID)),
	             "setting file player's file ID");
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduledFileRegion,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  &_region,
	                                  sizeof(_region)),
	             "setting file player region");
	
	AudioTimeStamp startTime = {0};
	startTime.mFlags = kAudioTimeStampSampleTimeValid;
	startTime.mSampleTime = -1;
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
	                                  kAudioUnitProperty_ScheduleStartTimeStamp,
	                                  kAudioUnitScope_Global,
	                                  0,
	                                  &startTime,
	                                  sizeof(startTime)),
	             "setting file player start time");
}

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::loop(unsigned int timesToLoop)
// ----------------------------------------------------------
{
	_region.mLoopCount = timesToLoop;
	play();
}

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::stop()
// ----------------------------------------------------------
{
	reset();
}
