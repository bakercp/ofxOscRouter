#include "ofxOscRouter.h"


//--------------------------------------------------------------
ofxOscRouter::ofxOscRouter() : ofxOscRouterNode(NULL, "/") {
    ofAddListener(ofEvents().update,this,&ofxOscRouter::update);
}

//--------------------------------------------------------------
ofxOscRouter::~ofxOscRouter() { 
    ofRemoveListener(ofEvents().update,this,&ofxOscRouter::update);
}

//--------------------------------------------------------------
void ofxOscRouter::setup(string rootNodeName, int port) {
    setOscNodeName(rootNodeName);
    setPort(port);
	ofLog(OF_LOG_NOTICE, "ofxOscRouter: Listening for osc messages on port " + ofToString(port));
}

//--------------------------------------------------------------
void ofxOscRouter::processOscMessage(string address, ofxOscMessage& m) {
    // the manager received a message
}

//--------------------------------------------------------------
void ofxOscRouter::update(ofEventArgs& eventsArts) {
	// check for waiting messages
	while(receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage( &m ); // fill the message
        routeOscMessage(m.getAddress(), m); // route the message
	}
}

//--------------------------------------------------------------
void ofxOscRouter::setPort(int port) {
    receiver.setup(port);
}

//--------------------------------------------------------------
ofxOscReceiver& ofxOscRouter::getOscReciever() {
    return receiver;
}
