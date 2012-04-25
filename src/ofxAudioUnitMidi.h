#pragma once

#include <CoreMIDI/CoreMIDI.h>
#include <string>
#include <vector>

#include "ofxAudioUnit.h"

struct ofxAudioUnitMidi 
{
  static unsigned long getNumberOfSources(){return MIDIGetNumberOfSources();}
	static std::vector<std::string> getSourceNames();
	static void printSourceNames();
};

class ofxAudioUnitMidiReceiver 
{
  MIDIClientRef    _client;
	MIDIEndpointRef  _endpoint;
	MIDIPortRef      _port;
	AudioUnit        _unit;
	
public:
  ofxAudioUnitMidiReceiver(std::string clientName = "openFrameworks");
	ofxAudioUnitMidiReceiver(const ofxAudioUnitMidiReceiver &other);
	ofxAudioUnitMidiReceiver& operator= (const ofxAudioUnitMidiReceiver &other);
	~ofxAudioUnitMidiReceiver();
	
	bool createMidiDestination(std::string portName);
	bool connectToMidiSource(unsigned long midiSourceIndex);
	void disconnectFromMidiSource(unsigned long midiSourceIndex);
	void routeMidiTo(ofxAudioUnit &unitToRouteTo);
};

static void ofxAudioUnitMidiInputProc(const MIDINotification *message, 
																			void *refCon);

static void ofxAudioUnitMidiReadProc(const MIDIPacketList *pktlist, 
																		 void *readProcRefCon,
																		 void *srcConnRefCon);
