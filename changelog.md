0.4
=====

- Created an abstract class ofxAudioUnitDSPNode, for general purpose DSP
  - ofxAudioUnitTap is now a subclass of this, so the "render and copy" behaviour is reusable

- ofxAudioUnitTap can now re-adjust its buffer length on the fly w/setBufferLength()

0.3
=====

New Features
------------
- Tap and Input now rely on TPCircularBuffer for buffering audio (circularly)
- Tap and Input now support an arbitrary buffer length (buffer length passed in constructor)

Changes & Deprecations
----------------------
- ofxAudioUnit::getUnit() now returns an AudioUnit instead of AudioUnitRef
  - use getUnitRef() to get the internal shared pointer

- ofxAudioUnitTapSamples is now ofxAudioUnitTap::StereoSamples
  - ofxAudioUnitTap::MonoSamples also added
  - getSamples methods overloaded to take either mono or stereo sample buffer type

- connectTo() now returns reference of connected-to unit (for chaining)
- operator >> (alias for connectTo()) now deprecated 

- Switched to "pimpl"-style for implementations of tap and input
- ofxAudioUnit constructors are now explicit

