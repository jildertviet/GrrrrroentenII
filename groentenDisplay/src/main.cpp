#include "ofApp.h"

int main(int argc, char *argv[]){
    ofSetupOpenGL(1280, 720, OF_WINDOW);

    ofApp *app = new ofApp();
    app->arguments = vector<string>(argv, argv + argc);
    for(int i=0; i<app->arguments.size(); i++){
        cout << "Arg[ " << i << "]: " << app->arguments[i] << endl;
    }
    
    ofRunApp(app);
}
