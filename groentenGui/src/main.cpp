#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    cout << "Before setup OpenGL" << endl;
    ofAppGLFWWindow window;
	ofSetupOpenGL(&window, 1024,768,OF_WINDOW);			// <-------- setup the GL context

    cout << "Before running app" << endl;
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
