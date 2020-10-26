#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

struct SimulateSettings {
    std::string xsub_ip    = "127.0.0.1";    
    unsigned int xsub_port = 5556;

    unsigned int interval   = 20;
    vector<int> sensors {
        1,2,3,4,5,6,7
    };

    SimulateSettings(){}
    SimulateSettings(std::string xsub_ip):
        xsub_ip(xsub_ip)
    {}
};

class Simulate : public ofThread {

public:

    Simulate();

    void setup(zmq::context_t & ctx, SimulateSettings settings);

    void threadedFunction();

private:
    bool isSetup = false;    
    SimulateSettings settings;

    shared_ptr<zmq::socket_t> pub;

};
