#include "ofxAudioUnit.h"

AudioComponentDescription outputDesc = {
	kAudioUnitType_Output,
	kAudioUnitSubType_HALOutput,
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitOutput::ofxAudioUnitOutput()
// ----------------------------------------------------------
{
  _desc = outputDesc;
  initUnit();
}

// ----------------------------------------------------------
void ofxAudioUnitOutput::start()
// ----------------------------------------------------------
{
  ERR_CHK(AudioOutputUnitStart(*_unit), "starting output unit");
}

// ----------------------------------------------------------
void ofxAudioUnitOutput::stop()
// ----------------------------------------------------------
{
  ERR_CHK(AudioOutputUnitStop(*_unit), "stopping output unit");
}
