#include "ofMain.h"
#include "ofApp.h"

int main(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetupOpenGL(1440, 900, OF_WINDOW);
	ofRunApp( new ofApp());
}
