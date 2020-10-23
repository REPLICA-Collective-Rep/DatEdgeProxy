#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"


struct ProxySettings {
    const std::string xpub_addr = "tcp://0.0.0.0:5555";
    const std::string xsub_addr = "tcp://0.0.0.0:5556";
    int timeout = 100;
};

class Proxy : public ofThread {

public:

    Proxy();

    void setup(zmq::context_t & ctx);

    void threadedFunction();

private:
    bool isSetup = false;    
    ProxySettings settings;

    shared_ptr<zmq::socket_t> xpub;
    shared_ptr<zmq::socket_t> xsub;
    std::vector<zmq::poller_event<>> events;

    zmq::poller_t<> poller;

};
