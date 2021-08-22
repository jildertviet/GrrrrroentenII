#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetWindowShape(ofGetScreenWidth() / 4, ofGetScreenHeight() / 2);
    ofSetWindowTitle("Grrrrroenten II cams");
    
    vector<string> ips = {
        "192.168.0.2",
        "192.168.0.3",
        "192.168.0.4",
        "192.168.0.5",
        "192.168.0.6"
    };
    ip.ips = ips;
    
    for(char i=0; i<NUM_CAMS; i++)
        camDatas[i].ip = ips[i];
    
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    gui->setAutoDraw(false);
    
    ofxDatGui2dPad* pad = gui->add2dPad("Pan / tilt");
    ofRectangle r = ofRectangle(0, 0, 1.0, 1.0);
    pad->setBounds(r);
    gui->addSlider("Lag (0-1)", 0., 1.0, camDatas[selectedCam].lag);
    
    gui->addTextInput("ip", camDatas[selectedCam].ip);
    gui->addBreak();
//    gui->addButton("Reset cam");
    gui->addButton("Open preview-app");
    gui->addButton("SSH");
//    gui->addButton("Shut down");
//    gui->addButton("Stop cam");
    gui->addButton("Pause / play");
//    gui->addButton("Start cam");
//    gui->addButton("Add cuePoint");
//    gui->addButton("Play video");
    gui->addBreak();
    
    gui->addSlider("Sharpness", -100, 100, 0.1); // -100, 100, 0
    gui->addSlider("Contrast", -80, 80, 0.1); // -100, 100, 0
    gui->addSlider("Brightness", 20, 80, 50); // 100, 50
    gui->addSlider("Saturation", -100, 100, 0.1); // -100, 100, 0
    gui->addSlider("Zoom level", 0, 20, 0);

    gui->addBreak();
    gui->addSlider("camFade", 0, 255, 255); // Master fade
    gui->addSlider("videoFade", 0, 255, 255); // Master fade
    
//    gui->addButton("Zoom in");
//    gui->addButton("Zoom out");
    gui->addBreak();
    
//    gui->addToggle("frameStabilisation"); // Works, but necessary?
//    gui->addToggle("flickerCancellation");
//    gui->addToggle("colorDenoise");
//    gui->addButton("Reset settings");
    
    gui->addMatrix("Cam preview", NUM_CAMS, true);
    gui->getMatrix("Cam preview")->setRadioMode(true);
    
    gui->addMatrix("Cam select", NUM_CAMS, true);
    gui->getMatrix("Cam select")->setRadioMode(true);
    
    gui->addMatrix("playVideo", 16, true);
    gui->getMatrix("playVideo")->setRadioMode(true);
    
    gui->addMatrix("cuePoint", NUM_CUES, true);
    gui->getMatrix("cuePoint")->setRadioMode(true);
    
    gui->addMatrix("scenes", NUM_SCENES, true);
    gui->getMatrix("scenes")->setRadioMode(true);

    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);

    ofxDatGuiTheme* t = new ofxDatGuiThemeMidnight();
    t->layout.breakHeight = 30.;
    gui->setTheme(t);
    
    ofSetWindowShape(gui->getWidth(), gui->getHeight() + 450);

    receiver.setup(5004);
    displayApp.setup("192.168.0.11", 5009);
    camSelector.setup("192.168.0.11", 5007);
    
    readCuePoints();
}

