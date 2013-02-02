#include "ofxAudioUnitFftNode.h"
#include <math.h>

ofxAudioUnitFftNode::ofxAudioUnitFftNode(unsigned int fftBufferSize, ofxAudioUnitWindowType windowType)
: _currentMaxLog2N(0)
, _windowType(windowType)
{
	setFftBufferSize(fftBufferSize);
}

ofxAudioUnitFftNode::~ofxAudioUnitFftNode()
{
	freeBuffers();
}

void ofxAudioUnitFftNode::freeBuffers()
{
	if(_fftSetup)      vDSP_destroy_fftsetup(_fftSetup);
	if(_fftData.realp) free(_fftData.realp);
	if(_fftData.imagp) free(_fftData.imagp);
	if(_window)        free(_window);
}

void generateWindow(ofxAudioUnitWindowType windowType, float * window, size_t size)
{
	switch (windowType) {
		case OFXAU_WINDOW_HAMMING:
			vDSP_hamm_window(window, size, 0);
			break;
			
		case OFXAU_WINDOW_HANNING:
			vDSP_hann_window(window, size, 0);
			break;
			
		case OFXAU_WINDOW_BLACKMAN:
			vDSP_blkman_window(window, size, 0);
			break;
	}
}

void ofxAudioUnitFftNode::setFftBufferSize(unsigned int bufferSize)
{
	_log2N = (unsigned int) ceilf(log2f(bufferSize));
	_N = 1 << _log2N;
	
	// if the new buffer size is bigger than what we've allocated for,
	// free everything and allocate anew (otherwise re-use)
	if(_log2N > _currentMaxLog2N) {
		freeBuffers();
		_fftData.realp = (float *)calloc(_N / 2, sizeof(float));
		_fftData.imagp = (float *)calloc(_N / 2, sizeof(float));
		_window = (float *)calloc(_N, sizeof(float));
		_fftSetup = vDSP_create_fftsetup(_log2N, kFFTRadix2);
		_currentMaxLog2N = _log2N;
	}

	generateWindow(_windowType, _window, _N);
	setBufferSize(_N);
}

void ofxAudioUnitFftNode::setWindowType(ofxAudioUnitWindowType windowType)
{
	_windowType = windowType;
	generateWindow(_windowType, _window, _N);
}

bool ofxAudioUnitFftNode::getFft(FftSample &outSample, bool logarithmic, bool normalize)
{
	getSamplesFromChannel(_sampleBuffer, 0);
	
	if(_sampleBuffer.size() < _N) {
		outSample.clear();
		return false;
	}
	
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
	return true;
}
