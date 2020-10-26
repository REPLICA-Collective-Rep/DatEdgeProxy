#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "constants.h"

struct WriterSettings {
    std::string  xpub_ip   = "127.0.0.1";
    unsigned int xpub_port = 5555;


    int timeout    = 100;
    unsigned int buffer_size = 100;

	WriterSettings(){}
	WriterSettings(std::string xpub_ip):
        xpub_ip(xpub_ip)
    {}
};


class Writer : public ofThread {

public:

    Writer();
    ~Writer();

    void setup(zmq::context_t & ctx, WriterSettings settings);

    void threadedFunction();

private:

    struct bufferInfo {
        std::string path;
        std::vector<std::array<float, DATE_NUM_CHANNELS + 1 >> data;
        
        bufferInfo(std::string path): path(path){}
        bufferInfo() {}
    };

    bool isSetup = false;    
    std::string session;
    int subsession;
    ofDirectory current_dir;
    WriterSettings settings;
    shared_ptr<zmq::socket_t> sub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;

    std::map<int, bufferInfo> buffers;

};
