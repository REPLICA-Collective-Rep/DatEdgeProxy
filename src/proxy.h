#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"


struct ProxySettings {
    std::string  xpub_ip   = "0.0.0.0";
    unsigned int xpub_port = 5555;

    std::string xsub_ip    = "0.0.0.0";
    unsigned int xsub_port = 5556;

    unsigned int timeout   = 100;

    ProxySettings(){}
    ProxySettings(std::string xpub_ip):
        xpub_ip(xpub_ip)
    {}
};

class Proxy : public ofThread {

public:

    Proxy();

    void setup(zmq::context_t & ctx, ProxySettings settings);

    void threadedFunction();

private:
    bool isSetup = false;    
    ProxySettings settings;

    shared_ptr<zmq::socket_t> xpub;
    shared_ptr<zmq::socket_t> xsub;
    std::vector<zmq::poller_event<>> events;

    zmq::poller_t<> poller;

};
