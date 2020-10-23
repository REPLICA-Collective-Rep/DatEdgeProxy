#include "simulate.h"


Simulate::Simulate(){
    
}


void Simulate::setup(zmq::context_t & ctx){

    pub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
    pub->connect(settings.xsub_addr);

    isSetup = true;   
}

void Simulate::threadedFunction(){

    if( !isSetup ){
        ofLogError("Simulate::threadedFunction") << "Not setup!!";
        return;
    }

    while( isThreadRunning() ){

        for (size_t i = 0; i < settings.sensors.size(); i++ ) {
            zmq::message_t msg("hello world!", 12);
            auto res = pub->send(msg, zmq::send_flags::none);
            
        }
        ofLogNotice("Simulate::threadedFunction") << "Sending... ";
        sleep(settings.interval);
    }
}