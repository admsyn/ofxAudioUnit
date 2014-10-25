#include "TargetConditionals.h"
#if !TARGET_OS_IPHONE

#include "ofxAudioUnit.h"
#include <Accelerate/Accelerate.h>

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap(unsigned int samplesToTrack)
// ----------------------------------------------------------
{
	setBufferSize(samplesToTrack);
}

// ----------------------------------------------------------
ofxAudioUnitTap::~ofxAudioUnitTap()
// ----------------------------------------------------------
{
	
}

// ----------------------------------------------------------
ofxAudioUnitTap::ofxAudioUnitTap(const ofxAudioUnitTap& orig)
// ----------------------------------------------------------
{
	setBufferSize(orig.getBufferSize());
}

// ----------------------------------------------------------
ofxAudioUnitTap& ofxAudioUnitTap::operator=(const ofxAudioUnitTap &orig)
// ----------------------------------------------------------
{
	setBufferSize(orig.getBufferSize());
	return *this;
}

// ----------------------------------------------------------
void ofxAudioUnitTap::setBufferLength(unsigned int samplesToBuffer)
// ----------------------------------------------------------
{
	setBufferSize(samplesToBuffer);
}

#pragma mark - Samples

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData, 0);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(MonoSamples &outData, unsigned int channel) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData, channel);
}

// ----------------------------------------------------------
void ofxAudioUnitTap::getSamples(StereoSamples &outData) const
// ----------------------------------------------------------
{
	getSamplesFromChannel(outData.left, 0);
	getSamplesFromChannel(outData.right, 1);
}

#pragma mark - RMS

// ----------------------------------------------------------
float ofxAudioUnitTap::getRMS(unsigned int channel)
// ----------------------------------------------------------
{
	getSamplesFromChannel(_tempBuffer, channel);
	float rms;
	vDSP_rmsqv(&_tempBuffer[0], 1, &rms, _tempBuffer.size());
	return rms;
}

#pragma mark - Waveforms

// ----------------------------------------------------------
void WaveformForBuffer(const ofxAudioUnitTap::MonoSamples &buffer, float width, float height, ofPolyline &outLine, unsigned sampleRate)
// ----------------------------------------------------------
{	
	outLine.clear();
	
	const float xStep = width / (buffer.size() / sampleRate);
	float x = 0;
	
	for (int i = 0; i < buffer.size(); i += sampleRate, x += xStep)
	{
		float y = ofMap(buffer[i], -1, 1, height, 0, true);
		outLine.addVertex(ofPoint(x, y));
	}
}

void ofxAudioUnitTap::getWaveform(ofPolyline &l, float w, float h, unsigned chan, unsigned rate)
{
	getSamples(_tempBuffer, chan);
	WaveformForBuffer(_tempBuffer, w, h, l, rate);
}

void ofxAudioUnitTap::getLeftWaveform(ofPolyline &l, float w, float h, unsigned rate)
{
	getWaveform(l, w, h, 0, rate);
}

void ofxAudioUnitTap::getRightWaveform(ofPolyline &l, float w, float h, unsigned rate)
{
	getWaveform(l, w, h, 1, rate);
}

void ofxAudioUnitTap::getStereoWaveform(ofPolyline &l, ofPolyline &r, float w, float h, unsigned rate)
{
	getLeftWaveform(l, w, h, rate);
	getRightWaveform(r, w, h, rate);
}

ofPolyline ofxAudioUnitTap::getWaveform(float w, float h, unsigned chan, unsigned rate) {
	ofPolyline tempWave;
	getWaveform(tempWave, w, h, chan, rate);
	return tempWave;
}

ofPolyline ofxAudioUnitTap::getLeftWaveform(float w, float h, unsigned rate) {
	return getWaveform(w, h, 0, rate);
}

ofPolyline ofxAudioUnitTap::getRightWaveform(float w, float h, unsigned rate) {
	return getWaveform(w, h, 1, rate);
}

#endif // !TARGET_OS_IPHONE
