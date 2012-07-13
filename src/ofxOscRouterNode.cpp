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

#include "ofxOscRouterNode.h"

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode() {
}

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode(const string& _nodeName) {
    addOscNodeAlias(_nodeName);
}

////--------------------------------------------------------------
//ofxOscRouterNode::ofxOscRouterNode(ofxOscRouterNode* _oscParent, string _oscNodeName) {
//    addOscParent(_oscParent);
//    addOscNodeAlias(_oscNodeName);
//}

//--------------------------------------------------------------
ofxOscRouterNode::~ofxOscRouterNode() {}

//--------------------------------------------------------------
void ofxOscRouterNode::routeOscMessage(string pattern, ofxOscMessage& m) {
  
//// For debugging.
//
    int level = 0;
    string tabs = "";
    string fullAddress = m.getAddress();
    string patternAddress = pattern;
    
    string::size_type offset = fullAddress.find( patternAddress, 0 );
    if( offset != string::npos ) {
        level = std::count(fullAddress.begin(), fullAddress.begin() + offset, '/');
        tabs = string(level*2,'\t');
    } else {
        level = 0;
    }
    
    
    int pattrOffset = 0;
    int addrOffset  = 0;
    int matchResult = 0;
    
    string matchedNodeAlias = "";
    
    char* _pattern = (char*)pattern.c_str();
    //char* _thisAddress = (char*)oscNodeName.c_str();
    
    // check our main node name
    // matchResult = osc_match(_pattern, _thisAddress, &pattrOffset, &addrOffset);
        
    // check our aliases
    if(!oscNodeNameAliases.isEmpty()) {//matchResult == 0) {
        // this reversal is to prevent overlap of aliases.  
        // an alias list like /l /live /lives will match /lives on /l and we want
        // it to match the largest one.  So we sort them in reverse alphabetical order.
        vector<string> aliases = oscNodeNameAliases.toArrayReverse();

        for(int i = 0; i < aliases.size(); i++) {
            ofLog(OF_LOG_VERBOSE, tabs + aliases[i] + " match? " + pattern);

            //string alias = aliases[i];
            pattrOffset = 0;
            addrOffset  = 0;
            matchResult = 0;
            char* _thisAlias = (char*)(aliases[i]).c_str();
            matchResult = osc_match(_pattern, _thisAlias, &pattrOffset, &addrOffset);
            
            if(matchResult != 0) {
                matchedNodeAlias = aliases[i];
                break;
            }
        }
    } else {
        ofLog(OF_LOG_ERROR, tabs + "ofxOscRouterNode: This node has no aliases: " + m.getAddress());
    }

    if(matchResult == 0) {
        
        

        ofLog(OF_LOG_VERBOSE, tabs + "ofxOscRouterNode: No match for: " + m.getAddress());
        
        return;
    } else if(matchResult == OSC_MATCH_ADDRESS_COMPLETE) {
        
        
        pattern = pattern.substr(pattrOffset);
        
        vector<string> methods = getOscMethods();
        
        
        for(int i = 0; i < methods.size(); i++) {
            ofLog(OF_LOG_VERBOSE, string((level+1)*2,'\t') + "pattern = " + pattern + " method=" + methods[i]);
        }
        
        
        if(hasOscMethod(pattern)) {// || children.size() <= 0) {
            ofLog(OF_LOG_VERBOSE, tabs + "ofxOscRouterNode: "+ matchedNodeAlias + " had the method : " + pattern);
            if(!executeOscPlugMethod(pattern, m)) {
                processOscMessage(pattern, m);
            }
        } else {
            ofLog(OF_LOG_VERBOSE, tabs + "ofxOscRouterNode: no methods, routing onward.");
            vector<ofxOscRouterNode*> children = oscChildren.toArray();
            for(int i = 0; i < children.size(); i++) { 
                children[i]->routeOscMessage(pattern, m);
            }
        }

        
        // otherwise, it gets ignored
        
    } else if(matchResult == OSC_MATCH_PATTERN_COMPLETE) {
        ofLog(OF_LOG_VERBOSE,tabs + "ofxOscRouterNode: PATTERN COMPLETE ++ PROCESSING.");
        if(!executeOscPlugMethod(pattern, m)) {
            processOscMessage(pattern, m);
        }
    } else if(matchResult == OSC_MATCH_ADDRESS_AND_PATTERN_COMPLETE) {
        ofLog(OF_LOG_VERBOSE,tabs + "ofxOscRouterNode: ADDRESS AND PATTERN COMPLETE : PERFECT MATCH ++ PROCESSING");
        if(!executeOscPlugMethod(pattern, m)) {
            processOscMessage(pattern, m);
        }
    } else {
        ofLog(OF_LOG_ERROR, tabs + "ofxOscRouterNode: Unknown osc_match result.");
    }
    
}

