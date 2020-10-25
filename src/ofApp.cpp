#include "ofApp.h"
#include "zmq.hpp"

zmq::context_t ctx;

//--------------------------------------------------------------
void ofApp::setup(){

    //ofSetLogLevel(OF_LOG_VERBOSE);

    simulate.setup(ctx);
    //simulate.startThread();

    writer.setup(ctx);
    writer.startThread();

    visualiser.setup(ctx);
    visualiser.startThread();

    proxy.setup(ctx);
    proxy.startThread();

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    visualiser.draw(graphFbo);



    std::ostringstream log;
    log << "simulate (s): " << (simulate.isThreadRunning()   ? "ON" : "OFF") << "\n";
    log << "writer (w): "   << (writer.isThreadRunning()     ? "ON" : "OFF") << "\n";
    log << "proxy (p): "    << (proxy.isThreadRunning()      ? "ON" : "OFF") << "\n";
    log << "visualiser: "   << (visualiser.isThreadRunning() ? "ON" : "OFF") ;
    ofDrawBitmapStringHighlight( log.str(), glm::vec2(50, 50));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
    case 's':
        if(!simulate.isThreadRunning()){
            simulate.startThread();
        } else {
            simulate.stopThread();
        }
        break;
    case 'w':
        if(!writer.isThreadRunning()){
            writer.startThread();
        } else {
            writer.stopThread();
        }
      break;
        break;
    case 'p':
        if(!proxy.isThreadRunning()){
            proxy.startThread();
        } else {
            proxy.stopThread();
        }
      break;
    default:
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
