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


#include "ofxOscRouterNode.h"


ofxOscRouterNode::ofxOscRouterNode()
{
}


ofxOscRouterNode::ofxOscRouterNode(const std::string& _nodeName)
{
    addOscNodeAlias(_nodeName);
}


ofxOscRouterNode::~ofxOscRouterNode()
{
}


std::set<std::string>& ofxOscRouterNode::getOscNodeAliases()
{
    return oscNodeNameAliases;
}


const std::set<std::string>& ofxOscRouterNode::getOscNodeAliases() const
{
    return oscNodeNameAliases;
}


bool ofxOscRouterNode::hasOscAliases() const
{
    return !oscNodeNameAliases.empty();
}


std::string ofxOscRouterNode::getFirstOscNodeAlias() const
{
    if (hasOscAliases())
    {
        return *oscNodeNameAliases.begin();
    }
    else
    {
        return "NO ALIASES";
    }
}


bool ofxOscRouterNode::hasOscNodeAlias(const std::string& _oscNodeAlias) const
{
    return oscNodeNameAliases.find(_oscNodeAlias) != oscNodeNameAliases.end();
}


bool ofxOscRouterNode::addOscNodeAlias(const std::string& _oscNodeAlias)
{
    if (hasChildWithAlias(_oscNodeAlias))
    {
        ofLogWarning() << "ofxOscRouterNode::addOscNodeAlias : oscChild Node already exists : " << _oscNodeAlias;
        return false;
    }

    if (oscNodeNameAliases.insert(_oscNodeAlias).second)
    {
        return true;
    }
    else
    {
        ofLogWarning() << "ofxOscRouterNode::addOscNodeAlias : oscNodeAlias already exists : " << _oscNodeAlias;
        return false;
    }
}


bool ofxOscRouterNode::removeOscNodeAlias(const string& _oscNodeAlias)
{
    if(hasOscNodeAlias(_oscNodeAlias))
    {
        oscNodeNameAliases.erase(_oscNodeAlias);
        return true;
    }
    else
    {
        return false;
    }
}


void ofxOscRouterNode::clearOscNodeAliases()
{
    oscNodeNameAliases.clear();
}
