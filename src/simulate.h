#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"


struct SimulateSettings {
    const std::string xsub_addr = "tcp://127.0.0.1:5556";
    int interval = 1000;
    vector<int> sensors {
        1,2,3,4,5,6,7
    };
};

class Simulate : public ofThread {

public:

    Simulate();

    void setup(zmq::context_t & ctx);

    void threadedFunction();

private:
    bool isSetup = false;    
    SimulateSettings settings;

    shared_ptr<zmq::socket_t> pub;

};