void ofApp::exit(){
    ip.bStop = true;
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e){
    if(e.target->is("frameStabilisation")){
        ofxOscMessage m;
        m.setAddress("/setframeStabilisation");
        m.addBoolArg(e.target->getChecked());
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("flickerCancellation")){
        ofxOscMessage m;
        m.setAddress("/setflickerCancellation");
        m.addBoolArg(e.target->getChecked());
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("colorDenoise")){
        ofxOscMessage m;
        m.setAddress("/setcolorDenoise");
        m.addBoolArg(e.target->getChecked());
        camDatas[selectedCam].appSender.sendMessage(m);
    }
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target->is("Reset cam")){
        ofxOscMessage m;
        m.setAddress("/reset");
        camDatas[selectedCam].groentenServerSender.sendMessage(m); // 5007
    } else if(e.target->is("Get preview")){
        ofxOscMessage m;
        m.setAddress("/requestPreview");
        m.addStringArg("192.168.0.10"); // Simply my IP...
        camDatas[selectedCam].groentenServerSender.sendMessage(m);
    } else if(e.target->is("Stop cam")){
        ofxOscMessage m;
        m.setAddress("/stop");
        camDatas[selectedCam].groentenServerSender.sendMessage(m);
    } else if(e.target->is("Start cam")){
        ofxOscMessage m;
        m.setAddress("/start");
        camDatas[selectedCam].groentenServerSender.sendMessage(m);
    } else if(e.target->is("Zoom in")){
        ofxOscMessage m;
        m.setAddress("/zoomIn");
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("Zoom out")){
        ofxOscMessage m;
        m.setAddress("/zoomOut");
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("Reset settings")){
        ofxOscMessage m;
        m.setAddress("/resetCamSettings");
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("Play video")){
        ofxOscMessage m;
        m.setAddress("/playMovie");
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("Open preview-app")){
        // Make this relative to the path where this app runs...
        system("/Users/jildertviet/of_v0.11.0_osx_release/apps/myApps/GrrrrroentenII/groentenPreview/bin/groentenPreviewDebug.app/Contents/MacOS/groentenPreviewDebug &");
    } else if(e.target->is("SSH")){
        string cmd = "/usr/bin/osascript -e 'tell application \"Terminal\" to do script \"ssh pi@";
        cmd += camDatas[selectedCam].ip;
        cmd += "\"' &";
        ofSystem(cmd);
        ofSystem("osascript -e 'tell application \"Terminal\" to activate'");
    } else if(e.target->is("Shut down (shift)") && bShiftPressed){
        ofxOscMessage m;
        m.setAddress("/shutdown");
        camDatas[selectedCam].groentenServerSender.sendMessage(m); // Is server running @ cam-node?
    } else if(e.target->is("Pause / play")){
        bPause = !bPause;
        cout << bPause << endl;
        ofxOscMessage m;
        m.setAddress("/pauseMovie");
        m.addBoolArg(bPause);
        displayApp.sendMessage(m);
    }
}

void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e){
    camDatas[selectedCam].destination = ofVec2f(e.x, e.y) * 2.0 - ofVec2f(1, 1);
}

