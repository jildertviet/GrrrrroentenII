#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ipScanner.h"
#include "ofxOsc.h"

#define NUM_CAMS    5
#define NUM_CUES    5
#define NUM_SCENES  32

class camData{
public:
    ofImage preview;
    ofVec2f pos, actualPos, speed, destination, direction;
    ofRectangle bounds;
    float lag = 0.25;
    string ip = "";
    bool bOnline = false;
    ofVec2f cuePoints[NUM_CUES];
    ofxOscSender sender, appSender;
    ofxOscSender groentenServerSender;
    ofxOscMessage panMsg, tiltMsg;
    float sharpness = 0;
    float contrast = 0;
    float brightness = 50;
    float saturation = 0;
    int fadeLevel = 255;
    int zoomlevel = 0;
    char numCuePoints = 0;
    void goToCuePoint(char index){
        destination = cuePoints[index];
    }
    void addCuePoint(ofVec2f c, char index){
        cuePoints[index] = c;
//        numCuePoints++;
//        if(numCuePoints >= 3){
//            cout << "Max num cuePoints" << endl;
//            numCuePoints = 2;
//        }
    }
    char setState(bool state){
        bool bChanged = false;
        if(bOnline != state){
            cout << "Cam state changed! - " << state << " - " << ip << endl;
            if(state){
                sender.setup(ip, 5005); // Pan/tilt - oscServer.py
                appSender.setup(ip, 5006); // Cam-app
                groentenServerSender.setup(ip, 5007); // Server
            }
            bChanged = true;
        }
        bOnline = state;
        if(bChanged && bOnline){
            return 1;
        } else if(bChanged && !bOnline){
            return 2;
        } else{
            return 0;
        }
        
    }
    void update(){
        if(pos != destination){
            direction = destination - pos;
            speed = direction.normalize() * lag * 0.1;
            pos += speed;
            if(bOnline){
                panMsg.clear();
                tiltMsg.clear();
                panMsg.setAddress("/pan");
                tiltMsg.setAddress("/tilt");
                
                panMsg.addFloatArg(pos.x * 80 * -1); // -1 <-> 1 to -90 <-> 90
                tiltMsg.addFloatArg(pos.y * 80); // -1 <-> 1 to -90 <-> 90
                
                sender.sendMessage(panMsg);
                sender.sendMessage(tiltMsg);
            }
        }
        if(pos.distance(destination) < ofMap(lag, 0, 1.0, 0.001, 0.1) && pos.distance(destination) != 0)
            pos = destination;
    }
    
    void draw(){
        ofSetColor(ofColor::red);
        ofPushMatrix();
        ofTranslate(bounds.x, bounds. y);
        ofDrawCircle(pos.x * bounds.getWidth(), pos.y * bounds.getHeight(), 10);
        ofPopMatrix();
    }
};

class ofApp : public ofBaseApp{

	public:
        void setup();
        void draw();
        void update();
        void exit();
    
        ofxDatGui* gui;

        void keyPressed(int key);
        void keyReleased(int key);
    
        void on2dPadEvent(ofxDatGui2dPadEvent e);
        void onMatrixEvent(ofxDatGuiMatrixEvent e);
        void onSliderEvent(ofxDatGuiSliderEvent e);
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void onToggleEvent(ofxDatGuiToggleEvent e);
    
        char selectedCam = 0;
        vector<ofxDatGuiTheme*> themes;
    
        camData camDatas[NUM_CAMS];
        ipScanner ip;
        ofxOscReceiver receiver;
        ofxOscSender displayApp;
        ofxOscSender camSelector;
        bool bShiftPressed = false;
        float stepSize = 0.1;
    void selectCam(char index);
    bool bPause = false;
    
    void saveCuePoints();
    void readCuePoints();
    void switchCamera(int id, int brightness=255);
//    void setZoomLevel(int id=-1);
};
