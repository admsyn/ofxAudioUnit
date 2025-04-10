#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
//	This example explains a bit about what an Audio Unit is, and
//	how to use one in the context of the ofxAudioUnit addon.
	
//	An Audio Unit is an external mini program you can use to 
//	experiment with sound. An Audio Unit usually performs a 
//	specific audio-related task. For example, there are 
//	synthesizer Audio Units for creating sound, effect Audio Units
//	for processing sound and so on. Typically, you connect
//	multiple Audio Units together to do interesting things.
	
//	If you've ever used an application like GarageBand, Ableton Live,
//	or Logic, you may have come across Audio Units in the form
//	of 3rd party synthesizers or effects. There are also many
//	Apple-supplied Audio Units that come pre-installed on
//	both OSX and iOS.
	
//	You access an Audio Unit by its description. An Audio Unit's 
//	description is a set of 3 OSTypes. These OSTypes correspond 
//	to the Type, Sub-Type and Manufacturer of an Audio Unit.
//	An OSType is a 4 letter code surrounded by ' characters

//	The Type is one of a handful of Apple-specified types such
//	as effects (reverb, delay, etc), music devices (synths) and
//	so on.
//	The Sub-Type is the specific Audio Unit you're looking for
//	The Manufacturer is who made the Audio Unit.
	
//	An example description would be 'aufx' 'mrev' 'appl', which
//	corresponds to the AUMatrixReverb Audio Unit
//	'aufx' is the Type and states that this is an effect
//	'mrev' is the Sub-Type and specifies the matrix reverb
//	'appl' is the Manufacturer and says that it's made by Apple
	
//	Here, we're creating an ofxAudioUnit which controls an
//	AUMatrixReverb Audio Unit
	
	reverb.setup('aufx', 'mrev', 'appl');
	
//	All of the Apple-supplied OSTypes also have convienient 
//	constants defined for them, which can make your code a
//	bit more readable. The reverb unit could also have been
//	set up like this:
//	
//	reverb.setup(kAudioUnitType_Effect,
//	             kAudioUnitSubType_MatrixReverb,
//	             kAudioUnitManufacturer_Apple);
//	
//	To see all of the Audio Units you have installed on your
//	computer, open a Terminal and run the command "auval -a"
//	This will list all of your units, as well as their descriptions
//	
//	ofxAudioUnit declares a few specific types that make it 
//	easier to work with certain Audio Units. These are:
//	
//	ofxAudioUnitFilePlayer        - plays or loops audio files
//	
//	ofxAudioUnitInput             - gets audio from your mic or line in
//	
//	ofxAudioUnitSampler           - plays files at various pitches
//	
//	ofxAudioUnitNetSend &         - these units work together to send audio over
//	ofxAudioUnitNetReceive          a network or between applications on the same computer
//	
//	ofxAudioUnitMixer             - mixes several audio streams into one
//	
//	ofxAudioUnitOutput            - sends audio to your speakers
//	
//	There is also the ofxAudioUnitTap, which sits between audio
//	units and allows you to read the audio samples that are
//	being sent from one to the other
//	
//	ofxAudioUnit.h contains more explanation about what each
//	of the specific types does
//	
//	We've already declared an ofxAudioUnitFilePlayer in the
//	ofApp.h of this example. Therefore we don't need to
//	specify which Audio Unit we're looking for since ofxAudioUnit
//	knows to look for the AUAudioFilePlayer unit.
//	
//	In order to do anything interesting with Audio Units, you
//	have to connect them together. Here, we're connecting 
//	our ofxAudioUnitFilePlayer to an ofxAudioUnitTap,
//	and then connecting the ofxAudioUnitTap to the
//	reverb we created earlier
	
	filePlayer.connectTo(tap);
	tap.connectTo(reverb);
	
//	Since this can get tedious, ofxAudioUnit lets you
//	chain calls to connectTo() on one line. Here, we're
//	connecting the reverb to an ofxAudioUnitMixer, and then
//	connecting the mixer to an ofxAudioUnitOutput
	
	reverb.connectTo(mixer).connectTo(output);
	
//	This syntax makes it a bit easier to visualize the connections.
//	The entire chain we've set up could have been done with this
//	one line:
//	
//	filePlayer.connectTo(tap).connectTo(reverb).connectTo(mixer).connectTo(output);
//	
//	One quirk regarding Audio Units is that they work on a "pull"
//	model. What this means is that the output requests audio
//	from the mixer, which causes the mixer to request audio
//	from the reverb, and so on, back to the filePlayer. While
//	this might seem backwards, it makes it easier to keep 
//	latency to a minimum. What this means for you is that 
//	nothing will happen until we tell the output to start
//	pulling audio.
	
	output.start();
	
//	As of right now, the output will pull audio down the chain,
//	but the filePlayer won't produce anything yet since we
//	haven't given it a file to play.
	
	filePlayer.setFile(ofToDataPath("entertainer.mp3"));
	filePlayer.play();
	
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){

//	The ofxAudioUnitTap has a function for getting an ofPolyline
//	representing the waveform of the samples currently passing 
//	through it

	tap.getLeftWaveform(waveform, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(170);
	ofSetColor(20);
	waveform.draw();
	
	ofSetColor(255, 230, 120);
	ofDrawBitmapString("<- Panning ->", ofGetWidth()/2 - 50, ofGetHeight() - 55);
	ofDrawCircle(ofGetMouseX(), ofGetHeight() - 30, 15);
	ofDrawBitmapString("Press space to show the reverb unit's UI,", 20, 20);
	ofDrawBitmapString("Press 'f' to make the file player's UI", 20, 40);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

//	The ofxAudioUnitMixer has functions for setting the pan, 
//	as well as its input and output volume
//	Pan is in the range -1 to 1 (left to right)

	mixer.setPan(ofMap(x, 0, ofGetWidth(), -1, 1, true));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
//	Audio Units have UIs which allow you to change their
//	parameters on the fly. Some Audio Units have very
//	elaborate custom UIs. The Matrix Reverb has a relatively
//	simple one with some sliders to change things like room
//	size, dry / wet mix and so on
	
	if(key == ' ') reverb.showUI();
	if(key == 'f') filePlayer.showUI();
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
