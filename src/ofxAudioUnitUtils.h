#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include "ofTypes.h"

class ofxAudioUnitTap;
class ofxAudioUnit;

typedef ofPtr<AudioUnit> AudioUnitRef;

// Container for samples returned from an ofxAudioUnitTap
typedef struct ofxAudioUnitTapSamples
{
	std::vector<AudioUnitSampleType> left;
	std::vector<AudioUnitSampleType> right;
	size_t size(){return min(left.size(), right.size());}
}
ofxAudioUnitTapSamples;

AudioBufferList * allocBufferList(int channels = 2, size_t size = 512);
void releaseBufferList(AudioBufferList * bufferList);

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
