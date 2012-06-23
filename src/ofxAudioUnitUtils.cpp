#include "ofxAudioUnitUtils.h"

AudioBufferList * allocBufferList(int channels, size_t size)
{
	AudioBufferList * bufferList;
	size_t bufferSize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * channels);
	bufferList = (AudioBufferList *)malloc(bufferSize);
	bufferList->mNumberBuffers = channels;
	
	for(UInt32 i = 0; i < bufferList->mNumberBuffers; i++)
	{
		bufferList->mBuffers[i].mNumberChannels = 1;
		bufferList->mBuffers[i].mDataByteSize = sizeof(AudioUnitSampleType) * size;
		bufferList->mBuffers[i].mData = malloc(sizeof(AudioUnitSampleType)  * size);
		memset(bufferList->mBuffers[i].mData, 0, bufferList->mBuffers[i].mDataByteSize);
	}
	return bufferList;
}

void releaseBufferList(AudioBufferList * bufferList)
{
	for(int i = 0; i < bufferList->mNumberBuffers; i++)
		free(bufferList->mBuffers[i].mData);
	
	free(bufferList);
}
