#include "ofxAudioUnitFftNode.h"

ofxAudioUnitFftNode::ofxAudioUnitFftNode()
: _log2N(10)
, _N(1 << _log2N)
{
	_log2N = 10;
	_N = 1 << _log2N;
	_fftSetup = vDSP_create_fftsetup(_log2N, kFFTRadix2);
	_fftData.realp = (float *)calloc(_N / 2, sizeof(float));
	_fftData.imagp = (float *)calloc(_N / 2, sizeof(float));
	_window = (float *)calloc(_N, sizeof(float));
	vDSP_hamm_window(_window, _N, 0);
}

ofxAudioUnitFftNode::~ofxAudioUnitFftNode()
{
	vDSP_destroy_fftsetup(_fftSetup);
	free(_fftData.realp);
	free(_fftData.imagp);
	free(_window);
}

void ofxAudioUnitFftNode::getFft(FftSample &outSample, bool logarithmic, bool normalize)
{
	getSamplesFromChannel(_sampleBuffer, 0);
	
	if(_sampleBuffer.size() < _N) return;
	
	float timeDomainMax;
	vDSP_maxv(&_sampleBuffer[0], 1, &timeDomainMax, _sampleBuffer.size());
	vDSP_vsdiv(&_sampleBuffer[0], 1, &timeDomainMax, &_sampleBuffer[0], 1, _sampleBuffer.size());
	
	vDSP_vmul(&_sampleBuffer[0], 1, _window, 1, &_sampleBuffer[0], 1, _N);
	vDSP_ctoz((COMPLEX *) &_sampleBuffer[0], 2, &_fftData, 1, _N/2);
	vDSP_fft_zrip(_fftSetup, &_fftData, 1, _log2N, kFFTDirection_Forward);
	
	_fftData.imagp[0] = 0.0f;	
	vDSP_zvmags(&_fftData, 1, _fftData.realp, 1, _N/2);

	if(logarithmic) {
		for(int i = 0; i < _N/2; i++) {
			_fftData.realp[i] = log10f(_fftData.realp[i] + 1);
		}
	}
	
	if(normalize) {
		float max;
		vDSP_maxv(_fftData.realp, 1, &max, _N / 2);
		vDSP_vsdiv(_fftData.realp, 1, &max, _fftData.realp, 1, _N / 2);
	}
	
	outSample.assign(_fftData.realp, _fftData.realp + _N/2);
}
