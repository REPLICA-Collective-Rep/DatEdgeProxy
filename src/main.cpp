#include "ofMain.h"
#include "ofApp.h"
#include "dateoptions.h"

//========================================================================
int main(int argc, char *argv[] ){	// <-------- setup the GL context
	OptionsParser parser = OptionsParser(argc, argv);

	ofSetupOpenGL(1024,768,OF_WINDOW);		
	ofRunApp(new ofApp());

}
