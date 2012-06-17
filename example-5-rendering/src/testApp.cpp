#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
//	This example will show you how to generate your own sound
//	programmatically. To do this, you'll be using render callbacks.
	
//	A render callback works like this : when the output unit begins
//	to pull audio through an audio unit chain, each unit pulls from
//	the audio source before it. That source can be either another
//	unit or a render callback.

//	A render callback is a function in your program that provides
//	samples to an Audio Unit. You can create samples however you
//	like. For instance, you can use sin() to create sine waves, or
//	write uncompressed samples you have retrieved from an audio file.
//	You could even do something weird like using video or image data 
//	as your samples. 
	
//	Note that if you're being experimental, it's worth it to keep your
//	samples in a sensible range (ie -1 to 1). Audio Units will let
//	you seriously overload your computer's audio hardware, which could
//	mess up your speakers if you have your volume set high
	
//	There are a few important catches, however. Render callbacks
//	work on what's called a realtime thread. This means that your
//	render callback must finish fast. Very fast. You won't have time
//	to read data off the hard drive or do any heavy processing.
//	If you want to do anything complicated, you should have a buffer
//	ready. If your render callback takes too long, you will hear silence
//	or glitches in the audio.
	
//	Another catch is that the render callback must be a static function,
//	not a member of your testApp. What this means is that you'll have to
//	pass a reference to your app (eg. "this") if you want to access its variables.
//	Your callback gets called by an Audio Unit (you don't call it yourself).
	
//	One more catch is that (since the callback is being called by the
//	Audio Unit on a realtime thread) you'll have to deal with things like
//	mutexes if you want to share variables between the callback and the rest
//	of your app. See ofxAudioUnitTap.cpp for an example of this.

//	In this example, we'll just be rendering simple waveforms. If you want
//	to do more complicated things, search for more info on AURenderCallback.
//	I recommend "Learning Core Audio" by Chris Adamson if you want a
//	book on the subject (and all manner of other Core Audio / Audio Unit
//	things).
	
//	First, let's set up an Audio Unit chain
	
	distortion = ofxAudioUnit(kAudioUnitType_Effect,
							  kAudioUnitSubType_Distortion);
	
	reverb = ofxAudioUnit(kAudioUnitType_Effect,
						  kAudioUnitSubType_MatrixReverb);
	
	distortion >> reverb >> tap >> output;
	
//	Now, we'll tell the distortion unit to get its source samples from
//	our render callback. You need to give the Audio Unit two bits of
//	information: the name of your callback (inputProc) and a pointer
//	to any information you'll want to access inside the callback
//	(inputProcRefCon). I'm passing "this" to the callback for demonstration
//	purposes (we're not actually going to use it).
	
	AURenderCallbackStruct callbackInfo;
	callbackInfo.inputProc = renderPulseSineChord;
	callbackInfo.inputProcRefCon = this;

	distortion.setRenderCallback(callbackInfo);
	
//	Once we tell the output unit to start pulling audio, our callback
//	function will start getting called. Typically, this will be for
//	batches of 512 samples. At a sample rate of 44,100 Hz, this means
//	that our callback will be called about 86 times a second.
	
	output.start();
	
	ofSetVerticalSync(true);
}

//	This is our render callback. Every render callback has the same arguments.
//	The important ones are:
//	inRefCon - the pointer we gave the Audio Unit earlier (inputProcRefCon)
//	inNumberFrames - the number of samples that the Audio Unit wants from us
//	ioData - where we're supposed to write the samples we create

//	This particular callback will render a chord of sine waves (3 sine waves
//	with different wavelengths). It will also pulse the chord's volume via
//	another sine wave.

//	See the commented-out render callback below this one for a bare-bones 
//	callback you can copy to get yourself started

OSStatus renderPulseSineChord(void * inRefCon,
							  AudioUnitRenderActionFlags * ioActionFlags,
							  const AudioTimeStamp * inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32 inNumberFrames,
							  AudioBufferList * ioData)
{
	static double phase = 0; // used to generate the sine waves
	static double pulse = 0; // used to pulse the volume of the sine wave
	
//	Here, we're grabbing pointers to the two chunks of memory that the Audio Unit
//	wants the samples written to (left channel and right channel).
	AudioUnitSampleType * leftSamples  = (AudioUnitSampleType *)ioData->mBuffers[0].mData;
	AudioUnitSampleType * rightSamples = (AudioUnitSampleType *)ioData->mBuffers[1].mData;
	
//	Now, we iterate over the buffer and write our sine wave samples into the 
//	left channel's buffer
	for(int i = 0; i < inNumberFrames; i++)
	{
		// generating the waves (and multiplying each by 0.33 so they don't clip)
		leftSamples[i]  = sin(phase)        * 0.33;
		leftSamples[i] += sin(phase * 0.5)  * 0.33;
		leftSamples[i] += sin(phase * 0.75) * 0.33;
		
		// pulsing the volume
		leftSamples[i] *= sin(pulse);
		
		phase += 0.05;
		pulse += 0.00005;
	}
	
//	Since we're not doing any stereo effects, we'll just copy the left channel's 
//	buffer into the right channel.
	memcpy(rightSamples, leftSamples, inNumberFrames * sizeof(AudioUnitSampleType));
	
//	We're done. We return noErr so the Audio Unit knows we finished properly
	return noErr;
}


/*

 This is a bare-bones callback you can copy to use in your own app.
 
OSStatus plainRenderCallback(void * inRefCon,
							 AudioUnitRenderActionFlags * ioActionFlags,
							 const AudioTimeStamp * inTimeStamp,
							 UInt32 inBusNumber,
							 UInt32 inNumberFrames,
							 AudioBufferList * ioData)
{	
	//  Here, we're grabbing a pointer to the two chunks of memory that the Audio Unit
	//	wants the samples written to (left channel and right channel).
	//	Note : you really should check ioData->mNumberBuffers to see if your destination
	//  Audio Unit actually wants stereo audio (mNumberBuffers == 2). We're just assuming.
	AudioUnitSampleType * leftSamples  = (AudioUnitSampleType *)ioData->mBuffers[0].mData;
	AudioUnitSampleType * rightSamples = (AudioUnitSampleType *)ioData->mBuffers[1].mData;
	
	//	Now, we iterate over the buffer and write our samples into the left channel's buffer
	for(int i = 0; i < inNumberFrames; i++)
	{
		// writing 0s (silence)
		leftSamples[i] = 0;
	}
	
	// copying the left channel into the right channel
	memcpy(rightSamples, leftSamples, inNumberFrames * sizeof(AudioUnitSampleType));
	
	//	Return noErr so the Audio Unit knows we finished properly
	return noErr;
}
*/

//--------------------------------------------------------------
void testApp::update(){
	tap.getLeftWaveform(waveform, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(30);
	ofSetColor(255, 100, 100);
	ofSetLineWidth(3);
	waveform.draw();
	
	ofSetColor(255);
	ofDrawBitmapString("Press 'd' to show distortion UI", ofPoint(20,20));
	ofDrawBitmapString("Press 'r' to show reverb UI", ofPoint(20,40));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'd') distortion.showUI();
	if(key == 'r') reverb.showUI();
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