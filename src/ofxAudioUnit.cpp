#include "ofxAudioUnit.h"
#include <iostream>

using namespace std;

// ----------------------------------------------------------
ofxAudioUnit::ofxAudioUnit(AudioComponentDescription description) 
: _desc(description)
// ----------------------------------------------------------
{
  initUnit();
}

// ----------------------------------------------------------
ofxAudioUnit::ofxAudioUnit(OSType type, 
                           OSType subType, 
                           OSType manufacturer)
// ----------------------------------------------------------
{
  _desc.componentType         = type;
  _desc.componentSubType      = subType;
  _desc.componentManufacturer = manufacturer;
  _desc.componentFlags        = 0;
  _desc.componentFlagsMask    = 0;
  initUnit();
};

// ----------------------------------------------------------
ofxAudioUnit::ofxAudioUnit(const ofxAudioUnit &orig)
: _desc(orig._desc), _unit(orig._unit){}
// ----------------------------------------------------------

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnit::operator=(const ofxAudioUnit &orig)
// ----------------------------------------------------------
{
	if(this == &orig) return *this;
	
  _desc = orig._desc;
	_unit = orig._unit;
	
	return *this;
}

// ----------------------------------------------------------
void ofxAudioUnit::initUnit()
// ----------------------------------------------------------
{
  AudioComponent component = AudioComponentFindNext(NULL, &_desc);
  if(!component)
  {
    cout << "Couldn't locate component for description" << endl;
    return;
  }
  
	_unit = ofPtr<AudioUnit>((AudioUnit *)malloc(sizeof(AudioUnit)), AudioUnitDeleter);
  ERR_CHK(AudioComponentInstanceNew(component, _unit.get()), "creating new unit");
  ERR_CHK(AudioUnitInitialize(*_unit),                       "initializing unit");
}

// ----------------------------------------------------------
void AudioUnitDeleter(AudioUnit * unit)
// ----------------------------------------------------------
{
	ERR_CHK(AudioUnitUninitialize(*unit),         "uninitializing unit");
  ERR_CHK(AudioComponentInstanceDispose(*unit), "disposing unit");
}

#pragma mark - Parameters

// ----------------------------------------------------------
void ofxAudioUnit::setParameter(AudioUnitParameterID parameter, 
																AudioUnitScope scope, 
																AudioUnitParameterValue value,
																int bus)
// ----------------------------------------------------------
{
	ERR_CHK(AudioUnitSetParameter(*_unit, parameter, scope, bus, value, 0), "setting parameter");
}

#pragma mark - Connections

// ----------------------------------------------------------
void ofxAudioUnit::connectTo(ofxAudioUnit &otherUnit, int destinationBus, int sourceBus)
// ----------------------------------------------------------
{
  AudioUnitConnection connection;
  connection.sourceAudioUnit    = *_unit;
  connection.sourceOutputNumber = sourceBus;
  connection.destInputNumber    = destinationBus;
  
  ERR_CHK(AudioUnitSetProperty(*(otherUnit._unit), 
                               kAudioUnitProperty_MakeConnection,
                               kAudioUnitScope_Input,
                               destinationBus,
                               &connection,
                               sizeof(AudioUnitConnection)),
          "connecting units");
}

// ----------------------------------------------------------
void ofxAudioUnit::connectTo(ofxAudioUnitTap &tap)
// ----------------------------------------------------------
{
	tap._sourceUnit = _unit;
}

// ----------------------------------------------------------
ofxAudioUnit& ofxAudioUnit::operator>>(ofxAudioUnit& otherUnit)
// ----------------------------------------------------------
{
  connectTo(otherUnit);
  return otherUnit;
}

// ----------------------------------------------------------
ofxAudioUnitTap& ofxAudioUnit::operator>>(ofxAudioUnitTap &tap)
// ----------------------------------------------------------
{
	connectTo(tap);
	return tap;
}

#pragma mark - Busses

