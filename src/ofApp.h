#pragma once
#include "ofMain.h"


#include "dateoptions.h"

#include "proxy.h"
#include "simulate.h"
#include "writer.h"
#include "visualiser.h"
#include "dataserver.h"


class ofApp : public ofBaseApp{

	public:

		ofApp( Args & args ) :
			ofBaseApp(),
			args(args)
			{	}


		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		const Args&  args;

		Proxy proxy;
		Simulate simulate;
		Dataserver dataserver;


		Writer writer;
		Visualiser visualiser;

		ofFbo graphFbo;
	
};
