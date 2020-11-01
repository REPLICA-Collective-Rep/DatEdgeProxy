#include "visualiser.h"
#include "parser.h"
#include <regex>


std::array<ofColor, DATE_NUM_CHANNELS> colors {
		ofColor::fromHex(ofHexToInt("47b1b9")),
		ofColor::fromHex(ofHexToInt("499dbb")),
		ofColor::fromHex(ofHexToInt("4a8abd")),
		ofColor::fromHex(ofHexToInt("4c76be")),
		ofColor::fromHex(ofHexToInt("4e62c0")),
		ofColor::fromHex(ofHexToInt("504fc1")),
		ofColor::fromHex(ofHexToInt("6751c3")),
		ofColor::fromHex(ofHexToInt("7e52c5"))
		// ofColor::fromHex(ofHexToInt("453283")),
		// ofColor::fromHex(ofHexToInt("563286")),
		// ofColor::fromHex(ofHexToInt("673289")),
		// ofColor::fromHex(ofHexToInt("7a328c")),
		// ofColor::fromHex(ofHexToInt("8e338f")),
		// ofColor::fromHex(ofHexToInt("913381")),
		// ofColor::fromHex(ofHexToInt("943371")),
		// ofColor::fromHex(ofHexToInt("973360"))
		// ofColor::fromHex(ofHexToInt("34192d")),
		// ofColor::fromHex(ofHexToInt("401d38")),
		// ofColor::fromHex(ofHexToInt("4d2042")),
		// ofColor::fromHex(ofHexToInt("246065")),
		// ofColor::fromHex(ofHexToInt("254f68")),
		// ofColor::fromHex(ofHexToInt("253e6a")),
		// ofColor::fromHex(ofHexToInt("262b6d")),
		// ofColor::fromHex(ofHexToInt("352670"))
};



Visualiser::Visualiser(){
    selected_reading = all_readings.begin();
}


void Visualiser::setup(zmq::context_t & ctx, VisualiserSettings settings){
    if(isThreadRunning()){
        ofLogWarning("Visualiser::setup") << "Thread running!";
        return;
    }
    this->settings = settings;
    isSetup = false;  
    try{
        std::ostringstream pub_addr; pub_addr << "tcp://" << settings.xpub_ip << ":" << ofToString(settings.xpub_port);
        sub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
        sub->connect(pub_addr.str());
        sub->set(zmq::sockopt::subscribe, "");


        poller.add(*sub, zmq::event_flags::pollin);
        events = std::vector<zmq::poller_event<>>(1);

        isSetup = true;   
    }
    catch( const zmq::error_t& e ) {
        ofLogWarning("Simulate::setup") << e.what();
    }
}

void Visualiser::draw(ofFbo & fbo ){

    if( display_lines.size()!= 0 ){
        if(display_lines[0].size() >=2 ){
            float interval = ((display_lines[0].end() - 1)->x - display_lines[0].begin()->x) / display_lines[0].size();
            float wOff     = display_lines[0].begin()->x;       
            
            float width  = ofGetWidth();
            float height = ofGetHeight();

            float hScale  = (height / 2.0) / DATE_NUM_CHANNELS;
            float hOff    = 2.0;


            float wScale   = width / ((float)settings.buffer_size * interval) ;

            lock();
                ofPushMatrix();
                ofScale( glm::vec3(wScale, hScale, 0.0));
                ofTranslate( glm::vec3(-wOff, 1.0, 0.0));

                for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){                
                    ofSetColor(colors[i]);
                    display_lines[i].draw();
                    ofTranslate( glm::vec3(0, hOff , 0.0));
                }
                ofPopMatrix();

                std::ostringstream log;
                log << "Suits: \n";
                for (auto it = all_readings.begin(); it != all_readings.end(); ++it) {
                    if(it == selected_reading)
                        log << "  + Suit " << it->first << " (" << it->second[0].back().x << ")\n";
                    else 
                        log << "  - Suit " << it->first << " (" << it->second[0].back().x << ")\n";
                    
                }

                ofDrawBitmapStringHighlight( log.str(), glm::vec2(ofGetWidth() - 150 , 50));
            unlock();
        }
    }

}

void Visualiser::threadedFunction(){

    if( !isSetup ){
        ofLogError("Visualiser::threadedFunction") << "Not setup!!";
        return;
    }
   

    while( isThreadRunning() ){
        
        const std::chrono::milliseconds _timeout(this->settings.timeout);
        const auto nin = poller.wait_all(events, _timeout);

        if (!nin) {
            ofLogVerbose("Visualiser::threadedFunction") << "Input timeout, try again";
            continue;
        } 

        ofLogVerbose("Visualiser::threadedFunction") << "Got " << nin << " events";
        
        for (unsigned int ind=0; ind<nin; ++ind) {
            zmq::message_t m;
            sub->recv(m);            
            
            assert(m.size() == sizeof(SensorData));

            SensorData  data;
            memcpy(&data, m.data(), sizeof(SensorData));

            lock();       
            auto & readings = all_readings[data.device];
            for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){   
                auto & reading = readings[i];

                reading.push_back(glm::vec3(data.mscounter, data.raw[i], 0));
                if( reading.size() > settings.buffer_size ){
                    reading.erase(readings[i].begin());
                }
            }
            if( all_readings.size() == 1) selected_reading = all_readings.begin();
            if( selected_reading->first == data.device ){
                for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){  
                    display_lines[i].clear(); 

                    glm::vec3 * verts = (glm::vec3*)(&(readings[i])[0]);
                    int size = readings[i].size();

                    display_lines[i].addVertices( verts, size);
                }
            }
            unlock();
        }
    }
}
