#pragma once
#include <regex>
#include "ofMain.h"
#include "ofxOsc.h"


#define BUFF_SIZE 5000
#define NUM_CHANNELS 8


class ofApp : public ofBaseApp{

	public:

		struct Reading { array<float, NUM_CHANNELS> data; int time; int rtime; };
		typedef std::deque<Reading> Readings;

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

		ofxOscReceiver receiver;
		
		std::vector<int> suits{
			1, 2, 3
		};

		std::map<int, Readings> data;

		std::map<int, array<ofColor, NUM_CHANNELS>> colors {
			{1, { 
				ofColor::fromHex(ofHexToInt("47b1b9")),
				ofColor::fromHex(ofHexToInt("499dbb")),
				ofColor::fromHex(ofHexToInt("4a8abd")),
				ofColor::fromHex(ofHexToInt("4c76be")),
				ofColor::fromHex(ofHexToInt("4e62c0")),
				ofColor::fromHex(ofHexToInt("504fc1")),
				ofColor::fromHex(ofHexToInt("6751c3")),
				ofColor::fromHex(ofHexToInt("7e52c5"))
			}},
			{2, { 
				ofColor::fromHex(ofHexToInt("453283")),
				ofColor::fromHex(ofHexToInt("563286")),
				ofColor::fromHex(ofHexToInt("673289")),
				ofColor::fromHex(ofHexToInt("7a328c")),
				ofColor::fromHex(ofHexToInt("8e338f")),
				ofColor::fromHex(ofHexToInt("913381")),
				ofColor::fromHex(ofHexToInt("943371")),
				ofColor::fromHex(ofHexToInt("973360"))
			}},
			{3, { 
				ofColor::fromHex(ofHexToInt("34192d")),
				ofColor::fromHex(ofHexToInt("401d38")),
				ofColor::fromHex(ofHexToInt("4d2042")),
				ofColor::fromHex(ofHexToInt("246065")),
				ofColor::fromHex(ofHexToInt("254f68")),
				ofColor::fromHex(ofHexToInt("253e6a")),
				ofColor::fromHex(ofHexToInt("262b6d")),
				ofColor::fromHex(ofHexToInt("352670"))
			}}
		};

		array<string, NUM_CHANNELS> ch_names {
			"R Shoulder",
			"R Knee",
			"R Foot",
			"R Elbow",
			"L Elbow",
			"L Foot",
			"L Knee",
			"L Shoulder"
		};

		int sSuit = 1;

		map<int, int> srt_times;
		map<int, int> srt_rtimes;

};
