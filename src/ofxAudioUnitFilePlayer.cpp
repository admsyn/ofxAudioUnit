#include "ofxAudioUnit.h"

AudioComponentDescription filePlayerDesc = {
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
	AudioFileClose(fileID[0]);
}

// ----------------------------------------------------------
bool ofxAudioUnitFilePlayer::setFile(const std::string &filePath)
// ----------------------------------------------------------
{
	if(!_unit) return false;
	
	CFURLRef fileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
															   (const UInt8 *)filePath.c_str(),
															   filePath.length(),
															   NULL);
	
	if(fileID[0]) AudioFileClose(fileID[0]);
	
	OSStatus s = AudioFileOpenURL(fileURL, kAudioFileReadPermission, 0, fileID);
	
	CFRelease(fileURL);
	
	if(s != noErr)
	{
		cout << "Error " << s << " while opening file at " << filePath << endl;
		return false;
	}
	
	UInt64 numPackets = 0;
	UInt32 dataSize = sizeof(numPackets);
	
	AudioFileGetProperty(fileID[0], kAudioFilePropertyAudioDataPacketCount, &dataSize, &numPackets);
	
	AudioStreamBasicDescription asbd = {0};
	dataSize = sizeof(asbd);
	
	AudioFileGetProperty(fileID[0], kAudioFilePropertyDataFormat, &dataSize, &asbd);
	
	// defining a region which basically says "play the whole file"
	memset(&region, 0, sizeof(region));
	region.mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
	region.mTimeStamp.mSampleTime = 0;
	region.mCompletionProc = NULL;
	region.mCompletionProcUserData = NULL;
	region.mAudioFile = fileID[0];
	region.mLoopCount = 0;
	region.mStartFrame = 0;
	region.mFramesToPlay = numPackets * asbd.mFramesPerPacket;
	
	// setting the file ID now since it seems to have some overhead.
	// Doing it now ensures you'll get sound pretty much instantly after
	// calling play()
	OFXAU_RET_BOOL(AudioUnitSetProperty(*_unit,
										kAudioUnitProperty_ScheduledFileIDs,
										kAudioUnitScope_Global,
										0, 
										fileID, 
										sizeof(fileID)),
				   "setting file player's file ID");
}

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::play()
// ----------------------------------------------------------
{
	if(!(region.mTimeStamp.mFlags & kAudioTimeStampSampleTimeValid))
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
									  fileID, 
									  sizeof(fileID)),
				 "setting file player's file ID");
	
	OFXAU_RETURN(AudioUnitSetProperty(*_unit,
									  kAudioUnitProperty_ScheduledFileRegion,
									  kAudioUnitScope_Global,
									  0,
									  &region,
									  sizeof(region)),
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
	region.mLoopCount = timesToLoop;
	play();
}

// ----------------------------------------------------------
void ofxAudioUnitFilePlayer::stop()
// ----------------------------------------------------------
{
	reset();
}
