#include "proxy.h"


Proxy::Proxy(){
    
}


void Proxy::setup(zmq::context_t & ctx, ProxySettings settings){
    if(isThreadRunning()){
        ofLogWarning("Proxy::setup") << "Thread running!";
        return;
    }

    this->settings = settings;
    isSetup = false;  
    try{
        std::ostringstream xpub_addr; xpub_addr << "tcp://" << settings.xpub_ip << ":" << ofToString(settings.xpub_port);
        ofLogNotice("Proxy::setup:xpub_addr") << xpub_addr.str();
        xpub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::xpub);
        xpub->bind(xpub_addr.str());

        std::ostringstream xsub_addr; xsub_addr << "tcp://" << settings.xsub_ip << ":" << ofToString(settings.xsub_port);

        ofLogNotice("Proxy::setup:xsub_addr") << xsub_addr.str();
        xsub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::xsub);
        xsub->bind(xsub_addr.str());

        poller.add(*xpub, zmq::event_flags::pollin);
        poller.add(*xsub, zmq::event_flags::pollin);

        events = std::vector<zmq::poller_event<>>(2);

        isSetup = true;
    }
    catch( const zmq::error_t& e ) {
        ofLogError("Proxy::setup") <<  e.what();
    }
}

void Proxy::threadedFunction(){

    if( !isSetup ){
        ofLogError("Proxy::threadedFunction") << "Not setup!!";
        return;
    }
    ofLogNotice("Proxy::threadedFunction") << "Starting proxy on " << settings.xpub_ip;
    while( isThreadRunning() ){

        const std::chrono::milliseconds _timeout(this->settings.timeout);
        const auto nin = poller.wait_all(events, _timeout);

        if (!nin) {
            ofLogVerbose("Proxy::threadedFunction") << "Input timeout, try again";
            continue;
        } 

        //ofLogVerbose("Proxy::threadedFunction") << "Got " << nin << " events";
        
        for (int ind=0; ind<nin; ++ind) {
            zmq::message_t msg;

            auto socket = events[ind].socket;

            if(socket == *xpub){
                auto res = xpub->recv(msg, zmq::recv_flags::none);
                res = xsub->send(msg, zmq::send_flags::none);
            }

            if(socket == *xsub){
                auto res = xsub->recv(msg, zmq::recv_flags::none);
                res = xpub->send(msg, zmq::send_flags::none);
            }
            

        }
    }
}