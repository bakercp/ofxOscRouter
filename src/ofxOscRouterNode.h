#pragma once

#include <map>
#include <set>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOscRouterNode.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"

extern "C" {    
    #include "osc_match.h"
}

using namespace std;
using Poco::RegularExpression;
using Poco::toUpper;
using Poco::toLower;
using Poco::icompare;

class ofxOscRouterNode {

public:

	// TODO: automaticaly add and remove slashes as needed
    // TODO: Pattern matching with methods
    // http://opensoundcontrol.org/spec-1_0-examples#OSCstrings
    // Order of Invocation of OSC Methods matched
    //  by OSC Messages in an OSC Bundle
    ofxOscRouterNode();
    ofxOscRouterNode(string _nodeName);

	ofxOscRouterNode(ofxOscRouterNode* _oscParent, string _oscNodeName);
    
    virtual ~ofxOscRouterNode();
    
    // the address here is the remaining bit of the address, as it is processed and pruned
    virtual void processOscMessage(string pattern, ofxOscMessage& m) = 0;
	
    void routeOscMessage(string pattern, ofxOscMessage& m);
    void setOscParent(ofxOscRouterNode* _oscParent);
    
    // recursively locate the root node
    ofxOscRouterNode* getOscRoot();
    
    void setOscNodeName(string _oscNodeName);    
    string getOscNodeName();
    ofxOscRouterNode* getOscParent();
    // TODO: this doesn't seem very efficient ... must be a better way ...
    vector<ofxOscRouterNode*> getOscSiblings();

    vector<ofxOscRouterNode*> getOscChildren(); 
    void addOscChild(ofxOscRouterNode* oscChild);
    bool removeOscChild(ofxOscRouterNode* oscChild);
    bool hasOscMethod(string _method);
    void addOscMethod(string _method);
    bool removeOscMethod(string _method);
    bool isMatch(string s0, string s1);
    bool toBoolean(ofxOscMessage& m, int index);
    bool validateOscSignature(string signature, ofxOscMessage& m); 
    

protected:
	
    string oscNodeName;
    
    ofxOscRouterNode* oscParent;
    vector<ofxOscRouterNode*> oscChildren;
    set<string> oscMethods;

};
