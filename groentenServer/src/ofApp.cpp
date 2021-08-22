#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    receiver.setup(5007);
    ofSetFrameRate(30);
//    myIp = ofSystem("ifconfig eth0");
//    myIp = ofSplitString(myIp, "\n")[1];
//    myIp = ofSplitString(myIp, " netmask")[0];
//    myIp = ofSplitString(myIp, "inet ")[1];
//
//    cout << "IP: " << myIp << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    while(receiver.hasWaitingMessages()){ // Receive osc
        ofxOscMessage m;
        receiver.getNextMessage(m);
        cout << m << endl;
        
        if(m.getAddress() == "/requestPreview"){
            string ip = m.getArgAsString(0);
            stopApp();
            ofSleepMillis(200);
            cout << "Run raspistill" << endl;
            ofSystem("/usr/bin/raspistill -t 1 -w 217 -h 162 -q 20 -o /home/pi/cam.jpg");
            
            ofSleepMillis(200);
            ofBuffer b = ofBufferFromFile("/home/pi/cam.jpg", true);
            
            ofxOscMessage reply;
            reply.setAddress("/preview");
            reply.addStringArg(myIp);
            reply.addBlobArg(b);
//            sender.sendMessage(reply);
            
            bool bFound = false;
            ofxOscSender* s;
            for(int i=0; i<senders.size(); i++){
                if(senders[i]->getHost() == ip){
                    bFound = true;
                    s = senders[i];
                    cout << "Reuse oscSender" << endl;
                }
            }
            if(!bFound){
                cout << "new oscSender" << endl;
                s = new ofxOscSender();
                s->setup(ip, 5004); // Gui-app
                senders.push_back(s);
            }
            
            s->sendMessage(reply);
            
            cout << "Msg send to " << ip << endl;

            ofSleepMillis(200);
            startApp();
        } else if(m.getAddress() == "/stop"){
            stopApp();
        } else if(m.getAddress() == "/start"){
            startApp();
        } else if(m.getAddress() == "/reset"){
            stopApp();
            ofSleepMillis(200);
            startApp();
        } else if(m.getAddress() == "/resetSender"){
            senders.clear();
        } else if(m.getAddress() == "/selectCam"){
            stopApp(); // Stop current app
            ofSleepMillis(1000);
            // Start new app w/ argument
            startApp(m.getArgAsInt(0));
        } else if(m.getAddress() == "/shutdown"){
            cout << "Bye!" << endl;
            system("sudo halt");
        }
    }
}

void ofApp::startApp(int index){
    cout << "startApp" << endl;
    string cmd = "sudo /home/pi/openFrameworks/apps/groenten/groentenDisplay/bin/groentenDisplay --args ";
    cmd += ofToString(index);
    cmd += " &";
    cout << cmd << endl;
    system(cmd.c_str());
}

bool ofApp::stopApp(){
    int processId = getRunningAppID();
    cout << "Stop app: " << processId << endl;
    if(processId){ // Stop app
        string cmd = "sudo kill -9 "; cmd += ofToString(processId);
        ofSystem(cmd);
//        ofSleepMillis(5000);
//        system("sudo fuser -k 5051/tcp"); // Clear TCP port, maybe do udp port as well?
        return true;
    }
    return false;
}

int ofApp::getRunningAppID(){ // PID*
    string result = ofSystem("ps -A | grep 'groentenDisplay'");
    int id;
    if(ofStringTimesInString(result, "groentenDisplay") != 0){
        // pingApp is running!
        string idString = ofSplitString(result, "?")[0];
        id = ofToInt(idString);
        cout << "groentenDisplay ID: " << id << endl;
        return id;
    }
    return 0;
}
