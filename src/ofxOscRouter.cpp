#include "ofxOscRouter.h"

//------------------------------------------------------------------------------
ofxOscRouter::ofxOscRouter() { }

//------------------------------------------------------------------------------
ofxOscRouter::ofxOscRouter(const string& rootNodeName, int listenerPort) {
    setup(rootNodeName, listenerPort);
}

//------------------------------------------------------------------------------
ofxOscRouter::~ofxOscRouter() { }

//------------------------------------------------------------------------------
void ofxOscRouter::setup(const string& rootNodeName, int port) {
    addOscNodeAlias(rootNodeName);
    setPort(port);
	ofLogNotice("ofxOscRouter") << "Listening for osc messages on port " << port;
}

//------------------------------------------------------------------------------
void ofxOscRouter::processOscCommand(const string& command, const ofxOscMessage& m) {
    // the manager received a message
    ofLogNotice("ofxOscRouter") << "The router processed a message.";
}

//------------------------------------------------------------------------------
void ofxOscRouter::update() {
	// check for waiting messages
	while(receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage( &m ); // fill the message
        routeOscMessage(m.getAddress(), m); // route the message
	}
}


//------------------------------------------------------------------------------
void ofxOscRouter::setPort(int port) {
    receiver.setup(port);
}

//------------------------------------------------------------------------------
ofxOscReceiver& ofxOscRouter::getOscReceiver() {
    return receiver;
}
