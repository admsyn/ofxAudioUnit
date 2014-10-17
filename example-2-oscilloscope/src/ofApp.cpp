#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	activeUnit = NULL;
	
//	This example demonstrates a few different ways of producing
//	audio with ofxAudioUnit

//	Un-comment one of these blocks depending on where you want
//	your audio to come from
	
//	This will open a file with an ofxAudioUnitFilePlayer and play it.
//-------------------------------------------------
//	filePlayer.setFile(ofToDataPath("../../../example-1-hello/bin/data/entertainer.mp3"));
//	filePlayer.play();
//	filePlayer.connectTo(tap).connectTo(output);
//	activeUnit = &filePlayer;
//-------------------------------------------------
	
//	This will use an ofxAudioUnitInput to get audio from
//	your selected audio input source (probably your mic
//	or line in). You can change the source device by calling
//	setDevice() on the input with either a device ID or just
//	the name of the device.
//	WARNING: You will get feedback if you attempt to
//	use your microphone for input and your speakers for
//	output. This will be unpleasant.
//	If you get error -10863, make sure your input sample
//	rate is set to 44100 and 32-bit float in Audio MIDI Setup.app
//-------------------------------------------------
//	input.listInputDevices();
//	input.connectTo(tap).connectTo(output);
//	input.start();
//	activeUnit = &input;
//-------------------------------------------------
	
//	This will use an ofxAudioUnitNetReceive to get audio
//	from a corresponding AUNetSend running in another application
//	on your computer (GarageBand, Ableton, Logic, etc)
//-------------------------------------------------
//	netReceive.connectToHost("127.0.0.1");
//	netReceive.connectTo(tap).connectTo(output);
//	activeUnit = &netReceive;
//-------------------------------------------------
	
//	This will create a MIDI destination that other
//	applications can send MIDI to. The MIDI destination
//	will show up as "openFrameworks". Any received MIDI
//	will be routed to an ofxAudioUnitSampler.
//	Note that if you're using Ableton to generate MIDI, you'll
//	have to go into the MIDI / Sync preferences and turn
//	"Track" on for the openFrameworks MIDI Destination.
//-------------------------------------------------	
//	midiReceiver.createMidiDestination("openFrameworks");
//	midiReceiver.routeMidiTo(sampler);
//	sampler.connectTo(tap).connectTo(output);
//	activeUnit = &sampler;
//-------------------------------------------------
	
//	This will connect a MIDI receiver to an existing MIDI
//	source (for instance, a MIDI keyboard you have plugged
//	into your computer). MIDI from that source will be
//	routed to an ofxAudioUnitSampler.
//	This will also print the source names and indexes
//	of your available MIDI sources. You can change the
//	arg for connectToMidiSource() to one of these indexes
//	if you want to use a different source.
//-------------------------------------------------
//	ofxAudioUnitMidi::printSourceNames();
//	midiReceiver.connectToMidiSource(0);
//	midiReceiver.routeMidiTo(sampler);
//	sampler.connectTo(tap).connectTo(output);
//	activeUnit = &sampler;
//-------------------------------------------------
	
	output.start();
	ofSetVerticalSync(true);
	ofEnableSmoothing();
}

//--------------------------------------------------------------
void ofApp::update(){
	tap.getStereoWaveform(oscLineLeft, oscLineRight, ofGetWidth(), ofGetHeight()/4);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0,50,10);
	drawOscilloscopeGrid();
	drawOscilloscopeLines();
	drawOscilloscopeLabels();
	
	ofSetColor(255);
	if(activeUnit){
		ofDrawBitmapString("Press space to show the Audio Unit's UI", ofPoint(25,25));
	} else {
		ofDrawBitmapString("You need to un-comment one of the blocks in setup() to generate some audio", ofPoint(25,25));
	}
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == ' ' && activeUnit) {
		activeUnit->showUI();
	}
}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeGrid() {
	ofSetColor(0, 40, 0);
	ofSetLineWidth(1);
	float step = float(ofGetHeight()) / 64;
	for(int x = 0; x < ofGetWidth(); x += step)
	{
		ofLine(x, 0, x, ofGetHeight());
	}
	for(int y = 0; y < ofGetHeight(); y += step)
	{
		ofLine(0, y, ofGetWidth(), y);
	}
}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeLabels() {
	ofSetColor(0, 200, 40);
	ofDrawBitmapString("Left Channel", 25, ofGetHeight() / 8 - 40);
	ofDrawBitmapString("Right Channel", 25, ofGetHeight() - (ofGetHeight() / 8 - 40));
	for(int y = 0; y < ofGetHeight(); y += ofGetHeight() / 2)
	{
		ofPushMatrix();
		{
			ofTranslate(0, y);
			
			ofTranslate(0, ofGetHeight() / 8);
			ofDrawBitmapString("+1", 0, 0);
			ofDrawBitmapString("+1", ofGetWidth() - 20, 0);
			
			ofTranslate(0, ofGetHeight() / 8);
			ofDrawBitmapString("0", 3, 0);
			ofDrawBitmapString("0", ofGetWidth() - 15, 0);
			
			ofTranslate(0, ofGetHeight() / 8);
			ofDrawBitmapString("-1", 0, 0);
			ofDrawBitmapString("-1", ofGetWidth() - 20, 0);
		}
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::drawOscilloscopeLines() {
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(60, 150, 80, 175);
	for(int i = 1; i < 7; i++)
	{
		ofSetLineWidth(i);
		ofPushMatrix();
		{
			ofTranslate(0, ofGetHeight()/8 - 5);
			oscLineLeft.draw();
			ofTranslate(0, ofGetHeight()/2);
			oscLineRight.draw();
		}
		ofPopMatrix();
	}
	ofDisableBlendMode();
	
	ofSetColor(255);
	ofSetLineWidth(2);
	ofPushMatrix();
	{
		ofTranslate(0, ofGetHeight()/8 - 5);
		oscLineLeft.draw();
		ofTranslate(0, ofGetHeight()/2);
		oscLineRight.draw();
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
