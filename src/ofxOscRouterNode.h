#pragma once

#include <map>
#include <set>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOscRouterNode.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"

#include "ofxSimpleSet.h"

extern "C" {    
    #include "osc_match.h"
}

using namespace std;
using Poco::RegularExpression;
using Poco::toUpper;
using Poco::toLower;
using Poco::icompare;
using Poco::replace;



class ofxOscRouterNode {

public:

	// TODO: automaticaly add and remove slashes as needed
    // TODO: Pattern matching with methods
    // http://opensoundcontrol.org/spec-1_0-examples#OSCstrings
    // Order of Invocation of OSC Methods matched
    //  by OSC Messages in an OSC Bundle
    ofxOscRouterNode();
    ofxOscRouterNode(const string& _nodeName);

	//ofxOscRouterNode(ofxOscRouterNode* _oscParent, string _oscNodeName);
    
    virtual ~ofxOscRouterNode();
    
    // the address here is the remaining bit of the address, as it is processed and pruned
    virtual void processOscMessage(const string& address, const ofxOscMessage& m) = 0;

    void routeOscMessage(string pattern, ofxOscMessage& m);
    
    // recursively locate the root node
    ofxOscRouterNode* getOscRoot() const;


    bool hasAliases() const;
    string getFirstOscNodeAlias() const; // get the first one -- may not always be the same
    vector<string> getOscNodeAliases() const;
    bool hasOscNodeAlias(const string& _oscNodeAlias) const;
    bool addOscNodeAlias(const string& _oscNodeAlias);
    bool removeOscNodeAlias(const string& _oscNodeAlias);
    void clearOscNodeAliases();
    
    // parent directory
    bool hasParents() const;
    ofxOscRouterNode* getFirstOscParent() const;
    vector<ofxOscRouterNode*> getOscParents() const;
    bool hasOscParent(ofxOscRouterNode* _oscParent) const;
    bool addOscParent(ofxOscRouterNode* _oscParent);
    bool removeOscParent(ofxOscRouterNode* _oscParent);

    // child directory
    bool hasChildren() const;
    ofxOscRouterNode* getFirstOscChild() const;
    vector<ofxOscRouterNode*> getOscChildren() const; 
    bool hasOscChild(ofxOscRouterNode* oscChild) const;
    bool addOscChild(ofxOscRouterNode* oscChild);
    bool removeOscChild(ofxOscRouterNode* oscChild);

    // siblings
    vector<ofxOscRouterNode*> getOscSiblings() const;
    
    // method directory
    vector<string> getOscMethods() const;
    bool hasOscMethod(string _method) const;
    bool addOscMethod(string _method);
    bool removeOscMethod(string _method);
    
    // TODO: add OSC Methods that link directly to a method, rather than go 
    // through the processOscMessage callback.
    
    string schemaToString(int& level) {
        level++;
        int tab = 4 * level;
        
        stringstream ss;
        
        vector<string> aliases = getOscNodeAliases(); 
        for(int i = 0; i < aliases.size(); i++) {
            ss  << aliases[i] << setw(tab) << endl;;
        }

        vector<string> methods  = getOscMethods(); 
        for(int i = 0; i < methods.size(); i++) {
            ss << methods[i] << setw(tab)<< endl;;
        }
        
        vector<ofxOscRouterNode*> children = getOscChildren(); 
        for(int i = 0; i < children.size(); i++) {
            ss << children[i]->schemaToString(level) << setw(tab)  << endl;
        }
        
        level--;

        return ss.str();
    }
    
    // utility methods
    static string normalizeMethodName(const string& name);
    static bool isMatch(const string& s0, const string& s1);
    static bool validateOscSignature(const string&, const ofxOscMessage& m); 

    static bool    getArgAsBoolean(const ofxOscMessage& m, int index);
    static ofColor getArgsAsColor(const ofxOscMessage& m, int index);
    static ofPoint getArgsAsPoint(const ofxOscMessage& m, int index);
    
    static float  getArgAsFloatUnchecked(const ofxOscMessage& m, int index);
    static int    getArgAsIntUnchecked(const ofxOscMessage& m, int index);
    static string getArgAsStringUnchecked(const ofxOscMessage& m, int index);
    
    static vector<float>  getArgsAsFloatArray(const ofxOscMessage& m, int index);
    static vector<int>    getArgsAsIntArray(const ofxOscMessage& m, int index);
    static vector<string> getArgsAsStringArray(const ofxOscMessage& m, int index);

    
    
private:
	
    // these must be manipulated via methods (to prevent dangling links)
    
    ofxSimpleSet<string> oscNodeNameAliases;

    ofxSimpleSet<string> oscMethods;

    ofxSimpleSet<ofxOscRouterNode*> oscParents;
    ofxSimpleSet<ofxOscRouterNode*> oscChildren;

};
