#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"

class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxAudioUnitOutput output;
	ofxAudioUnit reverb;
	ofxAudioUnit distortion;
	ofxAudioUnitTap tap;
	ofPolyline waveform;
};

static OSStatus renderPulseSineChord(void * inRefCon,
																		 AudioUnitRenderActionFlags * ioActionFlags,
																		 const AudioTimeStamp * inTimeStamp,
																		 UInt32 inBusNumber,
																		 UInt32 inNumberFrames,
																		 AudioBufferList * ioData);
