#include "dataserver.h"
#include "parser.h"
#include "cnpy.h"

#include "constants.h"

Dataserver::Dataserver(){

}

Dataserver::~Dataserver(){
}


void Dataserver::setup(zmq::context_t & ctx, DataserverSettings settings){
    if(isThreadRunning()){
        ofLogWarning("Dataserver::setup") << "Thread running!";
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
        ofLogWarning("Dataserver::setup") << e.what();
    }    
}

void Dataserver::threadedFunction(){
    
    if( !isSetup ){
        ofLogError("Dataserver::threadedFunction") << "Not setup!!";
        return;
    }

    while( isThreadRunning() ){
        
        const std::chrono::milliseconds _timeout(this->settings.timeout);
        const auto nin = poller.wait_all(events, _timeout);

        if (!nin) {
            ofLogVerbose("Dataserver::threadedFunction") << "Input timeout, try again";
            continue;
        } 

        ofLogVerbose("Dataserver::threadedFunction") << "Got " << nin << " events";
        
        for (unsigned int ind=0; ind<nin; ++ind) {
            SensorData data;

            zmq::message_t m;
            sub->recv(m);          

            std::string str((char *)m.data(), m.size());  

            if ( parseSensorData(str, data)) {

            } else {
                ofLogWarning("Dataserver::threadedFunction") << "Could not parse: '" << m << "'";
            }
        }
    }
}