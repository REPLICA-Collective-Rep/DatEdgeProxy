#include "proxy.h"


Proxy::Proxy(){
    
}


void Proxy::setup(zmq::context_t & ctx){

    xpub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::xpub);
    xpub->bind(settings.xpub_addr);

    xsub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::xsub);
    xsub->bind(settings.xsub_addr);

    poller.add(*xpub, zmq::event_flags::pollin);
    poller.add(*xsub, zmq::event_flags::pollin);

    events = std::vector<zmq::poller_event<>>(2);

    isSetup = true;
}

void Proxy::threadedFunction(){

    if( !isSetup ){
        ofLogError("Proxy::threadedFunction") << "Not setup!!";
        return;
    }

    while( isThreadRunning() ){

        const std::chrono::milliseconds _timeout(this->settings.timeout);
        const auto nin = poller.wait_all(events, _timeout);

        if (!nin) {
            ofLogVerbose("Proxy::threadedFunction") << "Input timeout, try again";
            continue;
        } 

        ofLogVerbose("Proxy::threadedFunction") << "Got " << nin << " events";
        
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