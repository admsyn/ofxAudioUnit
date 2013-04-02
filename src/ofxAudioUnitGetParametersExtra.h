//
//  ofxAudioUnitGetParametersExtra.h
//   
//
//  Created by Rui Pereira (rux) on 4/1/13
//
//

#pragma once

#include "CAAUParameter.h"
#include "AUParamInfo.h"

// returns either a list of all the parameters
// or a specific parameter and their properties
//
// properties can be accessed by the following syntax: 
// .name            +
// .unitName        -
// .clumpID         -
// .cfNameString    -
// .unit            -
// .minValue        +
// .maxValue        +
// .defaultValue    +
// .flags           -

class ofxAudioUnitGetParametersExtra{
public:
    
    static vector <AudioUnitParameterInfo> getAllParametersData(ofxAudioUnit &AU){
        
        vector <AudioUnitParameterInfo> infoStructAUList;
        
        AUParamInfo info(AU, false, false);
        
        for (int i = 0; i < info.NumParams(); i ++) {
            if (info.GetParamInfo(i) != NULL) {
                AudioUnitParameterInfo tempInfo = info.GetParamInfo(i)->ParamInfo();
                infoStructAUList.push_back(tempInfo);
            }
        }
        return infoStructAUList;
        ofLog(OF_LOG_NOTICE, "there are " +  ofToString(info.NumParams()) + " parameters available");

    };
    
    static AudioUnitParameterInfo getSpecificParameterData(ofxAudioUnit &AU, int _id){
        AudioUnitParameterInfo infoStructAU;
        
        AUParamInfo info(AU, false, false);
        
        if (info.GetParamInfo(_id) != NULL && _id < info.NumParams()) {
            infoStructAU = info.GetParamInfo(_id)->ParamInfo();
            return infoStructAU;
            ofLog(OF_LOG_NOTICE, "parameter " + ofTostring(_id) + " of " + ofToString(info.NumParams()) + " parameters");

        } else {
            ofLog(OF_LOG_NOTICE, "the parameter requested doens't exist! there are only " +  ofToString(info.NumParams()) + " parameters available");
        }
        
    };
    

};