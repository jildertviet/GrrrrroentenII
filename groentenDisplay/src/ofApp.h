#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxOsc.h"

class ofApp: public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    ofxOMXPlayer* player;
    ofxOMXPlayer videoPlayer;
    
    void selectCam(char index);
    int camFade = 255;
    int globalFade = 0;
    void playMovie(char index);
    vector<string> arguments;
private:
    ofxOscReceiver receiver;
};
