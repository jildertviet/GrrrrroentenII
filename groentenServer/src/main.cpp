#include "ofApp.h"
#include "ofAppNoWindow.h"

int main(){
    ofAppNoWindow window;
    ofSetupOpenGL(&window, 1, 1, OF_WINDOW);
    return ofRunApp(std::make_shared<ofApp>());
}
