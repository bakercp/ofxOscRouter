/*==============================================================================
 
 Copyright (c) 2010, 2011, 2012 Christopher Baker <http://christopherbaker.net>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 ==============================================================================*/

#include "ofxOscRouter.h"

//--------------------------------------------------------------
ofxOscRouter::ofxOscRouter() {
    ofAddListener(ofEvents().update,this,&ofxOscRouter::update);
}

//--------------------------------------------------------------
ofxOscRouter::~ofxOscRouter() { 
    ofRemoveListener(ofEvents().update,this,&ofxOscRouter::update);
}

//--------------------------------------------------------------
void ofxOscRouter::setup(string rootNodeName, int port) {
    addOscNodeAlias(rootNodeName);
    setPort(port);
	ofLog(OF_LOG_NOTICE, "ofxOscRouter: Listening for osc messages on port " + ofToString(port));
}

//--------------------------------------------------------------
void ofxOscRouter::processOscMessage(const string& pattern, const ofxOscMessage& m) {
    // the manager received a message
	ofLog(OF_LOG_VERBOSE, "ofxOscRouter: the router processed a message.");
}

//--------------------------------------------------------------
void ofxOscRouter::update(ofEventArgs& eventsArgs) {
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
