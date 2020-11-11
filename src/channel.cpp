#include "channel.h"

Channel::Channel(zmq::context_t & ctx, const std::string & name):
    ctx(ctx), name(name)    
{

};


void Channel::setup(){
    onSetup();
    events = std::vector<zmq::poller_event<>>(1);
    isSetup = true;
};

void Channel::start(){
    onStart();
    startThread();
};

void Channel::stop(){
    onStop();
    waitForThread(true);
};

void Channel::threadedFunction(){
    if( !isSetup ){
        ofLogError("Channel::threadedFunction") << name << " not setup!!";
        return;
    }

    while( isThreadRunning() ){
        
        const std::chrono::milliseconds t(timeout);
        auto nin = poller.wait_all(events, t);

        if (!nin) {
            ofLogWarning("Channel::threadedFunction") << name << " input timeout, try again";
            continue;
        }

       
        for (unsigned int ind=0; ind<nin; ++ind) {
            auto socket = events[ind].socket;

            for( auto const & poll_item : poll_list){
                if( socket == *(poll_item.first)){
                    (poll_item.second)();
                    continue;
                }
            }
        }
    }
    ofLogError("Channel::threadedFunction") << name << " exiting channel!!";

};

shared_ptr<zmq::socket_t> Channel::getPublisher(std::string ip, int port){
    socket_ptr socket;
    try{
        std::ostringstream addr; addr << "tcp://" << ip << ":" << ofToString(port);
        socket = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
        socket->bind(addr.str());
    }
    catch( const zmq::error_t& e ) {
        ofLogWarning("Channel::getPublisher") << name << "\n" <<  e.what();
    }
    return socket;
};

shared_ptr<zmq::socket_t> Channel::getSubscriber(std::string ip, int port, const std::string & filter){
    socket_ptr socket;
    try{
        std::ostringstream addr; addr << "tcp://" << ip << ":" << ofToString(port);
        socket = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
        socket->connect(addr.str());
        socket->set(zmq::sockopt::subscribe, filter);
    }
    catch( const zmq::error_t& e ) {
        ofLogWarning("Channel::getPublisher") << name << "\n" <<  e.what();
    }
    return socket;
};

void Channel::addCallback(socket_ptr socket_ptr, callback_func callback){
    poller.add(*socket_ptr, zmq::event_flags::pollin );
    events.push_back(zmq::poller_event<>());
    poll_list[socket_ptr] = callback;
};
