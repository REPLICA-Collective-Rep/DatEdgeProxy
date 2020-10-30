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

        std::ostringstream pub_addr; pub_addr << "tcp://" << settings.pub_ip << ":" << ofToString(settings.pub_port);
        pub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
        pub->bind(pub_addr.str());


        poller.add(*sub, zmq::event_flags::pollin);
        poller.add(*pub, zmq::event_flags::pollin);
        events = std::vector<zmq::poller_event<>>(2);

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

            zmq::message_t msg;
            auto socket = events[ind].socket;

            if(socket == *sub){

                sub->recv(msg);    
                std::string str((char *)msg.data(), msg.size());  

                SensorData data;
                if ( parseSensorData(str, data)) {
                    msg = zmq::message_t( (void *)&data, sizeof(data) );
                    pub->send(msg);
                } else {
                    ofLogWarning("Dataserver::threadedFunction") << "Could not parse: '" << msg << "'";
                }
            }

            if(socket == *pub){

            }



        }
    }
}