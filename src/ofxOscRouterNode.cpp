
#include "ofxOscRouterNode.h"

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode() {
    oscParent = NULL;
    oscNodeName = "/";
}

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode(string _nodeName) {
    oscParent = NULL;
    oscNodeName = _nodeName;
}

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode(ofxOscRouterNode* _oscParent, string _oscNodeName) {
    oscParent = _oscParent;
    oscNodeName = _oscNodeName;
}

//--------------------------------------------------------------
ofxOscRouterNode::~ofxOscRouterNode() {}

//--------------------------------------------------------------
void ofxOscRouterNode::routeOscMessage(string pattern, ofxOscMessage& m) {
    
    
    ofLog(OF_LOG_VERBOSE, "ofxOscRouterNode: " + getOscNodeName() + " processing : " +  m.getAddress());
    
    
    int pattrOffset = 0;
    int addrOffset  = 0;
    int matchResult = 0;
    
    char* _pattern = (char*)pattern.c_str();
    char* _thisAddress = (char*)oscNodeName.c_str();
    
    // check our main node name
    matchResult = osc_match(_pattern, _thisAddress, &pattrOffset, &addrOffset);
        
    // check our aliases
    if(matchResult == 0) {
        set<string>::iterator it;
        for(it = oscNodeNameAliases.begin(); it != oscNodeNameAliases.end(); it++ ) {
            char* _thisAlias = (char*)(*it).c_str();
            matchResult = osc_match(_pattern, _thisAlias, &pattrOffset, &addrOffset);
            if(matchResult != 0) {
                break;
            }
        }
    }
    

    //cout << m.getAddress() << "<<< " << endl;
    
    if(matchResult == 0) {
        ofLog(OF_LOG_VERBOSE, "\tofxOscRouterNode: No match for: " + m.getAddress());
        return;
    } else if(matchResult == OSC_MATCH_ADDRESS_COMPLETE) {
        pattern = pattern.substr(pattrOffset);
        
        if(hasOscMethod(pattern)) {// || children.size() <= 0) {
            ofLog(OF_LOG_VERBOSE, "\t\t\t" + getOscNodeName() + " had the method : " +  m.getAddress());
            processOscMessage(pattern, m);
        } else {
            for(int i = 0; i < oscChildren.size(); i++) { 
                oscChildren[i]->routeOscMessage(pattern, m);
            }
        }
        
        // otherwise, it gets ignored
        
    } else if(matchResult == OSC_MATCH_PATTERN_COMPLETE) {
        ofLog(OF_LOG_VERBOSE,"ofxOscRouterNode: PATTERN COMPLETE ++ PROCESSING.");
        processOscMessage(pattern, m);
    } else if(matchResult == OSC_MATCH_ADDRESS_AND_PATTERN_COMPLETE) {
        ofLog(OF_LOG_VERBOSE,"ofxOscRouterNode: ADDRESS AND PATTERN COMPLETE : PERFECT MATCH ++ PROCESSING");
        processOscMessage(pattern, m);
    } else {
        ofLog(OF_LOG_ERROR, "ofxOscRouterNode: Unknown osc_match result.");
    }
    
    
}

//--------------------------------------------------------------
void ofxOscRouterNode::setOscParent(ofxOscRouterNode* _oscParent) {
    oscParent = _oscParent;
}

//--------------------------------------------------------------
ofxOscRouterNode* ofxOscRouterNode::getOscParent() {
    return oscParent;
}




// TODO: this doesn't seem very efficient ... must be a better way ...
//--------------------------------------------------------------
vector<ofxOscRouterNode*> ofxOscRouterNode::getOscSiblings() {
    
    vector<ofxOscRouterNode*> oscSiblings;
    
    
    if(oscParent != NULL) {
        vector<ofxOscRouterNode*> allOscChildrenFromParent = oscParent->getOscChildren();
        vector<ofxOscRouterNode*>::iterator it;
        
        // iterator to vector element:
        it = find (allOscChildrenFromParent.begin(), allOscChildrenFromParent.end(), this);
        
        if(it != allOscChildrenFromParent.end()) {
            allOscChildrenFromParent.erase(it);
        }   
        
        oscSiblings = allOscChildrenFromParent;
        
        return oscSiblings;
        
    } else {
        return oscSiblings;
    }
}

