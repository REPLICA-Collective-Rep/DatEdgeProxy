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
    float width  = ofGetWidth();
    float height = ofGetHeight();

    lock();
    
    if(selected_reading != all_readings.end()) {
        int suit = selected_reading->first;
        int numReadings = selected_reading->second.size();

        auto reading_it0 = selected_reading->second.begin() + 1;

        float hScale  = (height / 2.0) / DATE_NUM_CHANNELS;
        float hInc    = height / DATE_NUM_CHANNELS;
        float hOff    = hInc * 0.5;

        float wScale  = width / ((float)settings.buffer_size ) ;

        float t = 0.0;
        for (auto reading_it1 = reading_it0; reading_it1 != selected_reading->second.end(); ++reading_it1) { 
            float t0 = t * wScale;
            float t1 = (t + 1) * wScale;
            for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){
                ofSetColor(colors[i]);
                float x0 = reading_it0->x[i] * hScale; x0 += hOff + (float)i * hInc;
                float x1 = reading_it1->x[i] * hScale; x1 += hOff + (float)i * hInc;  
                
                ofDrawLine( t0, x0, t1, x1);

            }
            reading_it0 = reading_it1;
            t += 1;
        }
    }
    unlock();

    std::ostringstream log;
    log << "Suits: \n";
    for (auto it = all_readings.begin(); it != all_readings.end(); ++it) {
        if(it == selected_reading)
            log << "  + Suit " << it->first << " (" << it->second.back().t << ")\n";
        else 
            log << "  - Suit " << it->first << " (" << it->second.back().t << ")\n";
        
    }

    ofDrawBitmapStringHighlight( log.str(), glm::vec2(ofGetWidth() - 150 , 50));

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
            
            const int numBytes = m.size();
            const char *src = (const char*)m.data();
            
            std::string data(src, numBytes);

            int device   ;
            int mscounter;
            std::vector<float> raw;    

            if ( parseSensorData(data, device, mscounter, raw)) {
                lock();
                
                auto & readings = all_readings[device];
                readings.push_back( Reading(mscounter, raw ) );
                if( readings.size() > settings.buffer_size ){
                    readings.pop_front();
                }

                if(all_readings.size() == 1) selected_reading = all_readings.begin();

                unlock();

            } else {
                ofLogWarning("Visualiser::threadedFunction") << "Could not parse: '" << data << "'";
            }
        }
    }
}
