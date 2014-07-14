Intro
-----

ofxAudioUnit is an addon for [openFrameworks](http://www.openframeworks.cc/) that lets you work with [Audio Units](http://en.wikipedia.org/wiki/Audio_Units) in an intuitive way.

[Here's a demo of the examples](http://vimeo.com/41115496)

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

If you have [OFPlugin](https://github.com/admsyn/OFPlugin), it will sort out the frameworks dependencies for you. If you'd like to set ofxAudioUnit up manually:

* Add the ofxAudioUnit src folder to your Xcode project
* Add the AudioUnit and CoreAudioKit frameworks to your project
* If you're using ofxAudioUnitMidi, add the CoreMidi framework to your project as well

[See here if you need help adding frameworks to your project](http://stackoverflow.com/questions/3352664/how-to-add-existing-frameworks-in-xcode-4)

After this, just add
```C++
#include "ofxAudioUnit.h"
```
in your ofApp.h file. If you're using the ofxAudioUnitMidiReceiver, `#include ofxAudioUnitMidi.h` as well.

iOS
---
ofxAudioUnit has rudimentary iOS support, but doesn't have 1:1 coverage. The DSP stuff (FFT, tap, recorder..) won't work at the moment, because they haven't been ported to adapt to the different sample format on iOS. You'll also have to manage your own audio session. Also, be aware that the Apple-supplied audio units aren't the same between iOS and OSX.

I would gladly accept a PR that adds more support for iOS.

License
----------

MIT License. See license.txt in this distrubtion.