//--------------------------------------------------------------
ofxOscRouterNode* ofxOscRouterNode::getFirstOscParent() const {
    if(hasParents()) {
        return *oscParents.begin();
    } else {
        return NULL;
    }
}

//--------------------------------------------------------------
vector<ofxOscRouterNode*> ofxOscRouterNode::getOscParents() const {
    return oscParents.toArray();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscParent(ofxOscRouterNode* _oscParent) const {
    return oscParents.contains(_oscParent);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscParent(ofxOscRouterNode* _oscParent) {
    return oscParents.add(_oscParent);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscParent(ofxOscRouterNode* _oscParent) {
    return oscParents.remove(_oscParent);
}

//--------------------------------------------------------------
vector<ofxOscRouterNode*> ofxOscRouterNode::getOscSiblings() const {
    vector<ofxOscRouterNode*> oscSiblings;
    if(!oscParents.isEmpty()) {
        vector<ofxOscRouterNode*> parents = oscParents.toArray();
        for(int i = 0; i < parents.size(); i++) {
            vector<ofxOscRouterNode*> children = parents[i]->getOscChildren();
            for(int j = 0; j < children.size(); j++) {
                if(children[j] != this) {
                    oscSiblings.push_back(children[j]);
                }
            }
        }
        return oscSiblings;
    } else {
        return oscSiblings;
    }
}

//--------------------------------------------------------------
// recursively locate the root node
ofxOscRouterNode* ofxOscRouterNode::getOscRoot() const {
    ofxOscRouterNode* parent = getFirstOscParent();
    return parent != NULL ? parent->getOscRoot() : parent;
}

// currently broken
/*
 void getFullOscNodeAddress(string &fullAddress) {
 // if it hasn't been cached yet
 if(parent != NULL) {
 getFullOscNodeAddress(fullAddress);
 } else {
 // nothing
 }
 fullAddress = nodeName + fullAddress;
 }
 */

//--------------------------------------------------------------
bool ofxOscRouterNode::hasAliases() const {
    return !oscNodeNameAliases.isEmpty();
}

//--------------------------------------------------------------
string ofxOscRouterNode::getFirstOscNodeAlias() const {
    if(hasAliases()) {
        return *oscNodeNameAliases.begin();
    } else {
        return NULL;
    }
}

//--------------------------------------------------------------
vector<string> ofxOscRouterNode::getOscNodeAliases() const {
    return oscNodeNameAliases.toArray();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscNodeAlias(const string& _oscNodeAlias) const {
    return oscNodeNameAliases.contains(_oscNodeAlias);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscNodeAlias(const string& _oscNodeAlias) {
    return oscNodeNameAliases.add(_oscNodeAlias);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscNodeAlias(const string& _oscNodeAlias) {
    return oscNodeNameAliases.remove(_oscNodeAlias);
}

//--------------------------------------------------------------
void ofxOscRouterNode::clearOscNodeAliases() {
    return oscNodeNameAliases.clear();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasChildren() const {
    return !oscChildren.isEmpty();
}

//--------------------------------------------------------------
ofxOscRouterNode* ofxOscRouterNode::getFirstOscChild() const {
    if(hasChildren()) {
        return *oscChildren.begin();
    } else {
        return NULL;
    }
}

//--------------------------------------------------------------
vector<ofxOscRouterNode*> ofxOscRouterNode::getOscChildren() const {
    return oscChildren.toArray();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::ofxOscRouterNode::addOscChild(ofxOscRouterNode* oscChild) {
    if(oscChild != NULL) {
        if(oscChild->addOscParent(this)) {
            return oscChildren.add(oscChild);
        } else {
            ofLog(OF_LOG_ERROR, "ofxOscRouterNode: Failed to add OSC Child.  Could not add self as Parent.");
            return false;
        }
    } else {
        ofLog(OF_LOG_ERROR, "ofxOscRouterNode: Failed to add OSC Child.  Child was NULL.");
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscChild(ofxOscRouterNode* oscChild) {
    if(oscChild != NULL) {
        if(!oscChild->removeOscParent(this)) {
            ofLog(OF_LOG_ERROR, "ofxOscRouterNode: Failed to remove parent link.");
            return false;
        } else {
            return true;
        }
    } {
        ofLog(OF_LOG_ERROR, "ofxOscRouterNode: Failed to remove OSC Child.  Child was NULL.");
        return false;
    }
    
    return oscChildren.remove(oscChild);
}

//--------------------------------------------------------------
vector<string> ofxOscRouterNode::getOscMethods() const {
    return oscMethods.toArray();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscMethod(string _method) const {
    // TODO: contains should should feature some kind of wildcard
    // or contains "like" etc.
    
    return oscMethods.contains(_method);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscMethod(string _method) {
    return oscMethods.add(_method);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscPlugMethod(string _method, NoArgPlugFunc func) {
    if(!hasOscMethod(_method) && addOscMethod(_method)) {
        noArgPlugFuncMap[_method] = func;
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscPlugMethod(string _method, OscMessagePlugFunc func) {
    if(!hasOscMethod(_method) && addOscMethod(_method)) {
        oscMessagePlugFuncMap[_method] = func;
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscMethod(string _command) {
    removeOscPlugMethod(_command);
    return oscMethods.remove(_command);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscPlugMethod(string _method) {
    
    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
    if(noArgPlugFuncMapIter != noArgPlugFuncMap.end()) {
        return true;
    } 
    
    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
    if(oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()) {
        return true;
    }
    
    return false;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscPlugMethod(string _method) {
    noArgPlugFuncMap.erase(_method);
    oscMessagePlugFuncMap.erase(_method);
    return true;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::executeOscPlugMethod(string _method, const ofxOscMessage& m) {

    bool foundOne = false;
    
    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
    if(noArgPlugFuncMapIter != noArgPlugFuncMap.end()) {
        noArgPlugFuncMapIter->second();
        foundOne = true;
    } 
    
    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
    if(oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()) {
        oscMessagePlugFuncMapIter->second(m);
        foundOne = true;
    }

    return foundOne;
}

//--------------------------------------------------------------
string ofxOscRouterNode::normalizeMethodName(const string& name) {
    string theName = name;
    theName = replace(theName, " ", "_");
    theName = replace(theName, "#", "_");
    theName = replace(theName, "*", "_");
    theName = replace(theName, ",", "_");
    theName = replace(theName, "/", "_");
    theName = replace(theName, "?", "_");
    theName = replace(theName, "[", "_");
    theName = replace(theName, "]", "_");
    theName = replace(theName, "{", "_");
    theName = replace(theName, "}", "_");
    return theName;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::isMatch(const string& s0, const string& s1) {
    return icompare(s0,s1) == 0;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::getArgAsBoolean(const ofxOscMessage& m, int index) {
    if(m.getNumArgs() < index) {
        ofxOscArgType argType = m.getArgType(index);
        if(argType == OFXOSC_TYPE_INT32) {
            return m.getArgAsInt32(index) != 0;
        } else if(argType == OFXOSC_TYPE_FLOAT) {
            return m.getArgAsFloat(index) != 0.0f;
        } else if(argType == OFXOSC_TYPE_STRING) {
            return isMatch(m.getArgAsString(index),"true");
        } else {
            return false;
        }
    } else {
        return false;
    }
}

//--------------------------------------------------------------
float ofxOscRouterNode::getArgAsFloatUnchecked(const ofxOscMessage& m, int index) {
    ofxOscArgType argType = m.getArgType(index);
    if(argType == OFXOSC_TYPE_INT32) {
        return (float)m.getArgAsInt32(index);
    } else if(argType == OFXOSC_TYPE_FLOAT) {
        return (float)m.getArgAsFloat(index);
    } else {
        ofLog(OF_LOG_ERROR, "getArgAsFloatUnchecked: invalid arg type. returning 0.0f");
        return 0.0f;
    }
}

//--------------------------------------------------------------
int ofxOscRouterNode::getArgAsIntUnchecked(const ofxOscMessage& m, int index) {
    ofxOscArgType argType = m.getArgType(index);
    if(argType == OFXOSC_TYPE_INT32) {
        return (int)m.getArgAsInt32(index);
    } else if(argType == OFXOSC_TYPE_FLOAT) {
        return (int)m.getArgAsFloat(index);
    } else {
        ofLog(OF_LOG_ERROR, "getArgAsIntUnchecked: invalid arg type. returning 0.0f");
        return 0.0f;
    }
}

//--------------------------------------------------------------
string ofxOscRouterNode::getArgAsStringUnchecked(const ofxOscMessage& m, int index) {
    ofxOscArgType argType = m.getArgType(index);
    if(argType == OFXOSC_TYPE_INT32) {
        return ofToString(m.getArgAsInt32(index));
    } else if(argType == OFXOSC_TYPE_FLOAT) {
        return ofToString(m.getArgAsFloat(index));
    } else if(argType == OFXOSC_TYPE_STRING) {
        return m.getArgAsString(index);
    } else {
        ofLog(OF_LOG_ERROR, "getArgAsStringUnchecked: invalid arg type. returning 0.0f");
        return "NULL";
    }
}

//--------------------------------------------------------------
ofColor ofxOscRouterNode::getArgsAsColor(const ofxOscMessage& m, int startIndex) {
    ofColor color;
    if(m.getNumArgs() > startIndex) {
        vector<float> args = getArgsAsFloatArray(m,startIndex);
        if(args.size() == 1) {
            color.set(args[0]);
        } else if(args.size() == 2) {
            color.set(args[0],args[1]);
        } else if(args.size() == 3) {
            color.set(args[0],args[1],args[2]);
        } else if(args.size() == 4) {
            color.set(args[0],args[1],args[2],args[3]);
            if(args.size() > 4) {
                ofLog(OF_LOG_ERROR, "getArgsAsColor: more than 4 args for color, ignoring.");
            }
        } 
    } else {
        ofLog(OF_LOG_ERROR, "getArgsAsColor: no args to parse.");
    }
    return color;
}

//--------------------------------------------------------------
ofPoint ofxOscRouterNode::getArgsAsPoint(const ofxOscMessage& m, int startIndex) {
    ofPoint point;
    if(m.getNumArgs() > startIndex) {
        vector<float> args = getArgsAsFloatArray(m,startIndex);
        if(args.size() == 1) {
            point.set(args[0]);
        } else if(args.size() == 2) {
            point.set(args[0],args[1]);
        } else if(args.size() == 3) {
            point.set(args[0],args[1],args[2]);
            if(args.size() > 3) {
                ofLog(OF_LOG_ERROR, "getArgsAsPoint: more than 3 args for point, ignoring extras.");
            }
        } 
    } else {
        ofLog(OF_LOG_ERROR, "getArgsAsPoint: no args to parse.");
    }
    return point;
}

//--------------------------------------------------------------
vector<float> ofxOscRouterNode::getArgsAsFloatArray(const ofxOscMessage& m, int index) {
    vector<float> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsFloatUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<int> ofxOscRouterNode::getArgsAsIntArray(const ofxOscMessage& m, int index) {
    vector<int> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsIntUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<string> ofxOscRouterNode::getArgsAsStringArray(const ofxOscMessage& m, int index) {
    vector<string> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsStringUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::validateOscSignature(const string& signature, const ofxOscMessage& m) {
    
    string mSignature = "";
    // make the signature
    for(int i=0; i < m.getNumArgs(); i++) {
        mSignature+=m.getArgTypeName(i).at(0);
    }
    
    RegularExpression re(signature);
    bool match = re.match(mSignature);
    
    if(!match) {
        ofLog(OF_LOG_ERROR, "Signature: " + signature + " did not match mSignature= " + mSignature);
    }
    
    return match;
}