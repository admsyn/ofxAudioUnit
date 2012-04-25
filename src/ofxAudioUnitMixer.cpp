#include "ofxAudioUnit.h"

AudioComponentDescription mixerDesc = {
	kAudioUnitType_Mixer, 
	kAudioUnitSubType_MultiChannelMixer, 
	kAudioUnitManufacturer_Apple
};

// ----------------------------------------------------------
ofxAudioUnitMixer::ofxAudioUnitMixer()
// ----------------------------------------------------------
{
  _desc = mixerDesc;
  initUnit();
  
  // default volume is 0, which can make things seem like they aren't working
	
	int busses = getInputBusCount();
	for(int i = 0; i < busses; i++)
		setInputVolume(1, i);
  
	setOutputVolume(1);
}

#pragma mark - Volume / Pan

// ----------------------------------------------------------
void ofxAudioUnitMixer::setInputVolume(float volume, int bus)
// ----------------------------------------------------------
{
  ERR_CHK(AudioUnitSetParameter(*_unit, 
                                kMultiChannelMixerParam_Volume, 
                                kAudioUnitScope_Input, 
                                bus, 
                                volume,
                                0), 
          "setting mixer input gain");
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::setOutputVolume(float volume)
// ----------------------------------------------------------
{
  ERR_CHK(AudioUnitSetParameter(*_unit, 
                                kMultiChannelMixerParam_Volume, 
                                kAudioUnitScope_Output, 
                                0, 
                                volume,
                                0), 
          "setting mixer output gain");
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::setPan(float pan, int bus)
// ----------------------------------------------------------
{
  ERR_CHK(AudioUnitSetParameter(*_unit,
                                kMultiChannelMixerParam_Pan,
                                kAudioUnitScope_Input,
                                bus,
                                pan,
                                0),
          "setting mixer pan");
}

#pragma mark - Metering

// ----------------------------------------------------------
float ofxAudioUnitMixer::getInputLevel(int bus)
// ----------------------------------------------------------
{	
	AudioUnitParameterValue level;
	ERR_CHK(AudioUnitGetParameter(*_unit,
																kMultiChannelMixerParam_PreAveragePower,
																kAudioUnitScope_Input,
																bus,
																&level),
					"getting mixer input level");
	return level;
}

// ----------------------------------------------------------
float ofxAudioUnitMixer::getOutputLevel()
// ----------------------------------------------------------
{	
	AudioUnitParameterValue level;
	ERR_CHK(AudioUnitGetParameter(*_unit,
																kMultiChannelMixerParam_PreAveragePower,
																kAudioUnitScope_Output,
																0,
																&level),
					"getting mixer output level");
	return level;
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::enableInputMetering(int bus)
// ----------------------------------------------------------
{
	UInt32 on = 1;
	AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_MeteringMode,
											 kAudioUnitScope_Input,
											 bus,
											 &on,
											 sizeof(on));
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::enableOutputMetering()
// ----------------------------------------------------------
{
	UInt32 on = 1;
	AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_MeteringMode,
											 kAudioUnitScope_Output,
											 0,
											 &on,
											 sizeof(on));
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::disableInputMetering(int bus)
// ----------------------------------------------------------
{
	UInt32 off = 0;
	AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_MeteringMode,
											 kAudioUnitScope_Input,
											 bus,
											 &off,
											 sizeof(off));
}

// ----------------------------------------------------------
void ofxAudioUnitMixer::disableOutputMetering()
// ----------------------------------------------------------
{
	UInt32 off = 0;
	AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_MeteringMode,
											 kAudioUnitScope_Output,
											 0,
											 &off,
											 sizeof(off));
}