// ----------------------------------------------------------
bool ofxAudioUnit::setInputBusCount(unsigned int numberOfInputBusses)
// ----------------------------------------------------------
{
	UInt32 busCount = numberOfInputBusses;
	return ERR_CHK_BOOL(AudioUnitSetProperty(*_unit,
																					 kAudioUnitProperty_ElementCount,
																					 kAudioUnitScope_Input, 
																					 0, 
																					 &busCount, 
																					 sizeof(busCount)),
											"setting number of input busses");
}

// ----------------------------------------------------------
unsigned int ofxAudioUnit::getInputBusCount()
// ----------------------------------------------------------
{
	UInt32 busCount;
	UInt32 busCountSize = sizeof(busCount);
	ERR_CHK(AudioUnitGetProperty(*_unit,
															 kAudioUnitProperty_ElementCount,
															 kAudioUnitScope_Input,
															 0,
															 &busCount, 
															 &busCountSize), 
					"getting input bus count");
	return busCount;
}

// ----------------------------------------------------------
bool ofxAudioUnit::setOutputBusCount(unsigned int numberOfOutputBusses)
// ----------------------------------------------------------
{
	UInt32 busCount = numberOfOutputBusses;
	return ERR_CHK_BOOL(AudioUnitSetProperty(*_unit,
																					 kAudioUnitProperty_ElementCount,
																					 kAudioUnitScope_Output, 
																					 0, 
																					 &busCount, 
																					 sizeof(busCount)),
											"setting number of output busses");
}

// ----------------------------------------------------------
unsigned int ofxAudioUnit::getOutputBusCount()
// ----------------------------------------------------------
{
	UInt32 busCount;
	UInt32 busCountSize = sizeof(busCount);
	ERR_CHK(AudioUnitGetProperty(*_unit,
															 kAudioUnitProperty_ElementCount,
															 kAudioUnitScope_Output,
															 0,
															 &busCount, 
															 &busCountSize), 
					"getting output bus count");
	return busCount;
}

#pragma mark - Presets

// ----------------------------------------------------------
bool ofxAudioUnit::setPreset(std::string presetPath)
// ----------------------------------------------------------
{
  CFURLRef          presetURL;
  CFDataRef         presetData;
  CFPropertyListRef presetPList;
  Boolean           presetReadSuccess;
  SInt32            presetReadErrorCode;
  OSStatus          presetSetStatus;
  
  presetURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                      (const UInt8*)presetPath.c_str(),
                                                      presetPath.length(),
                                                      NULL);
  
  presetReadSuccess = CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault,
                                                               presetURL,
                                                               &presetData,
                                                               NULL,
                                                               NULL,
                                                               &presetReadErrorCode);
  CFRelease(presetURL);
  
  if(presetReadSuccess)
  {
    presetPList = CFPropertyListCreateWithData(kCFAllocatorDefault,
                                               presetData,
                                               kCFPropertyListImmutable,
                                               NULL,
                                               NULL);
    
    presetSetStatus = AudioUnitSetProperty(*_unit,
                                            kAudioUnitProperty_ClassInfo,
                                            kAudioUnitScope_Global,
                                            0,
                                            &presetPList,
                                            sizeof(presetPList));
    CFRelease(presetData);
    CFRelease(presetPList);
  }
  else
  {
		if(presetReadErrorCode == kCFURLUnknownError || 
			 presetReadErrorCode == kCFURLResourceNotFoundError)
		{
			cout << "Couldn't locate preset at " << presetPath << endl;
		}
		else 
		{
			cout << "CFURL Error Code: " << presetReadErrorCode 
			<< " while reading preset at " << presetPath << endl;
		}
  }

  return presetReadSuccess && (presetSetStatus == noErr);
}

#pragma mark - Render Callbacks

// ----------------------------------------------------------
void ofxAudioUnit::setRenderCallback(AURenderCallbackStruct callback, int bus)
// ----------------------------------------------------------
{
	ERR_CHK(AudioUnitSetProperty(*_unit, 
											 kAudioUnitProperty_SetRenderCallback,
											 kAudioUnitScope_Global,
											 bus, 
											 &callback, 
											 sizeof(callback)),
					"setting render callback");
}
