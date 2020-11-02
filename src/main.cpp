#include "ofMain.h"
#include "ofApp.h"
#include "dateoptions.h"

//========================================================================
int main(int argc, char *argv[] ){	// <-------- setup the GL context
	Args args = Args(argc, argv);


	if(args.debug):
		ofSetupOpenGL(1024,768,OF_WINDOW);		
	else:
		ofSetupOpenGL(1024,768,OF_FULLSCREEN);	

	ofRunApp(new ofApp(args));

}
