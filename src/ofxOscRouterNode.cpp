//
//  ofxOscRouterNodeAliased.cpp
//  ofxLivedrawEngineExample
//
//  Created by Christopher Baker on 7/13/12.
//  Copyright (c) 2012 School of the Art Institute of Chicago. All rights reserved.
//

#include "ofxOscRouterNode.h"

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode() {}

//--------------------------------------------------------------
ofxOscRouterNode::ofxOscRouterNode(const string& _nodeName) {
    addOscNodeAlias(_nodeName);
}

//--------------------------------------------------------------
ofxOscRouterNode::~ofxOscRouterNode() {}

//--------------------------------------------------------------
set<string>& ofxOscRouterNode::getOscNodeAliasesRef() {
    return oscNodeNameAliases;
}

//--------------------------------------------------------------
const set<string>& ofxOscRouterNode::getOscNodeAliasesRef() const {
    return oscNodeNameAliases;
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscAliases() const {
    return !oscNodeNameAliases.empty();
}

//--------------------------------------------------------------
string ofxOscRouterNode::getFirstOscNodeAlias() const {
    if(hasOscAliases()) {
        return *oscNodeNameAliases.begin();
    } else {
        return "NO ALIASES";
    }
}

//--------------------------------------------------------------
bool ofxOscRouterNode::hasOscNodeAlias(const string& _oscNodeAlias) const {
    return oscNodeNameAliases.find(_oscNodeAlias) != oscNodeNameAliases.end();
}

//--------------------------------------------------------------
bool ofxOscRouterNode::addOscNodeAlias(const string& _oscNodeAlias) {
    if(hasChildWithAlias(_oscNodeAlias)) {
        ofLogWarning() << "ofxOscRouterNode::addOscNodeAlias : oscChild Node already exists : " << _oscNodeAlias;
        return false;
    }
    if(oscNodeNameAliases.insert(_oscNodeAlias).second) {
        return true;
    } else {
        ofLogWarning() << "ofxOscRouterNode::addOscNodeAlias : oscNodeAlias already exists : " << _oscNodeAlias;
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOscRouterNode::removeOscNodeAlias(const string& _oscNodeAlias) {
    if(hasOscNodeAlias(_oscNodeAlias)) {
        oscNodeNameAliases.erase(_oscNodeAlias);
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
void ofxOscRouterNode::clearOscNodeAliases() {
    oscNodeNameAliases.clear();
}
