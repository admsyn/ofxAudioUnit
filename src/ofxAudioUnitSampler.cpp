#include "ofxAudioUnit.h"

AudioComponentDescription samplerDesc = {
	kAudioUnitType_MusicDevice,
	kAudioUnitSubType_Sampler,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitSampler::ofxAudioUnitSampler()
// ----------------------------------------------------------
{
	_desc = samplerDesc;
	initUnit();
}

//CFArrayRef of CFURLRef

// ----------------------------------------------------------
bool ofxAudioUnitSampler::setSample(std::string samplePath)
// ----------------------------------------------------------
{
	CFURLRef sampleURL[1] = {CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
																	 (const UInt8 *)samplePath.c_str(),
																	 samplePath.length(),
																	 NULL)};
	
	CFArrayRef samples = CFArrayCreate(NULL, (const void **)&sampleURL, 1, &kCFTypeArrayCallBacks);

	OFXAU_RET_BOOL(AudioUnitSetProperty(*_unit,
						 kAUSamplerProperty_LoadAudioFiles,
						 kAudioUnitScope_Global,
						 0,
						 &samples,
						 sizeof(samples)),
				   "setting ofxAudioUnitSampler's source sample");
	
	CFRelease(samples);
	CFRelease(sampleURL[0]);
}
