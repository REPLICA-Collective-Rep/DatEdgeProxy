#include "writer.h"
#include "parser.h"

Writer::Writer(){
    subsession = 0;
}

Writer::~Writer(){
    if(current_dir.exists() && ofDirectory::isDirectoryEmpty(current_dir.path())){
        current_dir.remove(true);
    }

    for( auto & buffer : buffers){
        cnpy::npy_save(buffer.second.path,&((buffer.second.data)[0]),{ buffer.second.data.size(), DATE_NUM_CHANNELS + 1}, "w");
    }
    buffers.clear();
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

    session = ofGetTimestampString();

    if(settings.data_root == "")
        current_dir = ofDirectory(ofToDataPath(session));
    else
        current_dir = ofDirectory(ofFilePath::join(ofFilePath::getAbsolutePath(settings.data_root), session));



    if(!current_dir.exists()) {
        if(!current_dir.create(true))
            ofLogError("Writer::setup") << "Could not create data folder. " << current_dir.path();
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
        
        for (unsigned int ind=0; ind<nin; ++ind) {
            zmq::message_t m;
            sub->recv(m);            

            SensorData  data;
            memcpy(&data, m.data(), sizeof(SensorData));

            if( m.size() == sizeof(SensorData)){
                    
                auto buffer = buffers.find(data.device);
                if(  buffer == buffers.end()){ 
                    std::string path = ofToString(data.device, 2, 2, '0') + "-" + ofToString(subsession, 2, 2, '0') + ".npy";

                    buffers[data.device] = bufferInfo( ofFilePath::join( current_dir, path));
                            
                    buffer = buffers.find(data.device);
                }

                buffer->second.data.push_back( data.mscounter );
                for(int i = 1; i < DATE_NUM_CHANNELS + 1; i++ ){
                        buffer->second.data.push_back( data.raw[i - 1]);
                }

                if(buffer->second.data.size() >= settings.buffer_size ){

                    cnpy::npy_save(buffer->second.path, &((buffer->second.data)[0]), { buffer->second.data.size(), DATE_NUM_CHANNELS + 1}, "w");
                    buffer->second.data.clear();
                }
            } else {
                ofLogWarning("Writer::threadedFunction") << m.to_string();
            }
  
        }
    }

    for( auto & buffer : buffers){
        cnpy::npy_save(buffer.second.path,&((buffer.second.data)[0]),{ buffer.second.data.size(), DATE_NUM_CHANNELS + 1}, "w");
    }
    buffers.clear();

    subsession += 1;
}