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


#include "ofxOsc.h"
#include "ofxOscRouterNode.h"


class ofxOscRouter: public ofxOscRouterNode
{
public:
	ofxOscRouter();
	ofxOscRouter(const std::string& rootNodeName, int listenerPort);
	virtual ~ofxOscRouter();
    
    void setup(const std::string& rootNodeName, int listenerPort);
    void update();
    void setPort(int port);
    
    void processOscCommand(const std::string& command, const ofxOscMessage& m);

    ofxOscReceiver& getOscReceiver();

    //bool plug(string nodeAddress, ofxOscRouterBaseNode* node);
    //bool plug(string nodeAddress, string method, ofxOscRouterBaseNode* node, Function, function)
    
    // bool unplug(string nodeAddress);
    // bool unplug(string nodeAddress);
    
    
//    string dumpSchema() {
//    }
    
protected:

    //map<string, ofxOscRouterBaseNode*> plugMap;
    
	ofxOscReceiver	receiver;  // TODO: remove this from the router
                               // to separate the two functions
                               // for more flexibility.
	
};

