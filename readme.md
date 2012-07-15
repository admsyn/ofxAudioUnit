Intro
-----

ofxAudioUnit is an addon for [openFrameworks](http://www.openframeworks.cc/) that lets you work with [Audio Units](http://en.wikipedia.org/wiki/Audio_Units) in an intuitive way.

What Are Audio Units?
---------------------

Audio Units are small programs that you can hook up to each other to produce sound. For example, you can hook up a synthesizer unit to a distortion effect unit, then connect the distortion unit to an output unit to send the audio to your speakers. This is analogous to a guitar being connected to an effect stompbox which is connected to an amplifier.

Audio Units are only available on osx and iOS. If you're on a mac, you already have a bunch of Audio Units on your computer. Run this command in a Terminal to see them all: 

```
auval -a
```

Getting Started
---------------

Just add ofxAudioUnit to your openFrameworks/addons/ folder.

I highly recommend that you check out the examples, as they demonstrate the important concepts related to Audio Units, as well as clear up some of the terminology. The examples are numbered in order to provide a logical progression.

Adding ofxAudioUnit to your oF app
---------------------------------

* Add the ofxAudioUnit src folder to your Xcode project
* Add the AudioUnit and CoreAudioKit frameworks to your project
* If you're using ofxAudioUnitMidi, add the CoreMidi framework to your project as well

[See here if you need help adding frameworks to your project](http://stackoverflow.com/questions/3352664/how-to-add-existing-frameworks-in-xcode-4)

After this, just add
```C++
#include "ofxAudioUnit.h"
```
in your testApp.h file. If you're using the ofxAudioUnitMidiReceiver, `#include ofxAudioUnitMidi.h` as well.

Soon
---------------
* Proper iOS support
* Direct, port-less MIDI (for sending MIDI messages directly to the units)
* Saving .aupreset files

License
----------

MIT License. See license.txt in this distrubtion.
