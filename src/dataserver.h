#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "constants.h"

struct DataserverSettings {
    std::string  xpub_ip   = "127.0.0.1";
    unsigned int xpub_port = 5555;

    std::string  pub_ip   = "127.0.0.1";
    unsigned int pub_port = 5554;

    int timeout    = 10;

	DataserverSettings(){}
	DataserverSettings(std::string xpub_ip):
        xpub_ip(xpub_ip)
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
    shared_ptr<zmq::socket_t> pub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;


};
