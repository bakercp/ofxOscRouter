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


#include "ofxOscRouterBaseNode.h"


class ofxOscRouterNode: public ofxOscRouterBaseNode
{
public:
    ofxOscRouterNode();
    ofxOscRouterNode(const std::string& _nodeName);
    virtual ~ofxOscRouterNode();

    virtual void processOscCommand(const std::string& command, const ofxOscMessage& m) = 0;

    std::set<std::string>& getOscNodeAliases();

    const std::set<std::string>& getOscNodeAliases() const;

    bool hasOscAliases() const;
    
    std::string getFirstOscNodeAlias() const; // get the first one -- may not always be the same

    bool hasOscNodeAlias(const std::string& _oscNodeAlias) const;
    bool addOscNodeAlias(const std::string& _oscNodeAlias);
    bool removeOscNodeAlias(const std::string& _oscNodeAlias);
    void clearOscNodeAliases();
    
    void dumpOscNodeNameAlisases()
    {
        std::set<std::string>::iterator iter = oscNodeNameAliases.begin();
        while (iter != oscNodeNameAliases.end())
        {
            ofLogVerbose("dumpOscNodeNameAlisases") << *iter;
            ++iter;
        }
    }
    
protected:
    std::set<std::string> oscNodeNameAliases;

};
