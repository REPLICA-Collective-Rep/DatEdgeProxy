#include "ofApp.h"
#include "zmq.hpp"

zmq::context_t ctx;

//--------------------------------------------------------------
void ofApp::setup(){

    const std::string proxy_addr = "192.168.0.10";

    SimulateSettings   simulateSettings(proxy_addr);
    WriterSettings     writerSettings  (proxy_addr, args.data_root);

    DataserverSettings dataserverSettings(proxy_addr, args.osc_ip );
    ProxySettings      proxySettings("0.0.0.0");

    if( args.debug){
        ofLogNotice("ofApp::setup") << "DEBUG MODE";
        ofSetLogLevel(OF_LOG_VERBOSE);
        simulateSettings.xsub_ip   = "127.0.0.1";
        writerSettings.xpub_ip     = "127.0.0.1";
        //visualiserSettings.xpub_ip = "127.0.0.1";
    }


    writer.setup(ctx, writerSettings);
    writer.startThread();

    visualiser = new Visualiser(ctx, "vis");
    visualiser->setup();
    visualiser->start();

    if(args.proxy){
        dataserverSettings.xpub_ip = "127.0.0.1";

        proxy.setup(ctx, proxySettings);
        proxy.startThread();
    }

    dataserver.setup(ctx, dataserverSettings);
    dataserver.startThread();


    simulate.setup(ctx, simulateSettings);
    //simulate.startThread();

    ofBackground(ofColor::black);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    visualiser->draw(graphFbo);

    // std::ostringstream log;
    // log << "Framerate: "    << ofGetFrameRate() << "\n";
    // log << "\n";
    // log << "simulate   (s): "  << (simulate.isThreadRunning()   ? "ON" : "OFF") << "\n";
    // log << "writer     (w): "  << (writer.isThreadRunning()     ? "ON" : "OFF") << "\n";
    // if(args.proxy) log << "proxy      (p): "  << (proxy.isThreadRunning()      ? "ON" : "OFF") << "\n";
    // log << "visualiser (v): "  << (visualiser->isThreadRunning() ? "ON" : "OFF");

    // ofDrawBitmapStringHighlight( log.str(), 50 , 50);
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
        if(!args.proxy) break;
        if(!proxy.isThreadRunning()){
            proxy.startThread();
        } else {
            proxy.stopThread();
        }
      break;
    case 'v':
        if(!visualiser->isThreadRunning()){
            visualiser->start();
        } else {
            visualiser->stop();
        }
      break;
    case OF_KEY_UP:
    case OF_KEY_RIGHT:
        visualiser->incrementSelected();
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
