#include "TargetConditionals.h"
#if !TARGET_OS_IPHONE

#include "ofxAudioUnit.h"
#include <Accelerate/Accelerate.h>

ofxAudioUnitTap::ofxAudioUnitTap(unsigned int samplesToTrack) {
	setBufferSize(samplesToTrack);
}

ofxAudioUnitTap::~ofxAudioUnitTap() {
	
}

ofxAudioUnitTap::ofxAudioUnitTap(const ofxAudioUnitTap& orig) {
	setBufferSize(orig.getBufferSize());
}

ofxAudioUnitTap& ofxAudioUnitTap::operator=(const ofxAudioUnitTap &orig) {
	setBufferSize(orig.getBufferSize());
	return *this;
}

void ofxAudioUnitTap::setBufferLength(unsigned int samplesToBuffer) {
	setBufferSize(samplesToBuffer);
}

#pragma mark - Samples

void ofxAudioUnitTap::getSamples(MonoSamples &outData, unsigned channel) const {
	getSamplesFromChannel(outData, channel);
}

void ofxAudioUnitTap::getSamples(StereoSamples &outData) const {
	getSamplesFromChannel(outData.left, 0);
	getSamplesFromChannel(outData.right, 1);
}

void ofxAudioUnitTap::getLeftSamples(MonoSamples &outData) const {
	getSamplesFromChannel(outData, 0);
}

void ofxAudioUnitTap::getRightSamples(MonoSamples &outData) const {
	getSamplesFromChannel(outData, 1);
}

#pragma mark - RMS

float ofxAudioUnitTap::getRMS(unsigned int channel) {
	getSamplesFromChannel(_tempBuffer, channel);
	float rms;
	vDSP_rmsqv(&_tempBuffer[0], 1, &rms, _tempBuffer.size());
	return rms;
}

#pragma mark - Waveforms

void WaveformForBuffer(Float32 * begin, size_t length, float w, float h, ofPolyline &outLine, unsigned rate) {
	const size_t size = length / rate;
	
	if(size == 0) {
		outLine.clear();
		return;
	}
	
	if(outLine.size() != size) {
		outLine.resize(size);
	}
	
	float * v = (float *)&outLine[0];
	float zero = 0;
	float half = h / 2.;
	vDSP_vsmsa(begin, rate, &half, &half, v + 1, 3, size); // multiply and add "y"s
	vDSP_vgen(&zero, &w, v, 3, size); // generate "x"s
}

void ofxAudioUnitTap::getWaveform(ofPolyline &l, float w, float h, unsigned chan, unsigned rate) {
	getSamples(_tempBuffer, chan);
	WaveformForBuffer(&_tempBuffer[0], _tempBuffer.size(), w, h, l, rate);
}

void ofxAudioUnitTap::getLeftWaveform(ofPolyline &l, float w, float h, unsigned rate) {
	getWaveform(l, w, h, 0, rate);
}

void ofxAudioUnitTap::getRightWaveform(ofPolyline &l, float w, float h, unsigned rate) {
	getWaveform(l, w, h, 1, rate);
}

void ofxAudioUnitTap::getStereoWaveform(ofPolyline &l, ofPolyline &r, float w, float h, unsigned rate) {
	getLeftWaveform(l, w, h, rate);
	getRightWaveform(r, w, h, rate);
}

ofPolyline ofxAudioUnitTap::getWaveform(float w, float h, unsigned chan, unsigned rate) {
	getWaveform(_tempWave, w, h, chan, rate);
	return _tempWave;
}

ofPolyline ofxAudioUnitTap::getLeftWaveform(float w, float h, unsigned rate) {
	return getWaveform(w, h, 0, rate);
}

ofPolyline ofxAudioUnitTap::getRightWaveform(float w, float h, unsigned rate) {
	return getWaveform(w, h, 1, rate);
}

#endif // !TARGET_OS_IPHONE
