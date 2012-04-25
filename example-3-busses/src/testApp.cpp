#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{	
	
//	This example demonstrates the concept of "busses" as they
//	relate to audio and Audio Units in particular
	
//	A "bus" is basically a pathway for an audio stream to flow
//	through. This is NOT the same as a channel. For example,
//	you can have audio with 2 channels (ie. stereo) flowing
//	through 1 bus. You could also have 5.1 channel audio flowing
//	through a single bus.
	
//	Your output unit expects a single stereo bus of audio to send to
//	the speakers. If you want to have more than a single instrument
//	creating sound, you'll need to mix several busses down into
//	one. This is what the ofxAudioUnitMixer is for.
	
//	The ofxAudioUnitMixer has a variable amount of input busses
//	and one output bus. You can set the relative volume of each
//	bus in order to get a good mix between your sound sources.
	
//	In this example, we'll set up 3 file players playing different
//	loops in sync with each other. We'll use a mixer to mix these
//	audio sources down to a single bus for the output
	
//	First, let's set up our sources
	
	source1.setFile(ofFilePath::getAbsolutePath("kick.wav"));
	source2.setFile(ofFilePath::getAbsolutePath("snare.wav"));
	source3.setFile(ofFilePath::getAbsolutePath("hats.wav"));
	
//	Now, let's set up a different effect for each one
	
	distortion = ofxAudioUnit(kAudioUnitType_Effect,
														kAudioUnitSubType_Distortion);
	
	delay = ofxAudioUnit(kAudioUnitType_Effect,
											 kAudioUnitSubType_Delay);
	
	filter = ofxAudioUnit(kAudioUnitType_Effect,
												kAudioUnitSubType_LowPassFilter);
	
//	We'll send each of our sources through its own effect, and also
//	through its own tap so that we can see the individual waveforms
//	later
	
	source1 >> distortion >> tap1;
	source2 >> delay      >> tap2;
	source3 >> filter     >> tap3;
	
//	Now, we'll connect each of these sources to a different
//	input bus on the mixer. Since we need to specify which bus
//	we want to connect to, we can't use the ">>" syntax.
	
	mixer.setInputBusCount(3);
	tap1.connectTo(mixer, 0);
	tap2.connectTo(mixer, 1);
	tap3.connectTo(mixer, 2);
	
//	Now, we'll send the mixer's single output bus through a shared
//	compressor effect, then to the output
	
	compressor = ofxAudioUnit(kAudioUnitType_Effect,
														kAudioUnitSubType_DynamicsProcessor);
	
	mixer >> compressor >> output;
	
//	You can set the individual volume of each input bus on the mixer
//	Volume is in the range of 0 (muted) to 1 (unchanged)
//	You can set the volume higher than 1, but it may result in clipping

//	We'll set the volume of bus 2 (the hi hat loop) to 0.5, since
//	they're kind of loud
	
	mixer.setInputVolume(0.5, 2);
	
//	Now, start the output. The output will cause the mixer to pull
//	from each of the sources connected to it
	
	output.start();
	
//	Start each of the loops at the same time so that they're in sync
	
	source1.loop();
	source2.loop();
	source3.loop();
	
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void testApp::update(){
	tap1.getLeftWaveform(wave1, ofGetWidth(), ofGetHeight()/3);
	tap2.getLeftWaveform(wave2, ofGetWidth(), ofGetHeight()/3);
	tap3.getLeftWaveform(wave3, ofGetWidth(), ofGetHeight()/3);
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	
	ofPushMatrix();
	{
		ofSetColor(255, 255, 0);
		wave1.draw();
		ofSetColor(255);
		ofDrawBitmapString("Press 'd' for distortion UI", 20, 20);
		
		ofTranslate(0, ofGetHeight()/3);
		
		ofSetColor(0, 255, 255);
		wave2.draw();
		ofSetColor(255);
		ofDrawBitmapString("Press 'e' for delay UI", 20, 20);
		
		ofTranslate(0, ofGetHeight()/3);
		
		ofSetColor(255, 0, 255);
		wave3.draw();
		ofSetColor(255);
		ofDrawBitmapString("Press 'f' for filter UI", 20, 20);
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'e') delay.showUI();
	if(key == 'd') distortion.showUI();
	if(key == 'f') filter.showUI();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}