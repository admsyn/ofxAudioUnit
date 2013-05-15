#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofSetVerticalSync(true);

//	This example demonstrates how to list and access the parameters from non-Apple Audio Units.
//  This example uses Elek7ro - a free AU Synthesizer form Togu Audio Line
//  please download it and install it on your system before running this example:
//  http://kunz.corrupt.ch/Products/TAL-Elek7ro
//  feel free to use an AU of your choice with this example by changing the loading parameters!

//  load Audio Unit
//  run 'auval -a' cmd on terminal to list all available AU's
    externalAU = ofxAudioUnit('aumu' ,'Ele2','Togu');
    
//  connect AU to output 
    externalAU.connectTo(output);
    
//  get specific Parameter
    AudioUnitParameterInfo info = ofxAudioUnitGetParametersExtra::getSpecificParameterData(synth, 0);

    

    cout<< "name:: "        <<info.name         <<endl;
    cout<< "minValue:: "    <<info.minValue     <<endl;
    cout<< "maxValue:: "    <<info.maxValue     <<endl;
    cout<< "defaultValue:: "<<info.defaultValue <<endl;

//  get all parameters in a vector
    vector < AudioUnitParameterInfo > infoList = ofxAudioUnitGetParametersExtra::getAllParametersData(synth);
 
    for (int i = 0; i < infoList.size(); i++) {
        cout<< "ID::"           <<i                         <<endl;
        cout<< "name:: "        <<infoList[i].name          <<endl;
        cout<< "minValue:: "    <<infoList[i].minValue      <<endl;
        cout<< "maxValue:: "    <<infoList[i].maxValue      <<endl;
        cout<< "defaultValue:: "<<infoList[i].defaultValue  <<endl;
    }
}

//--------------------------------------------------------------
void testApp::update(){}
//--------------------------------------------------------------
void testApp::draw(){}
//--------------------------------------------------------------
void testApp::keyPressed(int key){}
//--------------------------------------------------------------
void testApp::keyReleased(int key){}
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}