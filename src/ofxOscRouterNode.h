//
//  ofxOscRouterNodeAliased.h
//  ofxLivedrawEngineExample
//
//  Created by Christopher Baker on 7/13/12.
//  Copyright (c) 2012 School of the Art Institute of Chicago. All rights reserved.
//

#pragma once

#include "ofxOscRouterBaseNode.h"

class ofxOscRouterNode : public ofxOscRouterBaseNode {
public:
    ofxOscRouterNode();
    ofxOscRouterNode(const string& _nodeName);
    virtual ~ofxOscRouterNode();

    virtual void processOscCommand(const string& command, const ofxOscMessage& m) = 0;

    set<string>& getOscNodeAliasesRef();
    const set<string>& getOscNodeAliasesRef() const;

    bool hasOscAliases() const;
    
    string getFirstOscNodeAlias() const; // get the first one -- may not always be the same

    bool hasOscNodeAlias(const string& _oscNodeAlias) const;
    bool addOscNodeAlias(const string& _oscNodeAlias);
    bool removeOscNodeAlias(const string& _oscNodeAlias);
    void clearOscNodeAliases();
    
protected:

    set<string> oscNodeNameAliases;

};
