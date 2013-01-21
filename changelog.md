0.2.3
=====

- ofxAudioUnit::getUnit() now returns an AudioUnit instead of AudioUnitRef
  - use getUnitRef() to get the internal shared pointer

- constructors are now explicit

- connectTo() now returns reference of connected-to unit (for chaining)

- Tap now relies on TPCircularBuffer for buffering audio (circularly)
- Tap now supports an arbitrary buffer length (buffer length passed in
constructor)
- ofxAudioUnitTapSamples is now ofxAudioUnitTap::StereoSamples
  - ofxAudioUnitTap::MonoSamples also added
