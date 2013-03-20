#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <vector>
#include "ofTypes.h"

class ofxAudioUnit;

class ofxAudioUnitDSPNode
{
public:
	ofxAudioUnitDSPNode(unsigned int samplesToBuffer = 2048);
	virtual ~ofxAudioUnitDSPNode();
	
	ofxAudioUnit& connectTo(ofxAudioUnit &destination, int destinationBus = 0, int sourceBus = 0);
	OF_DEPRECATED_MSG("Use connectTo() instead.", ofxAudioUnit& operator>>(ofxAudioUnit& destination));
	
	void setSource(ofxAudioUnit * source);
	void setSource(AURenderCallbackStruct callback, UInt32 channels = 2);
	
protected:
	void getSamplesFromChannel(std::vector<AudioUnitSampleType> &samples, unsigned int channel) const;
	void setBufferSize(unsigned int samplesToBuffer);
	unsigned int getBufferSize() const;
	
private:
	struct NodeImpl;
	ofPtr<NodeImpl> _impl;
};
