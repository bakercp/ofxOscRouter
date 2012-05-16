#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOscRouterNode.h"

class ofxOscRouter : public ofxOscRouterNode {
	
public:
	
	ofxOscRouter();
	ofxOscRouter(string rootNodeName, int listenerPort);
	virtual ~ofxOscRouter();
    
    void setup(string rootNodeName, int listenerPort);
    void update(ofEventArgs& eventsArts);
    void setPort(int port);
    
    void processOscMessage(string address, ofxOscMessage& m);
	
    ofxOscReceiver& getOscReciever();
	
protected:
		
	ofxOscReceiver	receiver;  // TODO: remove this from the router
                               // to separate the two functions
                               // for more flexibility.
	
};

