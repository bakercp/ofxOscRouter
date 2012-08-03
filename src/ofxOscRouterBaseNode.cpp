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

#include "ofxOscRouterBaseNode.h"

bool stringSortNormal        (const string& s0, const string& s1) { return (s0 < s1); }
bool stringSortReverse       (const string& s0, const string& s1) { return (s0 > s1); }
bool stringSortLengthNormal  (const string& s0, const string& s1) { return (s0.length() < s1.length() ); }
bool stringSortLengthReverse (const string& s0, const string& s1) { return (s0.length() > s1.length() ); }

//--------------------------------------------------------------
ofxOscRouterBaseNode::ofxOscRouterBaseNode() {
    bNodeActive = true;
}


////--------------------------------------------------------------
//ofxOscRouterBaseNode::ofxOscRouterBaseNode(ofxOscRouterBaseNode* _oscParent, string _oscNodeName) {
//    addOscParent(_oscParent);
//    addOscNodeAlias(_oscNodeName);
//}

//--------------------------------------------------------------
ofxOscRouterBaseNode::~ofxOscRouterBaseNode() {}

//--------------------------------------------------------------
void ofxOscRouterBaseNode::routeOscMessage(string pattern, ofxOscMessage& m) {
    
    if(!isNodeActive()) return; // bail if this node is active
    
    
    //cout << "routing=>" << pattern << " / " << m.getAddress() << " and have " << oscChildren.size() << " oscChildren" << endl;
    
//// For debugging.

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
    int matchResult = OSC_MATCH_NONE;
    
    string matchedNodeAlias = "";
    
    char* _pattern = (char*)pattern.c_str();
    
    // check our aliases
    set<string>& aliasesRef = getOscNodeAliasesRef();
    
    //cout << "found " << aliasesRef.size() << " aliases " << endl;
    
    if(!aliasesRef.empty()) {//matchResult == 0) {
        // this reversal is to prevent overlap of aliases.  
        // an alias list like /l /live /lives will match /lives on /l and we want
        // it to match the largest one.  So we sort them in reverse alphabetical order.
        //sort(aliases.begin(), aliases.end(), stringSortLengthReverse); // sort 

//        vector<string>::iterator aliasIter;
        std::set<string>::reverse_iterator aliasIter;
        
        //int xxx = 0;
        
        for( aliasIter = aliasesRef.rbegin(); aliasIter != aliasesRef.rend(); ++aliasIter) {

            
            //cout << "\t\t\t\t\t\t #aliases=" << aliasesRef.size() <<  " index = " << xxx << endl;
            //xxx++;
        
//        for(aliasIter = aliasesRef.begin(); aliasIter != aliasesRef.end(); aliasIter++) {
            string alias = (*aliasIter);
            ofLog(OF_LOG_VERBOSE, tabs + "/"+ alias + " match? " + pattern);

            //string alias = aliases[i];
            pattrOffset = 0;
            addrOffset  = 0;
            matchResult = 0;
            char* _thisAlias = (char*)("/"+alias).c_str();
            matchResult = osc_match(_pattern, _thisAlias, &pattrOffset, &addrOffset);
            
            if(matchResult != 0) {
                matchedNodeAlias = alias;
                break;
            }
        }
        
    } else {
        ofLog(OF_LOG_ERROR, tabs + "ofxOscRouterBaseNode: This node " + getFirstOscNodeAlias() + " has no aliases: " + m.getAddress());
    }

    if(matchResult == OSC_MATCH_NONE) {
        ofLog(OF_LOG_VERBOSE, tabs + "ofxOscRouterBaseNode: No match for: " + m.getAddress());
        return;
    } else {
        
        //cout << "evaluating results == pattern = " << pattern << endl;
        
        if(matchResult == OSC_MATCH_ADDRESS_AND_PATTERN_COMPLETE) {
            // an exact match for this node and there is no method to try.
            processOscCommand(pattern, m);
        } else if(matchResult == OSC_MATCH_PATTERN_COMPLETE) {
            // the pattern matched the addres, but the address still has nodes left.  
            // pass it on. to be processed by this node.  maybe something clever is going on.  
            processOscCommand(pattern, m);
        } else if(matchResult == OSC_MATCH_ADDRESS_COMPLETE) {
            // extract the updated pattern 
            string remainingPattern = pattern.substr(pattrOffset);
            string command = remainingPattern;

            // prepare the command for testing
            if(command.find("/") == 0) {
                command = command.substr(1); // remove slash
            }
            
            // if it is a command, then process it
            if(isValidOscCommand(command)) {
                processOscCommand(command, m);
            } else {
                // if it is not a command, try to pass it on up the chain
                //vector<ofxOscRouterBaseNode*> children = oscChildren.toArray();
                
                //cout << "Passing it up the chain and has " << oscChildren.size() << " oscChildren" << endl;
                
                set<ofxOscRouterBaseNode*>::iterator childIter = oscChildren.begin();
//                for (childIter = oscChildren.begin(); childIter != oscChildren.end(); childIter++) {
//                    cout << " : " <<  (*childIter)->getFirstOscNodeAlias() << endl;
//                }

                //cout << "----" << endl;
                
                for (childIter = oscChildren.begin(); childIter != oscChildren.end(); childIter++) {
                    
                    
                    if(*childIter != NULL) {
                        //cout << " : " <<  (*childIter)->getFirstOscNodeAlias() << endl;
                        // TODO: when killing off children, MUST SET BACK TO NULL
                        // TODO: what happens if routing a message DESTROYS one of the children in the vector?
                        (*childIter)->routeOscMessage(remainingPattern, m);
                    }
                }
                
                //cout << "Done passing it up the chain." << endl;
                
                
            }
        } else {
            ofLog(OF_LOG_ERROR, tabs + "ofxOscRouterBaseNode: Unknown osc_match result.");
        }
    }

}

