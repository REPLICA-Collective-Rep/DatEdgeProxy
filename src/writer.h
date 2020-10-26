#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"


struct WriterSettings {
    std::string  xpub_ip   = "127.0.0.1";
    unsigned int xpub_port = 5555;

    int timeout = 100;

	WriterSettings(){}
	WriterSettings(std::string xpub_ip):
        xpub_ip(xpub_ip)
    {}
};


class Writer : public ofThread {

public:

    Writer();

    void setup(zmq::context_t & ctx, WriterSettings settings);

    void threadedFunction();

private:
    bool isSetup = false;    
    WriterSettings settings;
    shared_ptr<zmq::socket_t> sub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;


};
