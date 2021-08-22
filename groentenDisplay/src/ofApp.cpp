#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(30);
    player = new ofxOMXPlayer();

    ofSetFullscreen(true);
    ofBackground(0,0,0);
    receiver.setup(5009);
    
    if(arguments.size() >= 3){
        int index = ofToInt(arguments.at(2));
        selectCam(index);
    } else {
        cout << "No arguments, use default" << endl;
        selectCam(0);
    }
}

void ofApp::update(){
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        cout << m << endl;
        if(m.getAddress() == "/setCamFade"){
            player->setAlpha(m.getArgAsInt(0));
        } else if(m.getAddress() == "/selectCam"){
            selectCam(m.getArgAsInt(0));
        } else if(m.getAddress() == "/playMovie"){
            playMovie(m.getArgAsInt(0));
        } else if(m.getAddress() == "/setFade"){
            globalFade = m.getArgAsInt(0);
            videoPlayer.setAlpha(m.getArgAsInt(0));
        } else if(m.getAddress() == "/pauseMovie"){
            videoPlayer.setPaused(m.getArgAsBool(0));
        }
    }
}

void ofApp::draw(){
//    ofSetColor(0, 255-globalFade);
//    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
}

void ofApp::keyPressed(int key){
    if (key == '0'){
        selectCam(0);
    } else if(key == '1'){
        selectCam(1);
    }
}

void ofApp::selectCam(char index){
    cout << "Select cam: " << (int)index << endl;
    
    string videoPath;
    switch(index){
        case 0:
            videoPath = "http://192.168.0.2:8000/stream.mjpg";
            break;
        case 1:
            videoPath = "http://192.168.0.3:8000/stream.mjpg";
            break;
        case 2:
            videoPath = "http://192.168.0.4:8000/stream.mjpg";
            break;
        case 3:
            videoPath = "http://192.168.0.5:8000/stream.mjpg";
            break;
        case 4:
            videoPath = "http://192.168.0.6:8000/stream.mjpg";
            break;
    }
    
    ofxOMXPlayerSettings settings;
    settings.videoPath = videoPath;
    settings.useHDMIForAudio = false;    //default true
    settings.enableTexture = false;        //default true
    settings.enableAudio = false;        //default true, save resources by disabling

    player->setup(settings);
    ofSleepMillis(200);
    player->setAlpha(0);
    
    for(char i=0; i<2; i++)
        player->increaseSpeed();
}

void ofApp::playMovie(char index){
//    player->close(); // Should close the player?
    
    player->setAlpha(0);
    cout << "Play movie: " << (int)index << endl;
    
    string videoPath = ofToString((int)index);
    videoPath += ".mov";
    videoPath = ofToDataPath(videoPath, true);
//    switch(index){
//        case 0:
//            videoPath = ofToDataPath("fingers.mov", true);
//            break;
//        case 1:
//            videoPath = ofToDataPath("Bsss.mov", true);
//            break;
//    }
    
    cout << "Play video: " << videoPath << endl;
    ofxOMXPlayerSettings settings;
    settings.videoPath = videoPath;
    settings.useHDMIForAudio = false;    //default true
    settings.enableTexture = false;        //default true
    settings.enableLooping = true;        //default true
    settings.enableAudio = false;        //default true, save resources by disabling
    
    videoPlayer.setup(settings);
}
