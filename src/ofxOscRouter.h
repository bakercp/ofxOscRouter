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
    void update(ofEventArgs& eventsArgs);
    void setPort(int port);
    
    void processOscMessage(const string& address, const ofxOscMessage& m);
	
    ofxOscReceiver& getOscReciever();
	
    //bool plug(string nodeAddress, ofxOscRouterNode* node);
    //bool plug(string nodeAddress, string method, ofxOscRouterNode* node, Function, function)
    
    // bool unplug(string nodeAddress);
    // bool unplug(string nodeAddress);
    
    
    string dumpSchema() {
        
        
        
        
    }
    
protected:

    //map<string, ofxOscRouterNode*> plugMap;
    
	ofxOscReceiver	receiver;  // TODO: remove this from the router
                               // to separate the two functions
                               // for more flexibility.
	
};

