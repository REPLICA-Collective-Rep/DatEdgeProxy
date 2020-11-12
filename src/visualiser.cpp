#include "visualiser.h"
#include <regex>

using namespace std::placeholders;

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



Visualiser::Visualiser(zmq::context_t & ctx, const std::string & name) :
    Channel(ctx, name)
{
}


void Visualiser::onSetup(){
    raw_sub = getSubscriber(settings.raw_pub_ip, settings.raw_pub_port, "");   
    addCallback(raw_sub, std::bind(&Visualiser::receiveSensorData, this) );

    ml_sub = getSubscriber(settings.ml_pub_ip, settings.ml_pub_port, "" );
    addCallback(ml_sub, std::bind(&Visualiser::receiveMLData, this));

}

void Visualiser::onStart(){
    // for( int i ; i < display_lines.size(); i++ )
    //     display_lines[i].clear();

    // all_readings.clear();
    // selected_reading = all_readings.begin();
}

void Visualiser::draw(ofFbo & fbo ){

    for( const auto & output : output_data ){
        if( newMLData[output.first] ){
            lock();  
            outputTextures[output.first].loadData(&(output.second[0]), Z_DIM, output.second.size() / Z_DIM, GL_LUMINANCE);
            newMLData[output.first] = false;
            unlock();
        }
        outputTextures[output.first].draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    for( const auto & data : raw_data ){
        if( newRawData[data.first] ){
            lock();  
            for(int i = 0; i < DATE_NUM_CHANNELS; i++)
                rawVbos[data.first][i].setVertexData((glm::vec3 *)&(data.second[0]), data.second.size(), GL_DYNAMIC_DRAW);
            
            newRawData[data.first] = false;
            unlock();
        }

        for(int i = 0; i < DATE_NUM_CHANNELS; i++)
            rawVbos[data.first][i].draw(GL_LINE_STRIP, 0, data.second.size());

    }

    // if( display_lines.size()!= 0 ){
    //     if(display_lines[0].size() >=2 ){
    //         float interval = ((display_lines[0].end() - 1)->x - display_lines[0].begin()->x) / display_lines[0].size();
    //         float wOff     = display_lines[0].begin()->x;       
            
    //         float width  = ofGetWidth();
    //         float height = ofGetHeight();

    //         float hScale  = (height / 2.0) / DATE_NUM_CHANNELS;
    //         float hOff    = 2.0;


    //         float wScale   = width / ((float)settings.buffer_size * interval) ;

    //         lock();
    //             ofPushMatrix();
    //             ofScale( glm::vec3(wScale, hScale, 0.0));
    //             ofTranslate( glm::vec3(-wOff, 1.0, 0.0));

    //             for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){                
    //                 ofSetColor(colors[i]);
    //                 display_lines[i].draw();
    //                 ofTranslate( glm::vec3(0, hOff , 0.0));
    //             }
    //             ofPopMatrix();

    //             std::ostringstream log;
    //             log << "Suits: \n";
    //             for (auto it = all_readings.begin(); it != all_readings.end(); ++it) {
    //                 if(it == selected_reading)
    //                     log << "  + Suit " << it->first << " (" << it->second[0].back().x << ")\n";
    //                 else 
    //                     log << "  - Suit " << it->first << " (" << it->second[0].back().x << ")\n";
                    
    //             }

    //             ofDrawBitmapStringHighlight( log.str(), glm::vec2(ofGetWidth() - 150 , 50));
    //         unlock();
    //     }
    // }

}

void Visualiser::receiveSensorData(){
    zmq::message_t msg;
    raw_sub->recv(msg);             

    auto size = sizeof(SensorData);

    if(msg.size() == size){

        SensorData  data;
        memcpy(&data, msg.data(), msg.size());

        lock();       
        auto & readings = raw_data[data.device];
        for( int i = 0; i < DATE_NUM_CHANNELS; i++ ){   
            auto & reading = readings[i];

            glm::vec3 point(data.mscounter, data.raw[i], i);

            if(reading.size() != 0 && 
                (reading.back().x >= point.x || point.x >= reading.back().x + 100 * DATE_TARGET_INTERVAL_MS ))
            {
                ofLogVerbose("Visualiser::threadedFunction") << "MS counter out of sequence,!!";
            }

            reading.push_back(point);
            if( reading.size() > settings.buffer_size ){
                reading.erase(readings[i].begin());
            }

            newRawData[data.device] = true;
        }
        unlock();
    } else {
        ofLogWarning("Visualiser::receiveMLData") << "Data wrong size";
    };

}


void Visualiser::receiveMLData(){
    zmq::message_t msg;
    ml_sub->recv(msg);            

    if(msg.size() == sizeof(OutputData)){
        OutputData data;
        memcpy(&data, msg.data(), msg.size());

        std::vector<float> & output = output_data[data.device];
        output.reserve(output.size() + Z_DIM);
        copy(&data.embedding[0], &data.embedding[Z_DIM], back_inserter(output));

        if(output.size() > settings.buffer_size * Z_DIM){
            output.erase(output.begin(), output.begin() + Z_DIM);
        }

        std::vector<float> & loss = losses[data.device];
        loss.push_back(data.loss);
        if(losses.size() > settings.buffer_size){
            losses.erase(losses.begin());
        }

        newMLData[data.device] = true;

    } else {
        ofLogVerbose("Visualiser::receiveSensorData" ) << "Data wrong size";
    }

}

