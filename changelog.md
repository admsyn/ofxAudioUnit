0.3
=====

- ofxAudioUnit::getUnit() now returns an AudioUnit instead of AudioUnitRef
  - use getUnitRef() to get the internal shared pointer

- connectTo() now returns reference of connected-to unit (for chaining)

- Tap and Input now rely on TPCircularBuffer for buffering audio (circularly)
- Tap and Input now support an arbitrary buffer length (buffer length passed in constructor)
- Switched to "pimpl"-style for implementations of tap and input

- ofxAudioUnitTapSamples is now ofxAudioUnitTap::StereoSamples
  - ofxAudioUnitTap::MonoSamples also added
  - getSamples methods overloaded to take either mono or stereo sample buffer type

- ofxAudioUnit constructors are now explicit