//--------------------------------------------------------------
// recursively locate the root node
ofxOscRouterNode* ofxOscRouterNode::getOscRoot() {
    return oscParent != NULL ? oscParent->getOscRoot() : oscParent;
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
void ofxOscRouterNode::setOscNodeName(string _oscNodeName) {
    oscNodeName = _oscNodeName;
}

//--------------------------------------------------------------
string ofxOscRouterNode::getOscNodeName() {
    return oscNodeName;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscNodeAlias(string _oscNodeAlias) {
    return oscNodeNameAliases.find(_oscNodeAlias) != oscNodeNameAliases.end();
}

//--------------------------------------------------------------
void ofxOscRouterNode::addOscNodeAlias(string _oscNodeAlias) {
    oscNodeNameAliases.insert(_oscNodeAlias);
}

//--------------------------------------------------------------
void ofxOscRouterNode::removeOscNodeAlias(string _oscNodeAlias) {
    oscNodeNameAliases.erase(oscNodeNameAliases.find(_oscNodeAlias));
}

//--------------------------------------------------------------
void ofxOscRouterNode::clearOscNodeAliases() {
    oscNodeNameAliases.clear();
}

//--------------------------------------------------------------
vector<ofxOscRouterNode*> ofxOscRouterNode::getOscChildren() {
    return oscChildren;
}

//--------------------------------------------------------------
void ofxOscRouterNode::ofxOscRouterNode::addOscChild(ofxOscRouterNode* oscChild) {
    
    // TODO: should this be auto sorted:: std::sort
    
    oscChild->setOscParent(this);
    oscChildren.push_back(oscChild);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscChild(ofxOscRouterNode* oscChild) {
    vector<ofxOscRouterNode*>::iterator it;
    
    // iterator to vector element:
    it = find (oscChildren.begin(), oscChildren.end(), oscChild);
    
    if(it != oscChildren.end()) {
        //it->setParent(NULL); // TODO: is this a problem?
        oscChildren.erase(it);
        return true;
    } else {
        return false;
    }
    
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscMethod(string _method) {
    return oscMethods.find(_method) != oscMethods.end();
}

//--------------------------------------------------------------
void ofxOscRouterNode::addOscMethod(string _command) {
    oscMethods.insert(_command);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscMethod(string _command) {
    oscMethods.erase(_command);
}

//--------------------------------------------------------------
bool ofxOscRouterNode::isMatch(string s0, string s1) {
    return icompare(s0,s1) == 0;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::getArgAsBoolean(ofxOscMessage& m, int index) {
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

float ofxOscRouterNode::getArgAsFloatUnchecked(ofxOscMessage& m, int index) {
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

int ofxOscRouterNode::getArgAsIntUnchecked(ofxOscMessage& m, int index) {
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

string ofxOscRouterNode::getArgAsStringUnchecked(ofxOscMessage& m, int index) {
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
ofColor ofxOscRouterNode::getArgsAsColor(ofxOscMessage& m, int startIndex) {
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
ofPoint ofxOscRouterNode::getArgsAsPoint(ofxOscMessage& m, int startIndex) {
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
vector<float> ofxOscRouterNode::getArgsAsFloatArray(ofxOscMessage& m, int index) {
    vector<float> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsFloatUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<int> ofxOscRouterNode::getArgsAsIntArray(ofxOscMessage& m, int index) {
    vector<int> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsIntUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<string> ofxOscRouterNode::getArgsAsStringArray(ofxOscMessage& m, int index) {
    vector<string> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsStringUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::validateOscSignature(string signature, ofxOscMessage& m) {
    
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