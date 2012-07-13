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

#pragma once

#include <map>
#include <set>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOscRouterNode.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"



// tr1 stuff, for function callbacks
#if (_MSC_VER)
#include <functional>
#include <unordered_map>
    using std::unordered_map;
    using std::function;
    using std::bind;
    using std::ref;
#else
#include <tr1/functional>
#include <tr1/unordered_map>
    using std::tr1::unordered_map;
    using std::tr1::function;
    using std::tr1::bind;
    using std::tr1::ref;
#endif

typedef function<void ()> NoArgPlugFunc;
typedef function<void (const ofxOscMessage& m)> OscMessagePlugFunc;

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

    bool hasOscPlugMethod(string _method);
    bool addOscPlugMethod(string _method, NoArgPlugFunc func);
    bool addOscPlugMethod(string _method, OscMessagePlugFunc func);
    bool removeOscPlugMethod(string _method);
    bool executeOscPlugMethod(string _method, const ofxOscMessage& m);
    
    
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
    
	unordered_map<string,NoArgPlugFunc>            noArgPlugFuncMap;
    unordered_map<string,NoArgPlugFunc>::iterator  noArgPlugFuncMapIter;

    unordered_map<string,OscMessagePlugFunc>            oscMessagePlugFuncMap;
    unordered_map<string,OscMessagePlugFunc>::iterator  oscMessagePlugFuncMapIter;

    ofxSimpleSet<ofxOscRouterNode*> oscParents;
    ofxSimpleSet<ofxOscRouterNode*> oscChildren;

};
