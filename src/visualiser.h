#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "constants.h"


struct VisualiserSettings {
    std::string  xpub_ip = "127.0.0.1";
    unsigned int xpub_port = 5555;

    int timeout = 10;
	unsigned int buffer_size = 512;

	VisualiserSettings(){}
	VisualiserSettings(std::string xpub_ip):
        xpub_ip(xpub_ip)
    {}
};


class Visualiser : public ofThread {

public:

    Visualiser();

    void setup(zmq::context_t & ctx, VisualiserSettings settings);

	void draw(ofFbo & fbo );

    void threadedFunction();

	void incrementSelected(){
		selected_reading++;
		if(selected_reading == all_readings.end()) selected_reading = all_readings.begin();
	}

private:

    bool isSetup = false;    
    VisualiserSettings settings;
    shared_ptr<zmq::socket_t> sub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;

	std::array<ofPolyline, DATE_NUM_CHANNELS> display_lines;

	std::map<int, std::array<std::vector<glm::vec3>, DATE_NUM_CHANNELS>> all_readings;
	std::map<int, std::array<std::vector<glm::vec3>, DATE_NUM_CHANNELS>>::const_iterator selected_reading;
};



		// std::vector<int> suits{
		// 	1, 2, 3
		// };

		// std::map<int, Readings> data;

		// std::map<int, array<ofColor, NUM_CHANNELS>> colors {
		// 	{1, { 
		// 		ofColor::fromHex(ofHexToInt("47b1b9")),
		// 		ofColor::fromHex(ofHexToInt("499dbb")),
		// 		ofColor::fromHex(ofHexToInt("4a8abd")),
		// 		ofColor::fromHex(ofHexToInt("4c76be")),
		// 		ofColor::fromHex(ofHexToInt("4e62c0")),
		// 		ofColor::fromHex(ofHexToInt("504fc1")),
		// 		ofColor::fromHex(ofHexToInt("6751c3")),
		// 		ofColor::fromHex(ofHexToInt("7e52c5"))
		// 	}},
		// 	{2, { 
		// 		ofColor::fromHex(ofHexToInt("453283")),
		// 		ofColor::fromHex(ofHexToInt("563286")),
		// 		ofColor::fromHex(ofHexToInt("673289")),
		// 		ofColor::fromHex(ofHexToInt("7a328c")),
		// 		ofColor::fromHex(ofHexToInt("8e338f")),
		// 		ofColor::fromHex(ofHexToInt("913381")),
		// 		ofColor::fromHex(ofHexToInt("943371")),
		// 		ofColor::fromHex(ofHexToInt("973360"))
		// 	}},
		// 	{3, { 
		// 		ofColor::fromHex(ofHexToInt("34192d")),
		// 		ofColor::fromHex(ofHexToInt("401d38")),
		// 		ofColor::fromHex(ofHexToInt("4d2042")),
		// 		ofColor::fromHex(ofHexToInt("246065")),
		// 		ofColor::fromHex(ofHexToInt("254f68")),
		// 		ofColor::fromHex(ofHexToInt("253e6a")),
		// 		ofColor::fromHex(ofHexToInt("262b6d")),
		// 		ofColor::fromHex(ofHexToInt("352670"))
		// 	}}
		// };
		// std::map<int, array<ofColor, NUM_CHANNELS>> colors {
		// 	{1, { 
		// 		ofColor::fromHex(ofHexToInt("47b1b9")),
		// 		ofColor::fromHex(ofHexToInt("499dbb")),
		// 		ofColor::fromHex(ofHexToInt("4a8abd")),
		// 		ofColor::fromHex(ofHexToInt("4c76be")),
		// 		ofColor::fromHex(ofHexToInt("4e62c0")),
		// 		ofColor::fromHex(ofHexToInt("504fc1")),
		// 		ofColor::fromHex(ofHexToInt("6751c3")),
		// 		ofColor::fromHex(ofHexToInt("7e52c5"))
		// 	}},
		// 	{2, { 
		// 		ofColor::fromHex(ofHexToInt("453283")),
		// 		ofColor::fromHex(ofHexToInt("563286")),
		// 		ofColor::fromHex(ofHexToInt("673289")),
		// 		ofColor::fromHex(ofHexToInt("7a328c")),
		// 		ofColor::fromHex(ofHexToInt("8e338f")),
		// 		ofColor::fromHex(ofHexToInt("913381")),
		// 		ofColor::fromHex(ofHexToInt("943371")),
		// 		ofColor::fromHex(ofHexToInt("973360"))
		// 	}},
		// 	{3, { 
		// 		ofColor::fromHex(ofHexToInt("34192d")),
		// 		ofColor::fromHex(ofHexToInt("401d38")),
		// 		ofColor::fromHex(ofHexToInt("4d2042")),
		// 		ofColor::fromHex(ofHexToInt("246065")),
		// 		ofColor::fromHex(ofHexToInt("254f68")),
		// 		ofColor::fromHex(ofHexToInt("253e6a")),
		// 		ofColor::fromHex(ofHexToInt("262b6d")),
		// 		ofColor::fromHex(ofHexToInt("352670"))
		// 	}}
		// };

		// array<string, NUM_CHANNELS> ch_names {
		// 	"R Shoulder",
		// 	"R Knee",
		// 	"R Foot",
		// 	"R Elbow",
		// 	"L Elbow",
		// 	"L Foot",
		// 	"L Knee",
		// 	"L Shoulder"
		// };

		// int sSuit = 1;

		// map<int, int> srt_times;
		// map<int, int> srt_rtimes;