#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

//	This example demonstrates what Audio Unit parameters
//	are and how to change them

//	Parameters are user-controllable variables on an Audio
//	Unit which you can change in real time. For example, a
//	Mixer audio unit typically has parameters for volume,
//	panning, etc. These are the parameters that you are
//	changing with ofxAudioUnitMixer's setPan() and
//	setInputVolume() functions.

//	However, there are a huge number of parameters that Audio
//	Units make available, and ofxAudioUnit can't make convienient
//	functions for all of them. This example shows you how to
//	programmatically set Audio Unit parameters on the fly.

//	These are the audio units we'll use:

//	AUVarispeed - let's you change the playback speed of audio
//	going through it, while simultaneously changing the pitch
//	(like slowing down and speeding up a record)

	varispeed.setup(kAudioUnitType_FormatConverter, kAudioUnitSubType_Varispeed);

//	AULowPass - a lowpass filter which cuts off high frequencies
//	at a user-controllable cutoff point

	lowpass.setup(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);

//	We'll also use an ofxAudioUnitFilePlayer to generate audio,
//	an ofxAudioUnitTap to extract the waveform, and an output
//	to send the resulting audio to your speakers

	filePlayer.connectTo(varispeed).connectTo(lowpass).connectTo(tap).connectTo(output);

//	First, we'll set the lowpass's resonance setting. All of the
//	Apple-manufactured Audio Units have parameter constants defined
//	which follow a particular naming pattern. The two lowpass
//	parameters are kLowPassParam_Resonance and
//	kLowPassParam_CutoffFrequency.

//	The easiest way to find the parameters for your Audio Unit
//	are to type kNameOfYourUnit and let Xcode's auto-complete
//	show you what's available. You can also see all of the parameters
//	defined in <AudioUnit/AudioUnitParameters.h>. It is a good idea
//	to read this header file anyway, as it'll tell you what values
//	these parameters are expecting. One way to get information
//	on the parameter you want to change is to type it, then Cmd-click
//	on it to take you to its entry in AudioUnitParameters.h

//	You set parameters by using the function AudioUnitSetParameter().
//	This function expects a few arguments :

//	inUnit - An AudioUnit struct. You can get this from an ofxAudioUnit
//	by calling getUnit() on it
//	inID - The parameter you want to change
//	inScope - The scope the parameter applies to. This is usually
//	the Global scope. Try the Input or Ouput scope if Global doesn't
//	work.
//	inElement - The bus you're changing the parameter on. For Global
//	params, it's always 0
//	inValue - The value you're setting the parameter to.
//	inBufferOffsetFrames - If you want to set the parameter in the
//	future, give this an offset in terms of samples (eg. 44100 for
//	1 second). Usually you want this to be 0.

//	Here, we're setting the lowpass's resonance to 10

	AudioUnitSetParameter(lowpass, kLowPassParam_Resonance, kAudioUnitScope_Global, 0, 10, 0);

//	If you're using a 3rd party unit (i.e. one that's not made by Apple)
//	you'll need to retrieve and set parameters by finding their parameter
//	ID (which is effectively just an index). This call will print a
//	unit's parameter list to the console.

	lowpass.printParameterList();

//	You can also save the state of an Audio Unit's parameters as a
//	preset file. Saving / Loading a preset file is done like this:

//	varispeed.saveCustomPreset("MyPreset");
//	varispeed.loadCustomPreset("MyPreset");

//	These functions will look for or create a preset file with the extension
//	".aupreset" in your app's data folder.

//	You can also create .aupreset files in Garageband and other DAWs.
//	Usually, these are stored in ~/Library/Audio/Presets/

	output.start();

	filePlayer.setFile(ofFilePath::getAbsolutePath("chopin.mp3"));
	filePlayer.loop();

	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	tap.getLeftWaveform(waveform, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(40);
	ofSetColor(255);
	waveform.draw();
	ofDrawBitmapString("Press 'f' to show the file player", glm::vec2(40,20));

	ofSetColor(255, 125, 50);
	ofDrawCircle(20, ofGetMouseY(), 15);
	ofDrawBitmapString("|\nFilter\nFrequency\n|", glm::vec2(40, ofGetHeight()/2));

	ofSetColor(20, 255, 150);
	ofDrawCircle(ofGetMouseX(), ofGetHeight() - 20, 15);
	ofDrawBitmapString("<- Playback Speed ->", glm::vec2(ofGetWidth()/2 - 100, ofGetHeight() - 40));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'f') filePlayer.showUI();
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

//	The varispeed has an adjustable playback rate. Setting
//	it to 1 means a normal playback speed. Anything higher
//	or lower speeds it up or slows it down accordingly.

	float newSpeed = ofMap(x, 0, ofGetWidth(), 0.01, 2, true);

	AudioUnitSetParameter(varispeed,
						  kVarispeedParam_PlaybackRate,
						  kAudioUnitScope_Global,
						  0,
						  newSpeed,
						  0);

	float newCutoff = ofMap(y, 0, ofGetHeight(), 10, 6900);

	AudioUnitSetParameter(lowpass,
						  kLowPassParam_CutoffFrequency,
						  kAudioUnitScope_Global,
						  0,
						  newCutoff,
						  0);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
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
