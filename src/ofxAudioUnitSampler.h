#pragma once

#include "ofxAudioUnitBase.h"

class ofxAudioUnitSampler : public ofxAudioUnit
{
public:
	ofxAudioUnitSampler();
	
	bool setSample(const std::string &samplePath);
	bool setSamples(const std::vector<std::string> &samplePaths);
};
