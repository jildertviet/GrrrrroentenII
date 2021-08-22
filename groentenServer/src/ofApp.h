#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
        int getRunningAppID();
        bool stopApp();
        void startApp(int index=0);
    ofxOscReceiver receiver;
    vector<ofxOscSender*> senders;
    string myIp;
};
