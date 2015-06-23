// =============================================================================
//
// Copyright (c) 2009-2015 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "osc_match.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"
#include "ofxOsc.h"
#include "ofxOscRouterBaseNode.h"


typedef std::function<void ()> NoArgPlugFunc;
typedef std::function<void (const ofxOscMessage& m)> OscMessagePlugFunc;


class ofxOscRouterBaseNode
{
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

    void routeOscMessage(const string& pattern, ofxOscMessage& m);
    
    // recursively locate the root node
    ofxOscRouterBaseNode* getOscRoot();

    virtual set<string>& getOscNodeAliases() = 0;
    virtual const set<string>& getOscNodeAliases() const = 0;

    virtual string getFirstOscNodeAlias() const;
    virtual string getLastOscNodeAlias() const;
    bool hasOscNodeAlias(const string& alias) const;
    
    // parent directory
    bool hasParents() const;
    const ofxOscRouterBaseNode* getFirstOscParent() const;
    ofxOscRouterBaseNode* getFirstOscParent();
    const ofxOscRouterBaseNode* getLastOscParent() const;
    ofxOscRouterBaseNode* getLastOscParent();

    bool hasOscParent(ofxOscRouterBaseNode* _oscParent) const;
    bool addOscParent(ofxOscRouterBaseNode* _oscParent);
    bool removeOscParent(ofxOscRouterBaseNode* _oscParent);

    // child directory
    bool hasChildren() const;
    bool hasChildWithAlias(const string& alias) const;
    bool hasChildWithAlias(const set<string>& aliases) const;
    bool hasChildWithAlias(const set<string>& aliases, string& clashName) const;

    ofxOscRouterBaseNode* getFirstOscChild();
    const ofxOscRouterBaseNode* getFirstOscChild() const;
    
    set<ofxOscRouterBaseNode*>& getOscChildren();
    const set<ofxOscRouterBaseNode*>& getOscChildren() const;
    
    bool hasOscChild(ofxOscRouterBaseNode* oscChild) const;
    bool addOscChild(ofxOscRouterBaseNode* oscChild);
    bool removeOscChild(ofxOscRouterBaseNode* oscChild);
    
    // method directory
    set<string>& getOscMethods();
    const set<string>& getOscMethods() const;

    bool hasOscMethod(const string& _method) const;
    bool addOscMethod(const string& _method);
    bool removeOscMethod(const string& _method);

    bool isNodeActive() const;
    void setNodeActive(bool _bNodeEnabled);
    
    
    string getFirstOscPath() const;
    string getLastOscPath() const;
    
    
    
//    bool hasOscPlugMethod(string _method);
//    bool addOscPlugMethod(string _method, NoArgPlugFunc func);
//    bool addOscPlugMethod(string _method, OscMessagePlugFunc func);
//    bool removeOscPlugMethod(string _method);
//    bool executeOscPlugMethod(string _method, const ofxOscMessage& m);
    
    
    // TODO: add OSC Methods that link directly to a method, rather than go 
    // through the processOscCommand callback.
    
    string schemaToString(int& level) const {
        level++;
        //int tab = 4 * level;
        
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

    static ofColor getArgsAsColor(const ofxOscMessage& m, int index, int endIndex);
    static ofPoint getArgsAsPoint(const ofxOscMessage& m, int index, int endIndex);

    static ofColor getArgsAsColor(const ofxOscMessage& m, int index);
    static ofPoint getArgsAsPoint(const ofxOscMessage& m, int index);
    
    static bool   getArgAsBoolUnchecked(const ofxOscMessage& m, int index);
    static float  getArgAsFloatUnchecked(const ofxOscMessage& m, int index);
    static int    getArgAsIntUnchecked(const ofxOscMessage& m, int index);
    static string getArgAsStringUnchecked(const ofxOscMessage& m, int index);
    
    static vector<bool>   getArgsAsBoolArray(const ofxOscMessage& m, int index, int endIndex);
    static vector<float>  getArgsAsFloatArray(const ofxOscMessage& m, int index, int endIndex);
    static vector<int>    getArgsAsIntArray(const ofxOscMessage& m, int index, int endIndex);
    static vector<string> getArgsAsStringArray(const ofxOscMessage& m, int index, int endIndex);

    static vector<bool>   getArgsAsBoolArray(const ofxOscMessage& m, int index);
    static vector<float>  getArgsAsFloatArray(const ofxOscMessage& m, int index);
    static vector<int>    getArgsAsIntArray(const ofxOscMessage& m, int index);
    static vector<string> getArgsAsStringArray(const ofxOscMessage& m, int index);

    static string getMessageAsString(const ofxOscMessage& m);
    
protected:
    
    bool bNodeActive;
    
private:
	
    // these must be manipulated via methods (to prevent dangling links)
    set<string> oscMethods;
    
	unordered_map<string,NoArgPlugFunc>            noArgPlugFuncMap;
    unordered_map<string,NoArgPlugFunc>::iterator  noArgPlugFuncMapIter;

    unordered_map<string,OscMessagePlugFunc>            oscMessagePlugFuncMap;
    unordered_map<string,OscMessagePlugFunc>::iterator  oscMessagePlugFuncMapIter;

    set<ofxOscRouterBaseNode*>::iterator nodeIter;
    set<ofxOscRouterBaseNode*> oscParents;
    set<ofxOscRouterBaseNode*> oscChildren;

};
