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


#include "ofxOscRouter.h"


ofxOscRouter::ofxOscRouter()
{
}


ofxOscRouter::ofxOscRouter(const std::string& rootNodeName, int listenerPort)
{
    setup(rootNodeName, listenerPort);
}


ofxOscRouter::~ofxOscRouter()
{
}


void ofxOscRouter::setup(const std::string& rootNodeName, int port)
{
    addOscNodeAlias(rootNodeName);
    setPort(port);
	ofLogNotice("ofxOscRouter") << "Listening for osc messages on port " << port;
}


void ofxOscRouter::processOscCommand(const std::string& command, const ofxOscMessage& m)
{
    // the manager received a message
    ofLogNotice("ofxOscRouter") << "The router processed a message.";
}


void ofxOscRouter::update()
{
	// check for waiting messages
	while (receiver.hasWaitingMessages())
    {
		ofxOscMessage m;
		receiver.getNextMessage(&m); // fill the message
        routeOscMessage(m.getAddress(), m); // route the message
	}
}


void ofxOscRouter::setPort(int port)
{
    receiver.setup(port);
}


ofxOscReceiver& ofxOscRouter::getOscReceiver()
{
    return receiver;
}
