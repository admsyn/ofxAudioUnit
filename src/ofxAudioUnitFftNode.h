#pragma once

#include "ofxAudioUnitDSPNode.h"
#include <Accelerate/Accelerate.h>

class ofxAudioUnitFftNode : public ofxAudioUnitDSPNode {
public:
	ofxAudioUnitFftNode(unsigned int fftBufferSize = 1024);
	~ofxAudioUnitFftNode();
	
	typedef std::vector<float> FftSample;
	bool getFft(FftSample &outSample, bool logarithmic = true, bool normalize = true);
	
	// this should be set to a power of 2 (512, 1024, 2048, etc), and will round up otherwise
	void setFftBufferSize(unsigned int bufferSize);
	
private:
	unsigned int _N;
	unsigned int _log2N;
	unsigned int _currentMaxLog2N;
	float _normalizationFactor;
	FFTSetup _fftSetup;
	COMPLEX_SPLIT _fftData;
	float * _window;
	std::vector<AudioUnitSampleType> _sampleBuffer;
	FftSample _fftBuffer;
	void freeBuffers();
};
