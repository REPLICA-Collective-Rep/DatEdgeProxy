#include "channel.h"

Channel::Channel(zmq::context_t & ctx, const std::string & name):
    ctx(ctx), name(name)    
{

};

void Channel::setup(){

};

void Channel::threadedFunction(){
    if( !isSetup ){
        ofLogError("Channel::threadedFunction") << name << " not setup!!";
        return;
    }

    while( isThreadRunning() ){
        
        const std::chrono::milliseconds t(timeout);
        const auto nin = poller.wait_all(events, t);

        if (!nin) {
            ofLogWarning("Channel::threadedFunction") << name << " input timeout, try again";
            continue;
        }
        ofLogVerbose("Channel::threadedFunction") << name << " got " << nin << " events";        
        for (unsigned int ind=0; ind<nin; ++ind) {
            auto socket = events[ind].socket;

            for( auto const & poll_item : poll_list){
                if( socket == *(poll_item.first)){
                    (poll_item.second)();
                    continue;
                }
            }
            ofLogWarning("Channel::threadedFunction") << name << " input timeout, try again";
        }
    }
};

shared_ptr<zmq::socket_t> Channel::getPublisher(std::string ip, int port){
    std::ostringstream addr; addr << "tcp://" << ip << ":" << ofToString(port);
    auto socket = make_shared<zmq::socket_t>(ctx, zmq::socket_type::pub);
    socket->bind(addr.str());
    return socket;

};

shared_ptr<zmq::socket_t> Channel::getSubscriber(std::string ip, int port, const std::string & filter){
    std::ostringstream addr; addr << "tcp://" << ip << ":" << ofToString(port);
    auto socket = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
    socket->connect(addr.str());
    socket->set(zmq::sockopt::subscribe, filter);
    return socket;
};

void Channel::addCallback(socket_ptr socket_ptr, callback_func callback){
    poll_list[socket_ptr] = callback;
};
