#include "ofApp.h"

#define PORT    5006

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    myIp = ofSystem("ifconfig mesh-bridge");
    myIp = ofSplitString(myIp, "\n")[1];
    myIp = ofSplitString(myIp, " netmask")[0];
    myIp = ofSplitString(myIp, "inet ")[1];
    
    cout << "IP: " << myIp << endl;
    
    string videoPath = ofToDataPath("fingers.mov", true);
    ofLog() << "videoPath: " << videoPath;
    ofxOMXPlayerSettings settings;
    settings.videoPath = videoPath;
    settings.enableTexture = false;        //default true
    settings.enableLooping = false;        //default true
    settings.enableAudio = false;        //default true, save resources by disabling
    
    //so either pass in the settings
    video.setup(settings);
    video.setPaused(true);
        
#ifndef TARGET_OSX
	consoleListener.setup(this); //allows keys to be entered via terminal remotely (ssh)
#endif
	
//    if(1280. / ofGetWidth() > 720. / ofGetHeight()){
//        w = ofGetWidth(); // Set width to 100%
//        h = 720. / 1280. * ofGetWidth();
//        offset.x = 0;
//        offset.y = (ofGetHeight() - h) * 0.5;
//    } else{
//        w = 720. / 1280. * ofGetHeight();
//        h = ofGetHeight(); // Set height to 100%;
//        offset.x = (ofGetWidth() - w) * 0.5;
//        offset.y = 0;
//    }
    cout << "Use w: " << w << " and h: " << h << endl;
    cout << "Use offset: " << offset << endl;

    setupCam();
	//ImageFilterCollection (filterCollection here) is helper class to iterate through available OpenMax filters
	filterCollection.setup();
    
    setupOsc();
    ofBackground(0);
    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(video.isOpen() && !video.isPlaying()){ // Hide when done
        video.close();
    }
    

    while(receiver.hasWaitingMessages()){ // Receive osc
        ofxOscMessage m;
        receiver.getNextMessage(m);
        cout << m << endl;
        if(m.getAddress() == "/requestPreview"){
            sendPreview(m.getArgAsString(0));
        } else if(m.getAddress() == "/reset"){
            // Reset this app :)
            cout << "Reset this app" << endl;
            string cmd;
            cmd = "sudo /home/pi/openFrameworks/groenten/groentenApp/bin/groentenApp";
            system(cmd.c_str());
            std::exit(0);
        } else if(m.getAddress() == "/setSharpness"){ // -100, 100
            videoGrabber.setSharpness(ofMap(m.getArgAsFloat(0), 0., 1., -100, 100));
        } else if(m.getAddress() == "/setContrast"){// -100, 100
            videoGrabber.setContrast(ofMap(m.getArgAsFloat(0), 0., 1., -100, 100));
        } else if(m.getAddress() == "/setBrightness"){ // 0 - 100
            videoGrabber.setBrightness(ofMap(m.getArgAsFloat(0), 0., 1., 0, 100));
        } else if(m.getAddress() == "/setSaturation"){// -100, 100
            videoGrabber.setSaturation(ofMap(m.getArgAsFloat(0), 0., 1., -100, 100));
        } else if(m.getAddress() == "/setISO"){// 0, 800
            videoGrabber.setISO(ofMap(m.getArgAsFloat(0), 0., 1., 0., 800.));
        } else if(m.getAddress() == "/zoomIn"){
            videoGrabber.zoomIn();
        } else if(m.getAddress() == "/zoomOut"){
            videoGrabber.zoomOut();
        } else if(m.getAddress() == "/resetCamSettings"){
            videoGrabber.resetValues(); //     of resetValues();
        } else if(m.getAddress() == "/setcolorDenoise"){
            videoGrabber.setColorDenoise(m.getArgAsBool(0));
        } else if(m.getAddress() == "/setflickerCancellation"){
            videoGrabber.setFlickerCancellation(m.getArgAsBool(0));
        } else if(m.getAddress() == "/setframeStabilisation"){
            videoGrabber.setFrameStabilization(m.getArgAsBool(0));
        } else if(m.getAddress() == "/setZoomLevel"){
            int toSet = m.getArgAsInt(0);
            int diff = zoomlevel - toSet;
            if(diff < 0){ // Zoom in
                for(int i=0; i<abs(diff); i++){
                    videoGrabber.zoomIn();
                }
            } else{ // Zoom out
                for(int i=0; i<abs(diff); i++){
                    videoGrabber.zoomOut();
                }
            }
            zoomlevel = toSet;
        } else if(m.getAddress() == "/setFade"){
            videoGrabber.setDisplayAlpha(m.getArgAsInt(0));
        } else if(m.getAddress() == "/playMovie"){
            videoGrabber.setDisplayAlpha(0); // Hide cam
            if(video.getCurrentFrame() > 10); // Video has already played?
                video.reopen();
            video.setPaused(false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
//    if(video.isPlaying()){
//        ofSetColor(255, videoBrightness);
//        video.draw(0,0,ofGetWidth(), ofGetHeight());
//    }
//    preview.draw(ofGetHeight() - 400, 0);
//    if(videoGrabber.isTextureEnabled())
//    {
//        //draw a smaller version via the getTextureReference() method
////        int drawWidth = settings.sensorWidth/4;
////        int drawHeight = settings.sensorHeight/4;
//        videoGrabber.getTextureReference().draw(offset.x, offset.y, w, h);
//    }
}

void ofApp::exit(){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
    switch(key){
        case '1':
            videoGrabber.close();
        break;
        case '2':
            setupCam();
        break;
        case '3':
            setupCam(true); // Texture enabled
        break;
        case '4':
            cout << videoGrabber.getPixels().size() << endl;
            break;
        case 'A':{
            offset.y++;
            ofRectangle drawRectangle(offset.x,
                                      offset.y,
                                      w,
                                      h);
            videoGrabber.setDisplayDrawRectangle(drawRectangle);
        }
            break;
        case 'B':{
            offset.y--;
            ofRectangle drawRectangle(offset.x,
                                      offset.y,
                                      w,
                                      h);
            videoGrabber.setDisplayDrawRectangle(drawRectangle);
        }
            break;
        case 'r':
            rotation += 45;
            if(rotation >= 360){
                rotation = 0;
            }
            videoGrabber.setDisplayRotation(rotation);
            break;
    }
}

void ofApp::onCharacterReceived(KeyListenerEventData& e){
	keyPressed((int)e.character);
}

void ofApp::sendPreview(string ip){
    if(!bSenderSet){
        if(sender.setup(ip, 5004))
            bSenderSet = true;
    }
    if(bSenderSet){
        // Stop cam
        cout << "Close cam" << endl;
        videoGrabber.close();
        ofSleepMillis(500);
        cout << "Run raspistill" << endl;
        ofSystem("/usr/bin/raspistill -t 1 -w 434 -h 324 -q 20 -o /home/pi/cam.jpg");
        
        ofSleepMillis(500);
        ofBuffer b = ofBufferFromFile("/home/pi/cam.jpg", true);
        
        ofxOscMessage reply;
        reply.setAddress("/preview");
        reply.addStringArg(myIp);
        reply.addBlobArg(b);
        sender.sendMessage(reply);
        cout << "Msg send to " << ip << endl;
        
        ofSleepMillis(2000);
        // Restart camera
        setupCam();
    }
    
}

void ofApp::setupOsc(){
//    sender.setup("192.168.0.1", 5000);
    receiver.setup(PORT);
}

void ofApp::setupCam(bool bUseTexture){
//    settings.resetValues();
    
    settings.enableTexture = bUseTexture;
//    settings.enablePixels = true;
    if(bUseTexture){
        settings.sensorWidth = 1280;
        settings.sensorHeight = 720;
    } else{
        settings.sensorWidth = w;
        settings.sensorHeight = h;
    }
    
//    settings.videoGrabberListener = this; //calls onRecordingComplete
    cout << "Settings set, setup videoGrabber" << endl;
    videoGrabber.setup(settings);
    
    cout << "Set display-rect" << endl;
    ofRectangle drawRectangle(offset.x, offset.y, w, h);
    videoGrabber.setDisplayDrawRectangle(drawRectangle);
}