void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e){
    if(e.target->is("Cam preview")){
        selectCam(e.child);
    } else if(e.target->is("Cam select")){
        switchCamera(e.child, 0);
    } else if(e.target->is("playVideo")){
        ofxOscMessage m;
        m.setAddress("/playMovie");
        m.addIntArg(e.child);
        displayApp.sendMessage(m);
        gui->getSlider("camFade")->setValue(0);
        cout << "Send: " << m << endl;
    } else if(e.target->is("cuePoint")){
        if(bShiftPressed){
            camDatas[selectedCam].addCuePoint(camDatas[selectedCam].pos, e.child);
            for(char i=0; i<NUM_CUES; i++){
                if(camDatas[selectedCam].cuePoints[i] != ofVec2f(0, 0)){
                    gui->getMatrix("cuePoint")->getButtonAtIndex(i)->bOnline = true;
                } else{
                    gui->getMatrix("cuePoint")->getButtonAtIndex(i)->bOnline = false;
                }
            }
        } else{
            camDatas[selectedCam].goToCuePoint(e.child);
        }
    } else if(e.target->is("scenes")){
        string fileName = "";
        fileName += ofToString(e.child);
        fileName += ".preset";
        if(bShiftPressed){
            cout << "Create file " << fileName << endl;
            // Save current state
            ofFile f(ofToDataPath(fileName), ofFile::WriteOnly);
            f.create();
            f.clear();
            ofBuffer b;
            for(int i=0; i<NUM_CAMS; i++){
                b.append(ofToString(camDatas[i].destination.x) + ";");
                b.append(ofToString(camDatas[i].destination.y) + ";");
                b.append(ofToString(camDatas[i].zoomlevel) + ";");
                b.append(ofToString((int)gui->getMatrix("Cam select")->getSelected()[0]) + "\n");
            }
            f.writeFromBuffer(b);
            f.close();
        } else{
            // Execute current scene
            ofFile f(ofToDataPath(fileName));
            ofBuffer b = f.readToBuffer();
            vector<string> lines = ofSplitString(b.getText(), "\n");
            for(int i=0; i<lines.size(); i++){
                if(lines[i].length() == 0)
                    continue;
                vector<string> arguments = ofSplitString(lines[i], ";");
                if(arguments.size()){
                    camDatas[i].destination.x = ofToFloat(arguments[0]);
                    camDatas[i].destination.y = ofToFloat(arguments[1]);
                    camDatas[i].zoomlevel = ofToFloat(arguments[2]);
                    
                    if(i==0){
                        // Execute once (for master)
                        gui->getMatrix("Cam select")->setSelected(vector<int>{ofToInt(arguments[3])});
                        switchCamera(ofToInt(arguments[3]), 255); // Full brightness = 255
                    }
                    
                    ofxOscMessage m; // Duplicated :/ should be in function?
                    m.setAddress("/setZoomLevel");
                    // Calculate a zoom level
                    float level = camDatas[selectedCam].zoomlevel;
                    float w, h, x, y;
                    w = 1280;
                    h = 720;
                    x = (level/20.) * w * 0.5 * 0.6;
                    y = (level/20.) * h * 0.5 * 0.6;
                    w = w - (2*x);
                    w /= 1280.;
                    h = h - (2*y);
                    h /= 720.;
                    x /= 1280.;
                    y /= 720.; // Scale 0-1
                    
                    cout << x << " " << y << " " << w << " " << h << endl;
                    m.addFloatArg(x);
                    m.addFloatArg(y);
                    m.addFloatArg(w);
                    m.addFloatArg(h);
                    camDatas[selectedCam].appSender.sendMessage(m);
                }
            }
            cout << "Preset:" << endl;
            cout << b.getText() << endl;
            cout << "End of preset:" << endl;
            f.close();
        }
    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    if (e.target->is("Lag (0-1)")){
        camDatas[selectedCam].lag = e.target->getValue();
    } else if(e.target->is("Sharpness")){
        ofxOscMessage m;
        m.setAddress("/setSharpness");
        m.addIntArg(e.target->getValue());
        camDatas[selectedCam].appSender.sendMessage(m);
        camDatas[selectedCam].sharpness = e.target->getValue();
    } else if(e.target->is("Contrast")){
        ofxOscMessage m;
        m.setAddress("/setContrast");
        m.addIntArg(e.target->getValue());
        camDatas[selectedCam].appSender.sendMessage(m);
        camDatas[selectedCam].contrast = e.target->getValue();
    } else if(e.target->is("Brightness")){
        ofxOscMessage m;
        m.setAddress("/setBrightness");
        m.addIntArg(e.target->getValue());
        camDatas[selectedCam].appSender.sendMessage(m);
        camDatas[selectedCam].brightness = e.target->getValue();
    } else if(e.target->is("Saturation")){
        ofxOscMessage m;
        m.setAddress("/setSaturation");
        m.addIntArg(e.target->getValue());
        camDatas[selectedCam].appSender.sendMessage(m);
        camDatas[selectedCam].saturation = e.target->getValue();
    } else if(e.target->is("ISO")){
        ofxOscMessage m;
        m.setAddress("/setISO");
        m.addFloatArg(e.target->getValue());
        camDatas[selectedCam].appSender.sendMessage(m);
    } else if(e.target->is("Zoom level")){
        ofxOscMessage m;
        m.setAddress("/setZoomLevel");
        // Calculate a zoom level
        float level = e.target->getValue();
        float w, h, x, y;
        w = 1280;
        h = 720;
        x = (level/20.) * w * 0.5 * 0.6;
        y = (level/20.) * h * 0.5 * 0.6;
        w = w - (2*x);
        w /= 1280.;
        h = h - (2*y);
        h /= 720.;
        x /= 1280.;
        y /= 720.; // Scale 0-1
        
        cout << x << " " << y << " " << w << " " << h << endl;
        m.addFloatArg(x);
        m.addFloatArg(y);
        m.addFloatArg(w);
        m.addFloatArg(h);
        camDatas[selectedCam].appSender.sendMessage(m);
        
        camDatas[selectedCam].zoomlevel = e.target->getValue();
    } else if(e.target->is("camFade")){
        ofxOscMessage m;
        m.setAddress("/setCamFade");
        m.addIntArg(e.target->getValue());
        displayApp.sendMessage(m);
//        camDatas[selectedCam].fadeLevel = e.target->getValue();
    } else if(e.target->is("videoFade")){
        ofxOscMessage m;
        m.setAddress("/setFade");
        m.addIntArg(e.target->getValue());
        displayApp.sendMessage(m);
        //        camDatas[selectedCam].fadeLevel = e.target->getValue();
    }
}

void ofApp::draw(){
    gui->draw();
    if(camDatas[selectedCam].preview.isAllocated()){
        camDatas[selectedCam].preview.draw(
                                           (ofGetWidth() - (camDatas[selectedCam].preview.getWidth() * 2)) * 0.5, // Center the img
                                           gui->getHeight() + 10,
                                           camDatas[selectedCam].preview.getWidth() * 2,
                                           camDatas[selectedCam].preview.getHeight() * 2);
    }
//    camDatas[selectedCam].draw();
}

void ofApp::update(){
    while(receiver.hasWaitingMessages()){ // Receive osc
        ofxOscMessage m;
        receiver.getNextMessage(m);
        cout << m << endl;
        if(m.getAddress() == "/preview"){
            cout << "Received msg from: " << m.getArgAsString(0) << endl;
            ofBuffer b = m.getArgAsBlob(1);
            camDatas[selectedCam].preview.load(b);
        }
    }
    
    // Get active IP's
    vector<bool> status = ip.getActiveIPs();
    for(char i=0; i<status.size(); i++){
        char state = camDatas[i].setState(status[i]);
        if(state == 1){
            gui->getMatrix("Cam preview")->getButtonAtIndex(i)->setOnline(true);
        } else if(state == 2){
            gui->getMatrix("Cam preview")->getButtonAtIndex(i)->setOnline(false);
        }
    }
    
    gui->update();
    if(ofGetFrameNum() == 1){
        vector<int> v = {0};
        gui->getMatrix("Cam preview")->setSelected(v);
    }
    
    for(char i=0; i<NUM_CAMS; i++){
        camDatas[i].update();
    }
    gui->getSlider("Zoom level")->setValue(camDatas[selectedCam].zoomlevel);
    gui->get2dPad("Pan / tilt")->actualLoc = camDatas[selectedCam].pos;
}

void ofApp::keyPressed(int key){
    switch(key){
        case OF_KEY_LEFT:
            camDatas[selectedCam].destination.x -= stepSize;
            gui->get2dPad("Pan / tilt")->setPoint((camDatas[selectedCam].destination + ofVec2f(1, 1)) / 2.0);
            if(camDatas[selectedCam].destination.x < -1.0){
                camDatas[selectedCam].destination.x = -1.0;
            }
            break;
        case OF_KEY_RIGHT:
            camDatas[selectedCam].destination.x += stepSize;
            gui->get2dPad("Pan / tilt")->setPoint((camDatas[selectedCam].destination + ofVec2f(1, 1)) / 2.0);
            if(camDatas[selectedCam].destination.x > 1.0){
                camDatas[selectedCam].destination.x = 1.0;
            }
            break;
        case OF_KEY_UP:
            camDatas[selectedCam].destination.y -= stepSize;
            gui->get2dPad("Pan / tilt")->setPoint((camDatas[selectedCam].destination + ofVec2f(1, 1)) / 2.0);
            if(camDatas[selectedCam].destination.y < -1.0){
                camDatas[selectedCam].destination.y = -1.0;
            }
            break;
        case OF_KEY_DOWN:
            camDatas[selectedCam].destination.y += stepSize;
            gui->get2dPad("Pan / tilt")->setPoint((camDatas[selectedCam].destination + ofVec2f(1, 1)) / 2.0);
            if(camDatas[selectedCam].destination.y > 1.0){
                camDatas[selectedCam].destination.y = 1.0;
            }
            break;
        case OF_KEY_SHIFT:
            stepSize = 0.025;
            bShiftPressed = true;
            break;
        case 's':
            saveCuePoints();
            break;
        case 'q':{
            ofxOscMessage m;
            m.setAddress("/playMovie");
            m.addIntArg(14);
            displayApp.sendMessage(m);
            
            m.clear();
            m.setAddress("/setCamFade");
            m.addIntArg(0); // ?
            displayApp.sendMessage(m);
            
            gui->getSlider("camFade")->setValue(0);
        }
            break;
        case 'f':{
            ofxOscMessage m;
            m.setAddress("/setCamFade");
            m.addIntArg(255);
            displayApp.sendMessage(m);
            gui->getSlider("camFade")->setValue(255);
        }
            break;
    }
//    if(key >= '1' && key <= '5'){
//        selectCam(key - 48 - 1);
//    }
}

void ofApp::keyReleased(int key){
    switch(key){
        case OF_KEY_SHIFT:
            stepSize = 0.1;
            bShiftPressed = false;
            break;
    }
}

void ofApp::selectCam(char index){
    selectedCam = index;
//    string title = "Camera ";
//    title += (index+48+1);
//    ofSetWindowTitle(title);
    vector<int> v = {index};
    gui->getMatrix("Cam preview")->setSelected(v);
    gui->get2dPad("Pan / tilt")->setPoint((camDatas[selectedCam].destination + ofVec2f(1, 1)) / 2.0);
    gui->getSlider("Lag (0-1)")->setValue(camDatas[selectedCam].lag);
    gui->getTextInput("ip")->setText(camDatas[selectedCam].ip);
    
    gui->getTextInput("ip")->setText(camDatas[selectedCam].ip);
    gui->getSlider("Sharpness")->setValue(camDatas[selectedCam].sharpness);
    gui->getSlider("Contrast")->setValue(camDatas[selectedCam].contrast);
    gui->getSlider("Brightness")->setValue(camDatas[selectedCam].brightness);
    gui->getSlider("Saturation")->setValue(camDatas[selectedCam].saturation);
    gui->getSlider("Zoom level")->setValue(camDatas[selectedCam].zoomlevel);
    
    for(char i=0; i<NUM_CUES; i++){
        if(camDatas[selectedCam].cuePoints[i] != ofVec2f(0, 0)){
            gui->getMatrix("cuePoint")->getButtonAtIndex(i)->bOnline = true;
        } else{
            gui->getMatrix("cuePoint")->getButtonAtIndex(i)->bOnline = false;
        }
    }
//    gui->getSlider("camFade")->setValue(camDatas[selectedCam].fadeLevel);
}

void ofApp::saveCuePoints(){
    string toSave;
    for(char i=0; i<NUM_CAMS; i++){
        for(char j=0; j<NUM_CUES; j++){
            toSave += ofToString(camDatas[i].cuePoints[j].x);
            toSave += ",";
            toSave += ofToString(camDatas[i].cuePoints[j].y);
            toSave += ";";
            //x,y;x,y;x,y
        }
        toSave += "\n";
    }
    cout << "Saving cuePoints: " << endl;
    cout << toSave << endl;
    ofFile f;
    f.open("cuePoints.txt", ofFile::Mode::WriteOnly);
    ofBuffer b;
    b.set(toSave);
    f.writeFromBuffer(b);
    f.close();
    //x,y;x,y;x,y
    //x,y;x,y;x,y
    //x,y;x,y;x,y
    //x,y;x,y;x,y
    //x,y;x,y;x,y
    // Split by newline ; and ,
}

void ofApp::readCuePoints(){
    ofFile f;
    f.open("cuePoints.txt");
    ofBuffer b = f.readToBuffer();
    string content = b.getText();
    vector<string> lines = ofSplitString(content, "\n");
    int index = 0;
    for(char i=0; i<NUM_CAMS; i++){
        vector<string> cues = ofSplitString(lines[i], ";");
        for(char j=0; j<NUM_CUES; j++){
            index = (i*NUM_CUES) + j;
            vector<string> oneCue = ofSplitString(cues[j], ",");
            cout << index << " " << oneCue[0] << " " << oneCue[1] << endl;
            camDatas[i].cuePoints[j] = ofVec2f(ofToFloat(oneCue[0]), ofToFloat(oneCue[1]));
//            gui->getMatrix("cuePoint")->getButtonAtIndex(index);
        }
    }
}

void ofApp::switchCamera(int id, int brightness){
    ofxOscMessage m;
    m.setAddress("/selectCam");
    m.addIntArg(id); // 0 - 4
    camSelector.sendMessage(m);
    gui->getSlider("camFade")->setValue(brightness);
    cout << "Select cam" << endl;
}
