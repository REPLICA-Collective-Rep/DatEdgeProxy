#include "ofApp.h"
#include "zmq.hpp"

zmq::context_t ctx;

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetLogLevel(OF_LOG_VERBOSE);

    SimulateSettings   simulateSettings("127.0.0.1");
    WriterSettings     writerSettings("127.0.0.1");
    VisualiserSettings visualiserSettings("127.0.0.1");
    DataserverSettings dataserverSettings("0.0.0.0");
    ProxySettings      proxySettings("0.0.0.0", "0.0.0.0");



    dataserver.setup(ctx, dataserverSettings);
    dataserver.startThread();

    writer.setup(ctx, writerSettings);
    writer.startThread();

    visualiser.setup(ctx, visualiserSettings);
    visualiser.startThread();

    proxy.setup(ctx, proxySettings);
    proxy.startThread();

    simulate.setup(ctx, simulateSettings);
    simulate.startThread();

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    visualiser.draw(graphFbo);

    std::ostringstream log;
    log << "Framerate: "    << ofGetFrameRate() << "\n";
    log << "\n";
    log << "simulate   (s): "  << (simulate.isThreadRunning()   ? "ON" : "OFF") << "\n";
    log << "writer     (w): "  << (writer.isThreadRunning()     ? "ON" : "OFF") << "\n";
    log << "proxy      (p): "  << (proxy.isThreadRunning()      ? "ON" : "OFF") << "\n";
    log << "visualiser (v): "  << (visualiser.isThreadRunning() ? "ON" : "OFF");

    ofDrawBitmapStringHighlight( log.str(), 50 , 50);
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
    case 'v':
        if(!visualiser.isThreadRunning()){
            visualiser.startThread();
        } else {
            visualiser.stopThread();
        }
      break;
    case OF_KEY_UP:
    case OF_KEY_RIGHT:
        visualiser.incrementSelected();
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
