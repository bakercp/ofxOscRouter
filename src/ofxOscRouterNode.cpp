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
    return oscNodeNameAliases.insert(_oscNodeAlias).second;
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
