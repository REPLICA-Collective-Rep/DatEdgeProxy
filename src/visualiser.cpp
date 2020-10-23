#include "visualiser.h"
#include "config.h"
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
    
}


void Visualiser::setup(zmq::context_t & ctx){

    sub = make_shared<zmq::socket_t>(ctx, zmq::socket_type::sub);
    sub->connect(settings.xpub_addr);
    sub->set(zmq::sockopt::subscribe, "");


    poller.add(*sub, zmq::event_flags::pollin);
    events = std::vector<zmq::poller_event<>>(1);

    isSetup = true;   
}

void Visualiser::draw(ofFbo & fbo ){
    lock();
    float width  = ofGetWidth();
    float height = ofGetHeight();


    for (auto it = all_readings.begin(); it != all_readings.end(); ++it) {
        int suit = it->first;
        int numReadings = it->second.size();

        auto reading_it0 = it->second.begin() + 1;

        float hScale  = (height / 2.0) / DATE_NUM_CHANNELS;
        float hInc    = height / DATE_NUM_CHANNELS;
        float hOff    = hInc * 0.5;

        float wScale  = width / ((float)BUFFER_SIZE ) ;

        float t = 0.0;
        for (auto reading_it1 = reading_it0; reading_it1 != it->second.end(); ++reading_it1) { 
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

        break;
    }
    unlock();

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
        
        for (int ind=0; ind<nin; ++ind) {
            ofBuffer data; /// Get rid of this
            data.clear();

            zmq::message_t m;
            sub->recv(&m);            
            
            const int numBytes = m.size();
            const char *src = (const char*)m.data();
            
            data.set(src, numBytes);
        

            //std::regex re("^sensors p(\\d) ((?:(?:[-+]?[0-9]*\\.?[0-9]+) ){8})(\\d+);.*$");
            std::string pttrn1 = "^sensors p(\\d) ((?:(?:[-+]?[0-9]*\\.?[0-9]+) ){8})(\\d+);(?:[\r\n]+)*$";
            std::string pttrn2 = "([-+]?[0-9]*\\.?[0-9]+)";
            std::regex re1(pttrn1);
            std::regex re2(pttrn2);
            std::smatch match;

            std::string text(data);
            if (std::regex_match(text, match, re1) && match.size() > 1) {

                int device    = ofToInt(match.str(1));
                int mscounter = ofToInt( match.str(3));

                std::smatch fmatch;
                std::string fstring = match.str(2);

                std::vector<float> raw;
                raw.reserve(DATE_NUM_CHANNELS);

                int i = 0;
                while(std::regex_search(fstring, fmatch, re2)) {
                    if(i >= DATE_NUM_CHANNELS)
                        break;
                    
                    raw.push_back( ofToFloat( fmatch[1]));
                    fstring = fmatch.suffix();
                }     
                
                if(raw.size() != DATE_NUM_CHANNELS ){
                    ofLogWarning("Visualiser::threadedFunction") << "Wrong number of channels, " << raw.size();
                    continue;
                }

                lock();
                auto & readings = all_readings[device];
                readings.push_back( Reading(mscounter, raw ) );
                if( readings.size() > BUFFER_SIZE ){
                    readings.pop_front();
                }


                unlock();

            } else {
                ofLogWarning("Visualiser::threadedFunction") << "Could not parse: '" << text << "'";
            }

        }
    }

}
    
    // while(receiver.hasWaitingMessages()){

	// 	// get the next message
	// 	ofxOscMessage msg;
	// 	receiver.getNextMessage(msg);


	// 	std::regex  re("/p(\\d)/sensor");
    //     std::smatch m;
    //     std::string address = msg.getAddress();
    //     if( std::regex_match(address, m, re) ){
            

    //         for( int suit : suits){
    //             int _suit = ofToInt(m[1]);

    //             if( suit == _suit){



    //                 Reading reading;
    //                 reading.data = array<float, NUM_CHANNELS>{
    //                     msg.getArgAsFloat(0),
    //                     msg.getArgAsFloat(1),
    //                     msg.getArgAsFloat(2),
    //                     msg.getArgAsFloat(3),
    //                     msg.getArgAsFloat(4),
    //                     msg.getArgAsFloat(5),
    //                     msg.getArgAsFloat(6),
    //                     msg.getArgAsFloat(7)
    //                 };
    //                 reading.time  = msg.getArgAsFloat(8);
    //                 reading.rtime = ofGetSystemTimeMillis();

    //                 if( srt_times.find(suit) == srt_times.end())
    //                     srt_times[suit] = reading.time;

    //                 if( srt_rtimes.find(suit) == srt_rtimes.end())
    //                     srt_rtimes[suit] = reading.rtime;

    //                 reading.time  -= srt_times[suit];
    //                 reading.rtime -= srt_rtimes[suit];

    //                 data[suit].push_back(reading);

    //                 if(data[suit].size() >= BUFF_SIZE){
    //                     data[suit].pop_front();
    //                 }




    //             }
    //         }
    //     };		
    // }