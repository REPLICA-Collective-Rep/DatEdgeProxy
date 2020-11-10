#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

class Channel : public ofThread {

    Channel(zmq::context_t & ctx, const std::string & name);

    void setup();

protected:
    using callback_func = std::function<void()>;
    using socket_ptr    = shared_ptr<zmq::socket_t>;

    socket_ptr getPublisher(std::string ip, int port);
    socket_ptr getSubscriber(std::string ip, int port, const std::string & filterq);

    void addCallback(socket_ptr socket_ptr, callback_func callback);

    std::string name;
    int timeout  = 1000;
    bool isSetup = false;

private:
    std::string & recieve(socket_ptr);
    void threadedFunction();


    zmq::context_t & ctx;
    zmq::poller_t<> poller;
    std::map<socket_ptr, callback_func> poll_list;
    std::vector<zmq::poller_event<>>    events;
};