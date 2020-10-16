#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    receiver.setup(51555);

    Reading reading;
    reading.data = array<float, NUM_CHANNELS>{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

}

//--------------------------------------------------------------
void ofApp::update(){

    std::ostringstream str;
    str << sSuit << " | " << ofGetFrameRate();
    ofSetWindowTitle( str.str() );
    	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage msg;
		receiver.getNextMessage(msg);


		std::regex  re("/p(\\d)/sensor");
        std::smatch m;
        std::string address = msg.getAddress();
        if( std::regex_match(address, m, re) ){
            

            for( int suit : suits){
                int _suit = ofToInt(m[1]);

                if( suit == _suit){



                    Reading reading;
                    reading.data = array<float, NUM_CHANNELS>{
                        msg.getArgAsFloat(0),
                        msg.getArgAsFloat(1),
                        msg.getArgAsFloat(2),
                        msg.getArgAsFloat(3),
                        msg.getArgAsFloat(4),
                        msg.getArgAsFloat(5),
                        msg.getArgAsFloat(6),
                        msg.getArgAsFloat(7)
                    };
                    reading.time  = msg.getArgAsFloat(8);
                    reading.rtime = ofGetSystemTimeMillis();

                    if( srt_times.find(suit) == srt_times.end())
                        srt_times[suit] = reading.time;

                    if( srt_rtimes.find(suit) == srt_rtimes.end())
                        srt_rtimes[suit] = reading.rtime;

                    reading.time  -= srt_times[suit];
                    reading.rtime -= srt_rtimes[suit];

                    data[suit].push_back(reading);

                    if(data[suit].size() >= BUFF_SIZE){
                        data[suit].pop_front();
                    }




                }
            }
        };		
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);


    int suit = sSuit ;



    glm::vec2 pt0, pt1;

    if(data[suit].size() >= 2){
        Reading old_reading = data[suit].front();

        float srt_time  = data[suit].front().time;
        float srt_rtime = data[suit].front().rtime;

        float d_time  = data[suit].back().time  - srt_time ;
        float d_rtime = data[suit].back().rtime - srt_rtime;


        for( auto r_itr = data[suit].begin() + 1;  r_itr < data[suit].end(); r_itr++ ){

            for(int ch = 0; ch < NUM_CHANNELS; ch++){
                float ch_offset = ch * (ofGetHeight() * 1.0 /  NUM_CHANNELS);
                // {
                //     pt0 = glm::vec2(
                //         old_reading.rtime,
                //         old_reading.data[ch]
                //     );

                //     pt1 = glm::vec2(
                //         r_itr->rtime,
                //         r_itr->data[ch]
                //     );

                //     pt0   *= scale;
                //     pt0.y *= 1.0 /  NUM_CHANNELS;
                //     pt0   += rbias;
                //     pt0.y += ch_offset;

                //     pt1 *= scale;
                //     pt1.y *= 1.0 /  NUM_CHANNELS;
                //     pt1 += rbias;
                //     pt1.y += ch_offset;

                //     ofSetColor( ofColor::red);
                //     ofDrawLine(pt0, pt1);
                // }
                {
                    pt0 = glm::vec2(
                        old_reading.time,
                        old_reading.data[ch]
                    );
                    pt1 = glm::vec2(
                        r_itr->time,
                        r_itr->data[ch]
                    );

                    pt0.x   -= srt_time;
                    pt0   *= glm::vec2(ofGetWidth() / d_time , ofGetHeight() / NUM_CHANNELS );                
                    pt0.y += ch_offset;

                    pt1.x   -= srt_rtime;
                    pt1   *= glm::vec2(ofGetWidth() / d_time , ofGetHeight() / NUM_CHANNELS );                
                    pt1.y += ch_offset;

                    ofSetColor( colors[suit][ch]);
                    ofDrawLine(pt0, pt1);
                }
            }

            old_reading = *r_itr;

        };

        for(int ch = 0; ch < NUM_CHANNELS; ch++){
            float ch_offset = ch * (ofGetHeight() * 1.0 /  NUM_CHANNELS);
            ofDrawBitmapString(ch_names[ch], glm::vec2( 40, ch_offset + 40));
        }
    }


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case '1':
            sSuit = 1;
            break;
        case '2':
            sSuit = 2;
            break;
        case '3':
            sSuit = 3;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
