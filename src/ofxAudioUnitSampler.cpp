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

// ----------------------------------------------------------
bool ofxAudioUnitSampler::setSample(const std::string &samplePath)
// ----------------------------------------------------------
{
	CFURLRef sampleURL[1] = {CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
																	 (const UInt8 *)samplePath.c_str(),
																	 samplePath.length(),
																	 NULL)};
	
	CFArrayRef sample = CFArrayCreate(NULL, (const void **)&sampleURL, 1, &kCFTypeArrayCallBacks);

	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUSamplerProperty_LoadAudioFiles,
									 kAudioUnitScope_Global,
									 0,
									 &sample,
									 sizeof(sample)),
				"setting ofxAudioUnitSampler's source sample");
	
	CFRelease(sample);
	CFRelease(sampleURL[0]);
	
	return true;
}

// ----------------------------------------------------------
bool ofxAudioUnitSampler::setSamples(const std::vector<std::string> &samplePaths)
// ----------------------------------------------------------
{
	CFURLRef sampleURLs[samplePaths.size()];
	
	for(int i = 0; i < samplePaths.size(); i++)
	{
		sampleURLs[i] = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
																(const UInt8 *)samplePaths[i].c_str(),
																samplePaths[i].length(),
																NULL);
	}
	
	CFArrayRef samples = CFArrayCreate(NULL, (const void **)&sampleURLs, samplePaths.size(), &kCFTypeArrayCallBacks);
	
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAUSamplerProperty_LoadAudioFiles,
									 kAudioUnitScope_Global,
									 0,
									 &samples,
									 sizeof(samples)),
				"setting ofxAudioUnitSampler's source samples");
	
	for(int i = 0; i < samplePaths.size(); i++) CFRelease(sampleURLs[i]);
	
	CFRelease(samples);
}
