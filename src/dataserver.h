#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "constants.h"

struct DataserverSettings {
    std::string  xpub_ip   =  "0.0.0.0";
    unsigned int xpub_port = 5555;

    std::string  pub_ip   = "0.0.0.0";
    unsigned int pub_port = 5554;

    std::string  sub_ip   = "0.0.0.0";
    unsigned int sub_port = 5553;

    std::string osc_ip = "127.0.0.1";
    unsigned int osc_port = 5558;

    int timeout    = 100;

	DataserverSettings(){}
	DataserverSettings(std::string xpub_ip, std::string osc_ip):
        xpub_ip(xpub_ip),
        osc_ip(osc_ip)
    {}
};


class Dataserver : public ofThread {

public:

    Dataserver();
    ~Dataserver();

    void setup(zmq::context_t & ctx, DataserverSettings settings);

    void threadedFunction();

private:

    bool isSetup = false;    
    DataserverSettings settings;
    shared_ptr<zmq::socket_t> sub;
    shared_ptr<zmq::socket_t> sub_core;
    shared_ptr<zmq::socket_t> pub_core;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;


    ofxOscSender sender;


};
