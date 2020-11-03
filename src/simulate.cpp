#include "simulate.h"


Simulate::Simulate(){
    
}


void Simulate::setup(zmq::context_t & ctx, SimulateSettings settings){
    if(isThreadRunning()){
        ofLogWarning("Simulate::setup") << "Thread running!";
        return;
    }

    this->settings = settings;
    isSetup = false;  
    try{
        std::ostringstream xsub_addr; xsub_addr << "tcp://" << settings.xsub_ip << ":" << ofToString(settings.xsub_port);
        ofLogNotice("Simulate::setup:xsub_addr") << xsub_addr.str();
        pub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
        pub->connect(xsub_addr.str());

        isSetup = true;   
    }
    catch( const zmq::error_t& e ) {
        ofLogError("Simulate::setup") <<  e.what();
    }
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
            _msg << "p" << settings.sensors[i] << " ";
            switch(2){
                case 0:
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    _msg << ofToString( ofRandomf(), 4, 5, 0) << " ";
                    break;
                case 1:
                    _msg << ofToString( sin(ofGetSystemTimeMillis() * 0.001 + 1 ), 4, 5, 0) << " ";
                    _msg << ofToString( cos(ofGetSystemTimeMillis() * 0.001 + 2 ), 4, 5, 0) << " ";
                    _msg << ofToString( sin(ofGetSystemTimeMillis() * 0.001 + 3 ), 4, 5, 0) << " ";
                    _msg << ofToString( cos(ofGetSystemTimeMillis() * 0.001 + 4 ), 4, 5, 0) << " ";
                    _msg << ofToString( sin(ofGetSystemTimeMillis() * 0.001 + 5 ), 4, 5, 0) << " ";
                    _msg << ofToString( cos(ofGetSystemTimeMillis() * 0.001 + 6 ), 4, 5, 0) << " ";
                    _msg << ofToString( sin(ofGetSystemTimeMillis() * 0.001 + 7 ), 4, 5, 0) << " ";
                    _msg << ofToString( cos(ofGetSystemTimeMillis() * 0.001 + 8 ), 4, 5, 0) << " ";
                    break; 
                case 2:
                    _msg << ofToString( 0.1, 4, 5, 0) << " ";
                    _msg << ofToString( 0.2, 4, 5, 0) << " ";
                    _msg << ofToString( 0.3, 4, 5, 0) << " ";
                    _msg << ofToString( 0.4, 4, 5, 0) << " ";
                    _msg << ofToString( 0.5, 4, 5, 0) << " ";
                    _msg << ofToString( 0.6, 4, 5, 0) << " ";
                    _msg << ofToString( 0.7, 4, 5, 0) << " ";
                    _msg << ofToString( 0.8, 4, 5, 0) << " ";
                    break;
            }          

            _msg << (float)counter;
            _msg <<  ";\n";

            std::string str_msg = _msg.str();

            zmq::message_t msg(str_msg.c_str(), str_msg.size());
            auto res = pub->send(msg, zmq::send_flags::none);
            
        }
        sleep(settings.interval);

        counter += settings.interval;
    }
}