#include "writer.h"


Writer::Writer(){
    
}


void Writer::setup(zmq::context_t & ctx, WriterSettings settings){
    if(isThreadRunning()){
        ofLogWarning("Writer::setup") << "Thread running!";
        return;
    }

    this->settings = settings;
    isSetup = false;  
    try{
        std::ostringstream xpub_addr; xpub_addr << "tcp://" << settings.xpub_ip << ":" << ofToString(settings.xpub_port);
        sub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
        sub->connect(xpub_addr.str());
        sub->set(zmq::sockopt::subscribe, "");

        poller.add(*sub, zmq::event_flags::pollin);
        events = std::vector<zmq::poller_event<>>(1);

        isSetup = true;   
    }
    catch( const zmq::error_t& e ) {
        ofLogWarning("Writer::setup") << e.what();
    }
}

void Writer::threadedFunction(){

    if( !isSetup ){
        ofLogError("Writer::threadedFunction") << "Not setup!!";
        return;
    }

    while( isThreadRunning() ){
        
        const std::chrono::milliseconds _timeout(this->settings.timeout);
        const auto nin = poller.wait_all(events, _timeout);

        if (!nin) {
            ofLogVerbose("Writer::threadedFunction") << "Input timeout, try again";
            continue;
        } 

        ofLogVerbose("Writer::threadedFunction") << "Got " << nin << " events";
        
        for (int ind=0; ind<nin; ++ind) {
            ofBuffer data;
            data.clear();

            zmq::message_t m;
            sub->recv(&m);            
            
            const int numBytes = m.size();
            const char *src = (const char*)m.data();
            
            data.set(src, numBytes);
        
            ofLogNotice("Writer::threadedFunction") << data;
        }
    }

}