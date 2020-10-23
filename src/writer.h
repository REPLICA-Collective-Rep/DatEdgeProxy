#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"


struct WriterSettings {
    const std::string xpub_addr = "tcp://127.0.0.1:5555";
    int timeout= 100;

};

class Writer : public ofThread {

public:

    Writer();

    void setup(zmq::context_t & ctx);

    void threadedFunction();

private:
    bool isSetup = false;    
    WriterSettings settings;
    shared_ptr<zmq::socket_t> sub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;


};
