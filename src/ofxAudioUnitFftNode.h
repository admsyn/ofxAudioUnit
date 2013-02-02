#pragma once

#include "ofxAudioUnitDSPNode.h"
#include <Accelerate/Accelerate.h>

class ofxAudioUnitFftNode : public ofxAudioUnitDSPNode {
public:
	ofxAudioUnitFftNode();
	~ofxAudioUnitFftNode();
	
	typedef std::vector<float> FftSample;
	void getFft(FftSample &outSample, bool logarithmic = true, bool normalize = true);
	
private:
	unsigned int _N;
	unsigned int _log2N;
	float _normalizationFactor;
	FFTSetup _fftSetup;
	COMPLEX_SPLIT _fftData;
	float * _window;
	std::vector<AudioUnitSampleType> _sampleBuffer;
	FftSample _fftBuffer;
};
