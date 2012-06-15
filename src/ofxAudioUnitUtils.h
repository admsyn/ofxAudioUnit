#pragma once

#include "ofTypes.h"

class ofxAudioUnitTap;
class ofxAudioUnit;

typedef ofPtr<AudioUnit> AudioUnitRef;

struct ofxAudioUnitTapContext
{
	ofPtr<AudioUnit> sourceUnit;
	AudioBufferList * trackedSamples;
	ofMutex * bufferMutex;
};

// Container for samples returned from an ofxAudioUnitTap
typedef struct ofxAudioUnitTapSamples
{
	std::vector<AudioUnitSampleType> left;
	std::vector<AudioUnitSampleType> right;
	size_t size(){return min(left.size(), right.size());}
}
ofxAudioUnitTapSamples;

// these macros make the "do core audio thing, check for error" process less repetitive
#define OFXAU_PRINT(s, stage)\
if(s!=noErr){\
	std::cout << "Error " << (OSStatus)s << " while " << stage << std::endl;\
}

#define OFXAU_RETURN(s, stage)\
if(s!=noErr){\
	std::cout << "Error " << (OSStatus)s << " while " << stage << std::endl;\
	return;\
}

#define OFXAU_RET_BOOL(s, stage)\
if(s!=noErr){\
	std::cout << "Error " << (OSStatus)s << " while " << stage << std::endl;\
	return false;\
}\
return true;

#define OFXAU_RET_FALSE(s, stage)\
if(s!=noErr){\
	std::cout << "Error " << (OSStatus)s << " while " << stage << std::endl;\
	return false;\
}

#define OFXAU_RET_STATUS(s, stage)\
OSStatus stat = s;\
if(stat!=noErr){\
	std::cout << "Error " << (OSStatus)s << " while " << stage << std::endl;\
	return stat;\
}
