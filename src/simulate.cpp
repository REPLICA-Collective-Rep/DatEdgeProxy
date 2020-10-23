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

    int counter = 0;
    while( isThreadRunning() ){

        for (size_t i = 0; i < settings.sensors.size(); i++ ) {


            std::ostringstream _msg;
            _msg << "sensors" << " ";      
            _msg << "p" << i << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
            _msg << counter;
            _msg <<  ";\n";

            std::string str_msg = _msg.str();

            zmq::message_t msg(str_msg.c_str(), str_msg.size());
            auto res = pub->send(msg, zmq::send_flags::none);
            
        }
        ofLogNotice("Simulate::threadedFunction") << "Sending... ";
        sleep(settings.interval);

        counter += settings.interval;
    }
}