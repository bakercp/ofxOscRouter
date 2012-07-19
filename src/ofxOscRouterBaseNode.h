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
#include "ofxOscRouterBaseNode.h"
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

extern "C" {    
    #include "osc_match.h"
}

using namespace std;
using Poco::RegularExpression;
using Poco::toUpper;
using Poco::toLower;
using Poco::icompare;
using Poco::replace;

class ofxOscRouterBaseNode {

public:

	// TODO: automaticaly add and remove slashes as needed
    // TODO: Pattern matching with methods
    // http://opensoundcontrol.org/spec-1_0-examples#OSCstrings
    // Order of Invocation of OSC Methods matched
    //  by OSC Messages in an OSC Bundle
    ofxOscRouterBaseNode();

	//ofxOscRouterBaseNode(ofxOscRouterBaseNode* _oscParent, string _oscNodeName);
    
    virtual ~ofxOscRouterBaseNode();
    
    // the address here is the remaining bit of the address, as it is processed and pruned
    virtual void processOscCommand(const string& command, const ofxOscMessage& m) = 0;

    void routeOscMessage(string pattern, ofxOscMessage& m);
    
    // recursively locate the root node
    ofxOscRouterBaseNode* getOscRoot();

    virtual set<string>& getOscNodeAliasesRef() = 0;
    virtual string getFirstOscNodeAlias();
    
    // parent directory
    bool hasParents() const;
    ofxOscRouterBaseNode* getFirstOscParent() const;

    bool hasOscParent(ofxOscRouterBaseNode* _oscParent) const;
    bool addOscParent(ofxOscRouterBaseNode* _oscParent);
    bool removeOscParent(ofxOscRouterBaseNode* _oscParent);

    // child directory
    bool hasChildren() const;
    ofxOscRouterBaseNode* getFirstOscChild();
    
    set<ofxOscRouterBaseNode*>& getOscChildrenRef();
    
    bool hasOscChild(ofxOscRouterBaseNode* oscChild) const;
    bool addOscChild(ofxOscRouterBaseNode* oscChild);
    bool removeOscChild(ofxOscRouterBaseNode* oscChild);
    
    // method directory
    set<string>& getOscMethodsRef();
    bool hasOscMethod(const string& _method) const;
    bool addOscMethod(const string& _method);
    bool removeOscMethod(const string& _method);

    bool isNodeActive() const;
    void setNodeActive(bool _bNodeEnabled);
    
    
//    bool hasOscPlugMethod(string _method);
//    bool addOscPlugMethod(string _method, NoArgPlugFunc func);
//    bool addOscPlugMethod(string _method, OscMessagePlugFunc func);
//    bool removeOscPlugMethod(string _method);
//    bool executeOscPlugMethod(string _method, const ofxOscMessage& m);
    
    
    // TODO: add OSC Methods that link directly to a method, rather than go 
    // through the processOscCommand callback.
    
    string schemaToString(int& level) const {
        level++;
        int tab = 4 * level;
        
        stringstream ss;
        
//        vector<string>::iterator stringIter;
//        vector<string> aliases = getOscNodeAliases(); 
//        for(stringIter = aliases.begin(); stringIter != aliases.end(); stringIter++) {
//            ss  << *stringIter << setw(tab) << endl;;
//        }
//
//        vector<string> methods  = getOscMethods(); 
//        for(stringIter = methods.begin(); stringIter != methods.end(); stringIter++) {
//            ss  << *stringIter << setw(tab) << endl;;
//        }
//        
//        vector<ofxOscRouterBaseNode*>::iterator childIter;
//        vector<ofxOscRouterBaseNode*> children = getOscChildren(); 
//        for(childIter = children.begin(); childIter != children.end(); childIter++) {
//            ss  << (*childIter) << setw(tab) << endl;;
//        }
        
        level--;

        return ss.str();
    }
    
    // utility methods
    static string normalizeOscNodeName(const string& name);
    static bool   isValidOscNodeName(const string& name);
    static string normalizeOscCommand(const string& name);
    static bool   isValidOscCommand(const string& name);
    
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

protected:
    
    bool bNodeActive;
    
private:
	
    // these must be manipulated via methods (to prevent dangling links)
    set<string> oscMethods;
    
	unordered_map<string,NoArgPlugFunc>            noArgPlugFuncMap;
    unordered_map<string,NoArgPlugFunc>::iterator  noArgPlugFuncMapIter;

    unordered_map<string,OscMessagePlugFunc>            oscMessagePlugFuncMap;
    unordered_map<string,OscMessagePlugFunc>::iterator  oscMessagePlugFuncMapIter;

    set<ofxOscRouterBaseNode*> oscParents;
    set<ofxOscRouterBaseNode*> oscChildren;

};