////--------------------------------------------------------------
//ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscParent() const {
//    if(hasParents()) {
//        return *oscParents.begin();
//    } else {
//        return NULL;
//    }
//}

////--------------------------------------------------------------
//vector<ofxOscRouterBaseNode*> ofxOscRouterBaseNode::getOscParents() const {
//    return oscParents.toArray();
//}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::hasOscParent(ofxOscRouterBaseNode* _oscParent) const {
    return _oscParent != NULL && oscParents.find(_oscParent) != oscParents.end();
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::addOscParent(ofxOscRouterBaseNode* _oscParent) {
    return oscParents.insert(_oscParent).second;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::removeOscParent(ofxOscRouterBaseNode* _oscParent) {
    if(hasOscParent(_oscParent)) {
        oscParents.erase(_oscParent);
        return true;
    } else {
        return false;
    }
}

////--------------------------------------------------------------
//vector<ofxOscRouterBaseNode*> ofxOscRouterBaseNode::getOscSiblings() const {
//    vector<ofxOscRouterBaseNode*> oscSiblings;
//    if(!oscParents.isEmpty()) {
//        vector<ofxOscRouterBaseNode*> parents = oscParents.toArray();
//        for(int i = 0; i < parents.size(); i++) {
//            vector<ofxOscRouterBaseNode*> children = parents[i]->getOscChildren();
//            for(int j = 0; j < children.size(); j++) {
//                if(children[j] != this) {
//                    oscSiblings.push_back(children[j]);
//                }
//            }
//        }
//        return oscSiblings;
//    } else {
//        return oscSiblings;
//    }
//}

//--------------------------------------------------------------
// recursively locate the root node
ofxOscRouterBaseNode* ofxOscRouterBaseNode::getOscRoot() {
    ofxOscRouterBaseNode* parent = getFirstOscParent();
    return parent != NULL ? parent->getOscRoot() : parent;
}

//--------------------------------------------------------------
string ofxOscRouterBaseNode::getFirstOscNodeAlias() {
    set<string>& aliasesRef = getOscNodeAliasesRef();
    if(!aliasesRef.empty()) {
        return *aliasesRef.begin();
    } else {
        ofLog(OF_LOG_ERROR,"ofxOscRouterBaseNode::getFirstOscNodeAlias() No ALiases!");
        return "NO ALIASES!";
    }
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
bool ofxOscRouterBaseNode::hasChildren() const {
    return !oscChildren.empty();
}

//--------------------------------------------------------------
ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscChild() {
    if(hasChildren()) {
        return *oscChildren.begin();
    } else {
        return NULL;
    }
}

////--------------------------------------------------------------
//vector<ofxOscRouterBaseNode*> ofxOscRouterBaseNode::getOscChildren() const {
//    return oscChildren.toArray();
//}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::hasOscChild(ofxOscRouterBaseNode* oscChild) const {
    return oscChild != NULL && oscChildren.find(oscChild) != oscChildren.end();
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::ofxOscRouterBaseNode::addOscChild(ofxOscRouterBaseNode* oscChild) {
    if(oscChildren.insert(oscChild).second) {
        oscChild->addOscParent(this);
        return true;
    } else {
        ofLog(OF_LOG_ERROR, "ofxOscRouterBaseNode: Failed to add OSC Child.  Child was NULL.");
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::removeOscChild(ofxOscRouterBaseNode* oscChild) {
    if(hasOscChild(oscChild)) {
        oscChild->removeOscParent(this);
        oscChildren.erase(oscChild);
        return true;
    } else {
        ofLog(OF_LOG_ERROR, "ofxOscRouterBaseNode: Failed to remove OSC Child.  Child was NULL.");
        return false;
    }

}

////--------------------------------------------------------------
//vector<string> ofxOscRouterBaseNode::getOscMethods() const {
//    return oscMethods.toArray();
//}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::hasOscMethod(const string& _method) const {
    return !_method.empty() && oscMethods.find(_method) != oscMethods.end();
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::addOscMethod(const string& _method) {
    return oscMethods.insert(_method).second;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::removeOscMethod(const string& _command) {
    if(hasOscMethod(_command)) {
        oscMethods.erase(_command);
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::isNodeActive() const {
    return bNodeActive;
}

//--------------------------------------------------------------
void ofxOscRouterBaseNode::setNodeActive(bool _bNodeEnabled) {
    bNodeActive = _bNodeEnabled;
}

//--------------------------------------------------------------
//bool ofxOscRouterBaseNode::addOscPlugMethod(string _method, NoArgPlugFunc func) {
//    if(!hasOscMethod(_method) && addOscMethod(_method)) {
//        noArgPlugFuncMap[_method] = func;
//        return true;
//    } else {
//        return false;
//    }
//}

//--------------------------------------------------------------
//bool ofxOscRouterBaseNode::addOscPlugMethod(string _method, OscMessagePlugFunc func) {
//    if(!hasOscMethod(_method) && addOscMethod(_method)) {
//        oscMessagePlugFuncMap[_method] = func;
//        return true;
//    } else {
//        return false;
//    }
//}


////--------------------------------------------------------------
//bool ofxOscRouterBaseNode::hasOscPlugMethod(string _method) {
//    
//    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
//    if(noArgPlugFuncMapIter != noArgPlugFuncMap.end()) {
//        return true;
//    } 
//    
//    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
//    if(oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()) {
//        return true;
//    }
//    
//    return false;
//}

////--------------------------------------------------------------
//bool ofxOscRouterBaseNode::removeOscPlugMethod(string _method) {
//    noArgPlugFuncMap.erase(_method);
//    oscMessagePlugFuncMap.erase(_method);
//    return true;
//}

////--------------------------------------------------------------
//bool ofxOscRouterBaseNode::executeOscPlugMethod(string _method, const ofxOscMessage& m) {
//
//    bool foundOne = false;
//    
//    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
//    if(noArgPlugFuncMapIter != noArgPlugFuncMap.end()) {
//        noArgPlugFuncMapIter->second();
//        foundOne = true;
//    } 
//    
//    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
//    if(oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()) {
//        oscMessagePlugFuncMapIter->second(m);
//        foundOne = true;
//    }
//
//    return foundOne;
//}


//--------------------------------------------------------------
string ofxOscRouterBaseNode::normalizeOscNodeName(const string& name) {
    string theName = name;
    RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    int numReplaced = re.subst(theName, "_", RegularExpression::RE_GLOBAL);
    return theName;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::isValidOscNodeName(const string& name) {
    // TODO ... ugly.
    string theName = name;
    RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    return re.subst(theName, "_", RegularExpression::RE_GLOBAL) <= 0;
}

//--------------------------------------------------------------
string ofxOscRouterBaseNode::normalizeOscCommand(const string& name) {
    string theName = name;
    RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    int numReplaced = re.subst(theName, "_", RegularExpression::RE_GLOBAL);
    return theName;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::isValidOscCommand(const string& name) {
    // TODO ... ugly.
    string theName = name;
    RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    return re.subst(theName, "_", RegularExpression::RE_GLOBAL) <= 0;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::isMatch(const string& s0, const string& s1) {
    return icompare(s0,s1) == 0;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::getArgAsBoolean(const ofxOscMessage& m, int index) {
    if(index < m.getNumArgs()) {
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
float ofxOscRouterBaseNode::getArgAsFloatUnchecked(const ofxOscMessage& m, int index) {
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
int ofxOscRouterBaseNode::getArgAsIntUnchecked(const ofxOscMessage& m, int index) {
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
string ofxOscRouterBaseNode::getArgAsStringUnchecked(const ofxOscMessage& m, int index) {
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
ofColor ofxOscRouterBaseNode::getArgsAsColor(const ofxOscMessage& m, int startIndex) {
    ofColor color;
    if(startIndex < m.getNumArgs()) {
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
ofPoint ofxOscRouterBaseNode::getArgsAsPoint(const ofxOscMessage& m, int startIndex) {
    ofPoint point;
    if(startIndex < m.getNumArgs()) {
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
vector<float> ofxOscRouterBaseNode::getArgsAsFloatArray(const ofxOscMessage& m, int index) {
    vector<float> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsFloatUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<int> ofxOscRouterBaseNode::getArgsAsIntArray(const ofxOscMessage& m, int index) {
    vector<int> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsIntUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
vector<string> ofxOscRouterBaseNode::getArgsAsStringArray(const ofxOscMessage& m, int index) {
    vector<string> array;
    for(int i = index; i < m.getNumArgs(); i++) array.push_back(getArgAsStringUnchecked(m,i));
    return array;
}

//--------------------------------------------------------------
bool ofxOscRouterBaseNode::validateOscSignature(const string& signature, const ofxOscMessage& m) {
    
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