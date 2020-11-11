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
        sub->set(zmq::sockopt::subscribe, "sensor");

        std::ostringstream pub_addr; pub_addr << "tcp://" << settings.pub_ip << ":" << ofToString(settings.pub_port);
        pub_core = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
        pub_core->bind(pub_addr.str());

        std::ostringstream sub_addr; sub_addr << "tcp://" << settings.sub_ip << ":" << ofToString(settings.sub_port);
        sub_core = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
        sub_core->connect(sub_addr.str());
        sub_core->set(zmq::sockopt::subscribe, "");


        poller.add(*sub, zmq::event_flags::pollin);
        poller.add(*pub_core, zmq::event_flags::pollin);
        poller.add(*sub_core, zmq::event_flags::pollin);

        events = std::vector<zmq::poller_event<>>(3);

        isSetup = true;   
    }
    catch( const zmq::error_t& e ) {
        ofLogWarning("Dataserver::setup") << e.what();
    }    


    sender.setup(settings.osc_ip, settings.osc_port);
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

        //ofLogVerbose("Dataserver::threadedFunction") << "Got " << nin << " events";
        
        for (unsigned int ind=0; ind<nin; ++ind) {

            zmq::message_t msg;
            auto socket = events[ind].socket;

            if(socket == *sub){
                sub->recv(msg);    
                std::string str =  msg.to_string();  

                SensorData data;
                if ( parseSensorData(str, data)) {

                    msg = zmq::message_t( (void *)&data, sizeof(data) );
                    pub_core->send(msg);

                    ofxOscMessage m;
                    m.setAddress("/sensor/" + ofToString(data.device));
                    for( int i = 0; i < DATE_NUM_CHANNELS; i++){
                        m.addFloatArg(data.raw[i]);
                    }
                    sender.sendMessage(m, true);
                } else {
                    ofLogWarning("Dataserver::threadedFunction") << "Could not parse: '" << msg << "'";
                }
            }

            if(socket == *pub_core){
                pub_core->recv(msg);
                ofLogWarning("Dataserver::threadedFunction") << "Message on pub: '" << msg.to_string() << "'";
            }

            if(socket == *sub_core){
                sub_core->recv(msg);       

                if( msg.size() == sizeof(OutputData)){
                    OutputData data;
                    memcpy(&data, msg.data(), sizeof(OutputData));
                    // Send OSC
                    ofxOscMessage msg_data, msg_loss;

                    msg_data.setAddress("/ml/data/" + ofToString(data.device));
                    for( int i = 0; i < Z_DIM; i++){
                         msg_data.addFloatArg(data.embedding[i]);
                    }

                    msg_loss.setAddress("/ml/loss/" + ofToString(data.device));
                    msg_loss.addFloatArg(data.loss);

                    sender.sendMessage( msg_data, true);
                    sender.sendMessage( msg_loss, true);
                } else {
                    ofLogWarning("Dataserver::threadedFunction") << "Output data wrong size " << msg.size() <<  "/" << sizeof(OutputData);
                };


            }
        }
    }
}