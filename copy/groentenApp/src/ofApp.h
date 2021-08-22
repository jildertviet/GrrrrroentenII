#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxOMXVideoGrabber.h"
#include "ImageFilterCollection.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp, public KeyListener, public ofxOMXVideoGrabberListener{
	public:
        void setup();
		void update();
		void draw();
		void keyPressed(int key);
    void exit();

        void onCharacterReceived(KeyListenerEventData& e);
    
        ofxOMXVideoGrabber videoGrabber;
        ImageFilterCollection filterCollection;
        ofxOMXCameraSettings settings;
    void setupCam(bool bUseTexture = false);
    void sendPreview(string ip);
    
    ofxOscReceiver receiver;
    ofxOscSender sender;
    bool bSenderSet = false;
    string myIp = "";
    
    void setupOsc();
    int w = 1280;
    int h = 720;
    ofVec2f offset = ofVec2f(0,0);
    int rotation = 0;
    
    ofImage preview;
    void onRecordingComplete(string filePath) override;
    unsigned int frameNumRecordingStarted;
    bool bRecording = false;
    
#ifndef TARGET_OSX
    TerminalListener consoleListener;
#endif
};

