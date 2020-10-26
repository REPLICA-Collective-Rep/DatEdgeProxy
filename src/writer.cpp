#include "writer.h"
#include "parser.h"
#include "cnpy.h"

#include "constants.h"

Writer::Writer(){

}

Writer::~Writer(){
    if(current_dir.exists() && ofDirectory::isDirectoryEmpty(current_dir.path())){
        current_dir.remove(true);
    }

    for( auto & buffer : buffers){
        cnpy::npy_save(buffer.second.path,&(buffer.second.data)[0],{ settings.buffer_size, DATE_NUM_CHANNELS + 1}, "a");
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
    current_dir = ofDirectory(ofToDataPath(session));
    if(!current_dir.exists()){
        current_dir.create();
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
            
            const int numBytes = m.size();
            const char *src = (const char*)m.data();
            
            std::string data(src, numBytes);

            int device   ;
            int mscounter;
            std::vector<float> raw;    

            if ( parseSensorData(data, device, mscounter, raw)) {
                
                auto buffer = buffers.find(device);
                if(  buffer == buffers.end()){ 
                    std::string path = ofToString(device, 2, 2, '0') + "-" + ofToString(subsession, 2, 2, '0') + ".npz";

                    buffers[device] = bufferInfo( ofFilePath::join( current_dir, path));
                            
                    buffer = buffers.find(device);
                }

                std::array<float,DATE_NUM_CHANNELS + 1 > row;

                row[0] = mscounter;
                for(int i = 1; i < DATE_NUM_CHANNELS + 1; i++ ){
                    row[i] = data[i - 1];
                }

                buffer->second.data.push_back( std::move(row));

                if(buffer->second.data.size() == settings.buffer_size ){
                    cnpy::npy_save(buffer->second.path,&(buffer->second.data)[0],{ settings.buffer_size, DATE_NUM_CHANNELS + 1}, "a");
                    buffer->second.data.clear();
                }

        
                
            } else {
                ofLogWarning("Writer::threadedFunction") << "Could not parse: '" << data << "'";
            }
        }
    }

    for( auto & buffer : buffers){
        cnpy::npy_save(buffer.second.path,&(buffer.second.data)[0],{ settings.buffer_size, DATE_NUM_CHANNELS + 1}, "a");
    }
    buffers.clear();

    subsession += 1;
}