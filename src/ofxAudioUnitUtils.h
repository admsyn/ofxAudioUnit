#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <iostream>
#include <sstream>

static AudioBufferList * AudioBufferListAlloc(UInt32 channels, UInt32 samplesPerChannel)
{
	AudioBufferList * bufferList = NULL;
	size_t bufferListSize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * channels);
	bufferList = (AudioBufferList *)calloc(1, bufferListSize);
	bufferList->mNumberBuffers = channels;
	
	for(UInt32 i = 0; i < bufferList->mNumberBuffers; i++) {
		bufferList->mBuffers[i].mNumberChannels = 1;
		bufferList->mBuffers[i].mDataByteSize   = samplesPerChannel * sizeof(Float32);
		bufferList->mBuffers[i].mData           = calloc(samplesPerChannel, sizeof(Float32));
	}
	return bufferList;
}

static void AudioBufferListRelease(AudioBufferList * bufferList)
{
	for(int i = 0; i < bufferList->mNumberBuffers; i++) {
		free(bufferList->mBuffers[i].mData);
	}
	
	free(bufferList);
}

static std::string StringForDescription(const AudioComponentDescription &desc)
{
	std::stringstream ss;
	unsigned char * c = (unsigned char *)&desc.componentType;
	ss << c[3] << c[2] << c[1] << c[0] << ", ";
	ss << c[7] << c[6] << c[5] << c[4] << ", ";
	ss << c[11] << c[10] << c[9] << c[8];
	return ss.str();
}

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
