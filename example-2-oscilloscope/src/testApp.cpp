#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	activeUnit = NULL;
	
//	This example demonstrates a few different ways of producing
//	audio with ofxAudioUnit

//	Un-comment one of these blocks depending on where you want
//	your audio to come from
	
//	This will open a file with an ofxAudioUnitFilePlayer and play it.
//-------------------------------------------------
//	filePlayer.setFile(ofFilePath::getAbsolutePath("entertainer.mp3"));
//	filePlayer.play();
//	filePlayer >> tap >> output;
//	activeUnit = &filePlayer;
//-------------------------------------------------

//	This will use an ofxAudioUnitSpeechSynth to generate
//	audio via text-to-speech.
//	Press 's' to make the speech synth talk.
//	See the console output to see what other voices are
//	available on your computer.
//-------------------------------------------------
//	speechSynth.printAvailableVoices();
//	speechSynth.setVoice("Fred");
//	speechSynth >> tap >> output;
//	activeUnit = &speechSynth; 
//-------------------------------------------------
	
//	This will use an ofxAudioUnitNetReceive to get audio
//	from a corresponding AUNetSend running in another application
//	on your computer (GarageBand, Ableton, Logic, etc)
//-------------------------------------------------
//	netReceive.connectToHost("127.0.0.1");
//	netReceive >> tap >> output;
//	activeUnit = &netReceive;
//-------------------------------------------------
	
//	This will create a MIDI destination that other
//	applications can send MIDI to. The MIDI destination
//	will show up as "openFrameworks". Any received MIDI
//	will be routed to a synth unit.
//	Note that if you're using Ableton to generate MIDI, you'll
//	have to go into the MIDI / Sync preferences and turn
//	"Track" on for the openFrameworks MIDI Destination.
//	If you're on osx 10.6 or earlier, you will have to
//	change kAudioUnitSubType_Sampler to 
//	kAudioUnitSubType_DLSSynth
//-------------------------------------------------
//	sampler = ofxAudioUnit(kAudioUnitType_MusicDevice,
//												 kAudioUnitSubType_Sampler);
//
//	midiReceiver.createMidiDestination("openFrameworks");
//	midiReceiver.routeMidiTo(sampler);
//	sampler >> tap >> output;
//	activeUnit = &sampler;
//-------------------------------------------------
	
//	This will connect a MIDI receiver to an existing MIDI
//	source (for instance, a MIDI keyboard you have plugged
//	into your computer). This MIDI will be routed to
//	a synth unit. 
//	This will also print the source names and indexes
//	of your available MIDI sources. You can change the
//	arg for connectToMidiSource() to one of these indexes
//	if you want to use a different source.
//	If you're on osx 10.6 or earlier, you will have to
//	change kAudioUnitSubType_Sampler to 
//	kAudioUnitSubType_DLSSynth
//-------------------------------------------------
//	sampler = ofxAudioUnit(kAudioUnitType_MusicDevice,
//												 kAudioUnitSubType_Sampler);
//	
//	ofxAudioUnitMidi::printSourceNames();
//	midiReceiver.connectToMidiSource(0);
//	midiReceiver.routeMidiTo(sampler);
//	sampler >> tap >> output;
//	activeUnit = &sampler;
//-------------------------------------------------
	
	output.start();
	ofSetVerticalSync(true);
	ofEnableSmoothing();
}

//--------------------------------------------------------------
void testApp::update(){
	tap.getStereoWaveform(oscLineLeft, oscLineRight, ofGetWidth(), ofGetHeight()/4);
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0,50,10);
	drawOscilloscopeGrid();
	drawOscilloscopeLines();
	drawOscilloscopeLabels();
	
	ofSetColor(255);
	if(activeUnit){
		ofDrawBitmapString("Press space to show the Audio Unit's UI", ofPoint(25,25));
	}
	else {
		ofDrawBitmapString("You need to un-comment one of the blocks in\
						   setup() to generate some audio", ofPoint(25,25));
	}
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ' && activeUnit)
	{
		activeUnit->showUI();
	}
	else if(key == 's')
	{
		speechSynth.say("Fitter. Happier. More Productive");
	}
}

//--------------------------------------------------------------
void testApp::drawOscilloscopeGrid()
{
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
void testApp::drawOscilloscopeLabels()
{
	ofSetColor(0, 200, 40);
	ofDrawBitmapString("Left Channel", ofPoint(ofGetWidth() / 2 - 60, ofGetHeight() / 8 - 20));
	ofDrawBitmapString("Right Channel", ofPoint(ofGetWidth() / 2 - 70, 
												ofGetHeight() - (ofGetHeight() / 8 - 20)));
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
void testApp::drawOscilloscopeLines()
{
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
