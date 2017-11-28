#include "ofxAudioUnitRecorder.h"
#include "ofxAudioUnitUtils.h"

// a render callback records audio passing through it
static OSStatus Record(void * inRefCon,
					   AudioUnitRenderActionFlags *	ioActionFlags,
					   const AudioTimeStamp *	inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32	inNumberFrames,
					   AudioBufferList * ioData);


ofxAudioUnitRecorder::ofxAudioUnitRecorder()
: _recordFile(NULL) {
	
}

ofxAudioUnitRecorder::~ofxAudioUnitRecorder() {
	if(_recordFile) {
		ExtAudioFileDispose(_recordFile);
		_recordFile = NULL;
	}
}

bool ofxAudioUnitRecorder::startRecording(const std::string &filePath) {
	
	if(_recordFile) {
		stopRecording();
	}
	
	AudioStreamBasicDescription inASBD = getSourceASBD();
	
	if(inASBD.mFormatID == 0) {
		std::cout << "Recorder couldn't determine proper stream format. ";
		std::cout << "Is the recorder directly after an Audio Unit?" << std::endl;
		return false;
	}
	
	CFURLRef fileURL = CFURLCreateFromFileSystemRepresentation(NULL,
															   (const UInt8*)filePath.c_str(),
															   filePath.length(),
															   false);
    
    //default m4a setting
    AudioStreamBasicDescription outASBD = {
		.mChannelsPerFrame = inASBD.mChannelsPerFrame,
		.mSampleRate = inASBD.mSampleRate,
		.mFormatID = kAudioFormatMPEG4AAC
	};
    
	
    //detect file format from fileName
    std::string ext = "";
     if(filePath.find_last_of(".") != std::string::npos){
        ext = filePath.substr(filePath.find_last_of(".")+1);
    }
    
    OSStatus s;
    //http://kaniini.dereferenced.org/2014/08/31/CoreAudio-sucks.html
    //you can expand wav formats as you like
    if(ext == "wav"){
        outASBD = {
                .mChannelsPerFrame = inASBD.mChannelsPerFrame,
                .mSampleRate = inASBD.mSampleRate,
                .mFormatID = kAudioFormatLinearPCM,
                .mFramesPerPacket = 1, //For uncompressed audio, the value is 1. For variable bit-rate formats, the value is a larger fixed number, such as 1024 for AAC
                //S16_BE
                //.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
                //S16_LE...better for Ableton
                .mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
                .mBitsPerChannel = 16,
                .mBytesPerFrame = inASBD.mChannelsPerFrame * 2,
                .mBytesPerPacket = inASBD.mChannelsPerFrame * 2
        };
        
        
        s = ExtAudioFileCreateWithURL(fileURL,
                kAudioFileWAVEType,
                &outASBD,
                NULL,
                kAudioFileFlags_EraseFile,
                &_recordFile);
    }else{
        //default m4a
        s = ExtAudioFileCreateWithURL(fileURL,
                kAudioFileM4AType,
                &outASBD,
                NULL,
                kAudioFileFlags_EraseFile,
                &_recordFile);
    
    }
    

	CFRelease(fileURL);
	
	if(s != noErr) {
		std::cout << "Couldn't create audio file: "<<filePath<<" err code: " << (OSStatus)s << std::endl;
		return false;
	}
	
	s = ExtAudioFileSetProperty(_recordFile,
								kExtAudioFileProperty_ClientDataFormat,
								sizeof(inASBD),
								&inASBD);
	
	
	if(s != noErr) {
		std::cout << "Couldn't set client format: " << (OSStatus)s << std::endl;
		return false;
	}
	
	setProcessCallback((AURenderCallbackStruct){Record, _recordFile});
	
	return true;
}

void ofxAudioUnitRecorder::stopRecording() {
	if(_recordFile) {
		setProcessCallback((AURenderCallbackStruct){0}); // stop calling Record callback
		ExtAudioFileDispose(_recordFile);
		_recordFile = NULL;
	}
}

OSStatus Record(void * inRefCon,
				AudioUnitRenderActionFlags * ioActionFlags,
				const AudioTimeStamp *	inTimeStamp,
				UInt32 inBusNumber,
				UInt32	inNumberFrames,
				AudioBufferList * ioData)
{
	ExtAudioFileRef file = (ExtAudioFileRef)inRefCon;
	
	OSStatus s = ExtAudioFileWriteAsync(file, inNumberFrames, ioData);
	
	if(s != noErr) {
		std::cout << "error while recording audio: " << (OSStatus)s << std::endl;
	}
	
	return noErr;
}
