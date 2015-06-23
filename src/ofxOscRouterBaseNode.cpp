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


#include "ofxOscRouterBaseNode.h"


bool stringSortNormal        (const std::string& s0, const std::string& s1)
{ return (s0 < s1); }
bool stringSortReverse       (const std::string& s0, const std::string& s1)
{ return (s0 > s1); }
bool stringSortLengthNormal  (const std::string& s0, const std::string& s1)
{ return (s0.length() < s1.length() ); }
bool stringSortLengthReverse (const std::string& s0, const std::string& s1)
{ return (s0.length() > s1.length() ); }


ofxOscRouterBaseNode::ofxOscRouterBaseNode():
    bNodeActive(true)
{
}


//ofxOscRouterBaseNode::ofxOscRouterBaseNode(ofxOscRouterBaseNode* _oscParent, std::string _oscNodeName){
//    addOscParent(_oscParent);
//    addOscNodeAlias(_oscNodeName);
//}


ofxOscRouterBaseNode::~ofxOscRouterBaseNode()
{
}


void ofxOscRouterBaseNode::routeOscMessage(const string& pattern, ofxOscMessage& m)
{
    if (!isNodeActive())
        return; // bail if this node is active
    
    
    //cout << "routing=>" << pattern << " / " << m.getAddress() << " and have " << oscChildren.size() << " oscChildren" << endl;
    
//// For debugging.

    int level = 0;
    std::string tabs = "";
    std::string fullAddress = m.getAddress();
    std::string patternAddress = pattern;
    
    std::string::size_type offset = fullAddress.find(patternAddress, 0);
    if (string::npos != offset)
    {
        level = std::count(fullAddress.begin(), fullAddress.begin() + offset, '/');
        tabs = std::string(level,'\t');
    }
    else
    {
        level = 0;
    }
    
    
    int pattrOffset = 0;
    int addrOffset  = 0;
    int matchResult = OSC_MATCH_NONE;
    
    std::string matchedNodeAlias = "";
    
    char* _pattern = (char*)pattern.c_str();
    
    // check our aliases
    std::set<std::string>& aliases = getOscNodeAliases();
    
    //cout << "found " << aliasesRef.size() << " aliases " << endl;
    
    if (!aliases.empty())
    {
        //matchResult == 0){
        // this reversal is to prevent overlap of aliases.  
        // an alias list like /l /live /lives will match /lives on /l and we want
        // it to match the largest one.  So we sort them in reverse alphabetical order.
        //sort(aliases.begin(), aliases.end(), stringSortLengthReverse); // sort 

//        vector<string>::iterator aliasIter;
        std::set<string>::reverse_iterator aliasIter = aliases.rbegin();
        
        //int xxx = 0;

        while (aliasIter != aliases.rend())
        {
            //cout << "\t\t\t\t\t\t #aliases=" << aliasesRef.size() <<  " index = " << xxx << endl;
            //xxx++;
        
            std::string alias = (*aliasIter);
            ofLogVerbose("ofxOscRouterBaseNode") << tabs << "alias /" << alias << " match? : " << pattern;

            //string alias = aliases[i];
            pattrOffset = 0;
            addrOffset  = 0;
            matchResult = 0;
            char* _thisAlias = (char*)("/" + alias).c_str();
            matchResult = osc_match(_pattern, _thisAlias, &pattrOffset, &addrOffset);

            if (OSC_MATCH_NONE != matchResult)
            {
                matchedNodeAlias = alias;
                break;
            }

            ++aliasIter;
        }
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << tabs << "The node " << getFirstOscNodeAlias() << " has no aliases: " << m.getAddress();
    }

    if (OSC_MATCH_NONE == matchResult)
    {
        ofLogVerbose("ofxOscRouterBaseNode") << tabs << getFirstOscNodeAlias() << " don't match for " << m.getAddress();
        return;
    }
    else
    {
        //cout << "evaluating results == pattern = " << pattern << endl;
        
        if (OSC_MATCH_ADDRESS_AND_PATTERN_COMPLETE == matchResult)
        {
            // an exact match for this node and there is no method to try.
            processOscCommand(pattern, m);
        }
        else if (OSC_MATCH_PATTERN_COMPLETE == matchResult)
        {
            // the pattern matched the addres, but the address still has nodes left.  
            // pass it on. to be processed by this node.  maybe something clever is going on.  
            processOscCommand(pattern, m);
        }
        else if (OSC_MATCH_ADDRESS_COMPLETE == matchResult)
        {
            // extract the updated pattern 
            std::string remainingPattern = pattern.substr(pattrOffset);
            std::string command = remainingPattern;

            // prepare the command for testing
            if (0 == command.find("/"))
            {
                command = command.substr(1); // remove slash
            }
            
            // if it is a command, then process it
            if (isValidOscCommand(command))
            {
                processOscCommand(command, m);
            }
            else
            {
                // if it is not a command, try to pass it on up the chain
                //vector<ofxOscRouterBaseNode*> children = oscChildren.toArray();
                
                //cout << "Passing it up the chain and has " << oscChildren.size() << " oscChildren" << endl;
                
                std::set<ofxOscRouterBaseNode*>::iterator childIter = oscChildren.begin();
//                for (childIter = oscChildren.begin(); childIter != oscChildren.end(); childIter++) {
//                    cout << " : " <<  (*childIter)->getFirstOscNodeAlias() << endl;
//                }

                //cout << "----" << endl;
                
                while (childIter != oscChildren.end())
                {
                    if (*childIter)
                    {
                        //cout << " : " <<  (*childIter)->getFirstOscNodeAlias() << endl;
                        // TODO: when killing off children, MUST SET BACK TO NULL
                        // TODO: what happens if routing a message DESTROYS one of the children in the vector?
                        (*childIter)->routeOscMessage(remainingPattern, m);
                    }

                    ++childIter;
                }
                
                //cout << "Done passing it up the chain." << endl;
            }
        }
        else
        {
            ofLogError("ofxOscRouterBaseNode") << tabs << "Unknown osc_match result.";
        }
    }
}


bool ofxOscRouterBaseNode::hasParents() const
{
    return !oscParents.empty();
}


ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscParent()
{
    if (hasParents())
    {
        return *oscParents.begin();
    }
    else
    {
        return 0;
    }
}


const ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscParent() const
{
    if (hasParents())
    {
        return *oscParents.begin();
    }
    else
    {
        return 0;
    }
}



ofxOscRouterBaseNode* ofxOscRouterBaseNode::getLastOscParent()
{
    if (hasChildren())
    {
        return *oscChildren.rbegin();
    }
    else
    {
        return 0;
    }
}


const ofxOscRouterBaseNode* ofxOscRouterBaseNode::getLastOscParent() const
{
    if (hasChildren())
    {
        return *oscChildren.rbegin();
    }
    else
    {
        return 0;
    }
}


//vector<ofxOscRouterBaseNode*> ofxOscRouterBaseNode::getOscParents() const {
//    return oscParents.toArray();
//}


bool ofxOscRouterBaseNode::hasOscParent(ofxOscRouterBaseNode* _oscParent) const
{
    return _oscParent != 0 &&
            oscParents.find(_oscParent) != oscParents.end();
}


bool ofxOscRouterBaseNode::addOscParent(ofxOscRouterBaseNode* _oscParent)
{
    return oscParents.insert(_oscParent).second;
}


bool ofxOscRouterBaseNode::removeOscParent(ofxOscRouterBaseNode* _oscParent)
{
    if (hasOscParent(_oscParent))
    {
        oscParents.erase(_oscParent);
        return true;
    }
    else
    {
        return false;
    }
}


//vector<ofxOscRouterBaseNode*> ofxOscRouterBaseNode::getOscSiblings() const {
//    vector<ofxOscRouterBaseNode*> oscSiblings;
//    if (!oscParents.isEmpty()) {
//        vector<ofxOscRouterBaseNode*> parents = oscParents.toArray();
//        for(int i = 0; i < parents.size(); i++) {
//            vector<ofxOscRouterBaseNode*> children = parents[i]->getOscChildren();
//            for(int j = 0; j < children.size(); j++) {
//                if (children[j] != this) {
//                    oscSiblings.push_back(children[j]);
//                }
//            }
//        }
//        return oscSiblings;
//    } else {
//        return oscSiblings;
//    }
//}


// recursively locate the root node
ofxOscRouterBaseNode* ofxOscRouterBaseNode::getOscRoot()
{
    ofxOscRouterBaseNode* parent = getFirstOscParent();
    return parent ? parent->getOscRoot() : parent;
}


std::string ofxOscRouterBaseNode::getFirstOscNodeAlias() const
{
    const std::set<std::string>& aliasesRef = getOscNodeAliases();

    if (!aliasesRef.empty())
    {
        return *aliasesRef.begin();
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "getFirstOscNodeAlias(): No Aliases!";
        return "NO ALIASES!";
    }
}


std::string ofxOscRouterBaseNode::getLastOscNodeAlias() const
{
    const std::set<std::string>& aliasesRef = getOscNodeAliases();

    if (!aliasesRef.empty())
    {
        return *aliasesRef.rbegin();
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "getLastOscNodeAlias() No Aliases!";
        return "NO ALIASES!";
    }
}


bool ofxOscRouterBaseNode::hasOscNodeAlias(const std::string& alias) const
{
    const std::set<std::string>& aliasesRef = getOscNodeAliases();
    return aliasesRef.find(alias) != aliasesRef.end();
}


// currently broken
/*
 void getFullOscNodeAddress(string &fullAddress)
{
 // if it hasn't been cached yet
 if (parent != NULL)
{
 getFullOscNodeAddress(fullAddress);
 } else {
 // nothing
 }
 fullAddress = nodeName + fullAddress;
 }
 */



bool ofxOscRouterBaseNode::hasChildren() const
{
    return !oscChildren.empty();
}


bool ofxOscRouterBaseNode::hasChildWithAlias(const std::string& name) const
{
    set<ofxOscRouterBaseNode*>::const_iterator constNodeIter = oscChildren.begin();

    while (constNodeIter != oscChildren.end())
    {
        if ((*constNodeIter)->hasOscNodeAlias(name))
        {
            return true;
        }

        ++constNodeIter;
    }

    return false;
}


bool ofxOscRouterBaseNode::hasChildWithAlias(const std::set<std::string>& theOtherAliases) const
{
    std::string clashName = "";
    return hasChildWithAlias(theOtherAliases, clashName);
}


bool ofxOscRouterBaseNode::hasChildWithAlias(const std::set<std::string>& theOtherAliases,
                                             std::string& clashName) const
{
    // seems like this could be done with set_intersection ...
    std::set<ofxOscRouterBaseNode*>::iterator it;
    std::set<string>::iterator theOtherIterator;

   // cout << "I have -> " << oscChildren.size() << " children and need to check ";
   // cout <<  theOtherAliases.size() << " incoming aliases." << endl;

    if (oscChildren.empty())
    {
        return false;
    }

    if (theOtherAliases.empty())
    {
        return false;
    }

    for (it = oscChildren.begin();
         it != oscChildren.end();
         it++)
    {
        
        ofxOscRouterBaseNode* theChild = (*it);
        
        for (theOtherIterator = theOtherAliases.begin();
             theOtherIterator != theOtherAliases.end();
             theOtherIterator++)
        {
            
            if (theChild->hasOscNodeAlias(*theOtherIterator))
            {
                clashName = *theOtherIterator; // return it to see what it was.
                return true;
            }
        }
    }

    return false;
}


ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscChild()
{
    if (hasChildren())
    {
        return *oscChildren.begin();
    }
    else
    {
        return 0;
    }
}


const ofxOscRouterBaseNode* ofxOscRouterBaseNode::getFirstOscChild() const
{
    if (hasChildren())
    {
        return *oscChildren.begin();
    }
    else
    {
        return 0;
    }
}


std::set<ofxOscRouterBaseNode*>& ofxOscRouterBaseNode::getOscChildren()
{
    return oscChildren;
}


const std::set<ofxOscRouterBaseNode*>& ofxOscRouterBaseNode::getOscChildren() const
{
    return oscChildren;
}


bool ofxOscRouterBaseNode::hasOscChild(ofxOscRouterBaseNode* oscChild) const
{
    return oscChild != 0 && oscChildren.find(oscChild) != oscChildren.end();
}


bool ofxOscRouterBaseNode::ofxOscRouterBaseNode::addOscChild(ofxOscRouterBaseNode* oscChild)
{
    if (!oscChild)
{
        ofLogError("ofxOscRouterBaseNode") << "addOscChild: Failed to add OSC Child.  Child was NULL.";
        return false;
    }

    const std::set<std::string>& proposedChildAliases = oscChild->getOscNodeAliases();
    
    std::string clashName = "";

    if (hasChildWithAlias(proposedChildAliases,clashName))
    {
        std::string thisClash = getFirstOscPath() + clashName;
        std::string thatClash = ".../" + clashName;
        ofLogError("ofxOscRouterBaseNode") << "addOscChild: Failed to add OSC Child.  Name clash with existing node child or node alias.";
        ofLogError("ofxOscRouterBaseNode") << setw(4) << "Existing Node: " << thisClash;
        
        std::set<std::string>::const_iterator iter;
        
        ofLogError() << setw(4) << "Attempted Node: " << thatClash;

        for(iter = proposedChildAliases.begin();
            iter != proposedChildAliases.end();
            iter++)
        {
            ofLogError("ofxOscRouterBaseNode") << setw(4) << "Additional Node Aliase: " << *iter;
        }

        return false;
    }    
    
    if (oscChildren.insert(oscChild).second)
    {
        oscChild->addOscParent(this);
        return true;
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "addOscChild: Failed to add OSC Child.  Already had the same child.";
        return false;
    }
}


bool ofxOscRouterBaseNode::removeOscChild(ofxOscRouterBaseNode* oscChild)
{
    if (hasOscChild(oscChild))
    {
        oscChild->removeOscParent(this);
        oscChildren.erase(oscChild);
        return true;
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "removeOscChild: Failed to remove OSC Child.  Child was NULL.";
        return false;
    }
}


std::set<std::string>& ofxOscRouterBaseNode::getOscMethods()
{
    return oscMethods;
}


const std::set<std::string>& ofxOscRouterBaseNode::getOscMethods() const
{
    return oscMethods;
}


bool ofxOscRouterBaseNode::hasOscMethod(const std::string& _method) const
{
    return !_method.empty() && oscMethods.find(_method) != oscMethods.end();
}


bool ofxOscRouterBaseNode::addOscMethod(const std::string& _method)
{
    bool success = oscMethods.insert(_method).second;
    if (!success) ofLogWarning("ofxOscRouterBaseNode") << "addOscMethod : method=" <<  _method << " already exists.";
    return success;
}


bool ofxOscRouterBaseNode::removeOscMethod(const std::string& _command)
{
    if (hasOscMethod(_command))
    {
        oscMethods.erase(_command);
        return true;
    }
    else
    {
        return false;
    }
}


bool ofxOscRouterBaseNode::isNodeActive() const
{
    return bNodeActive;
}


void ofxOscRouterBaseNode::setNodeActive(bool _bNodeEnabled)
{
    bNodeActive = _bNodeEnabled;
}


std::string ofxOscRouterBaseNode::getFirstOscPath() const
{
    return (hasParents() ? getFirstOscParent()->getFirstOscPath() : "/" + getFirstOscNodeAlias() + "/");
}


std::string ofxOscRouterBaseNode::getLastOscPath() const
{
    return (hasParents() ? getLastOscParent()->getLastOscPath() : "/" + getLastOscNodeAlias() + "/");
}


//bool ofxOscRouterBaseNode::addOscPlugMethod(string _method, NoArgPlugFunc func) {
//    if (!hasOscMethod(_method) && addOscMethod(_method)) {
//        noArgPlugFuncMap[_method] = func;
//        return true;
//    } else {
//        return false;
//    }
//}


//bool ofxOscRouterBaseNode::addOscPlugMethod(string _method, OscMessagePlugFunc func){
//    if (!hasOscMethod(_method) && addOscMethod(_method)){
//        oscMessagePlugFuncMap[_method] = func;
//        return true;
//    } else {
//        return false;
//    }
//}


////------------------------------------------------------------------------------
//bool ofxOscRouterBaseNode::hasOscPlugMethod(string _method){
//    
//    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
//    if (noArgPlugFuncMapIter != noArgPlugFuncMap.end()){
//        return true;
//    } 
//    
//    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
//    if (oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()){
//        return true;
//    }
//    
//    return false;
//}

////------------------------------------------------------------------------------
//bool ofxOscRouterBaseNode::removeOscPlugMethod(string _method){
//    noArgPlugFuncMap.erase(_method);
//    oscMessagePlugFuncMap.erase(_method);
//    return true;
//}


//bool ofxOscRouterBaseNode::executeOscPlugMethod(string _method, const ofxOscMessage& m){
//
//    bool foundOne = false;
//    
//    noArgPlugFuncMapIter = noArgPlugFuncMap.find(_method);
//    if (noArgPlugFuncMapIter != noArgPlugFuncMap.end()){
//        noArgPlugFuncMapIter->second();
//        foundOne = true;
//    } 
//    
//    oscMessagePlugFuncMapIter = oscMessagePlugFuncMap.find(_method);
//    if (oscMessagePlugFuncMapIter != oscMessagePlugFuncMap.end()){
//        oscMessagePlugFuncMapIter->second(m);
//        foundOne = true;
//    }
//
//    return foundOne;
//}



string ofxOscRouterBaseNode::normalizeOscNodeName(const string& name)
{
    std::string theName = name;
    Poco::RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    /*int numReplaced = */re.subst(theName, "_", Poco::RegularExpression::RE_GLOBAL);
    return theName;
}


bool ofxOscRouterBaseNode::isValidOscNodeName(const std::string& name)
{
    // TODO ... ugly.
    std::string theName = name;
    Poco::RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    return re.subst(theName, "_", Poco::RegularExpression::RE_GLOBAL) <= 0;
}


std::string ofxOscRouterBaseNode::normalizeOscCommand(const std::string& name)
{
    std::string theName = name;
    Poco::RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    /*int numReplaced */re.subst(theName, "_", Poco::RegularExpression::RE_GLOBAL);
    return theName;
}


bool ofxOscRouterBaseNode::isValidOscCommand(const std::string& name)
{
    // TODO ... ugly.
    std::string theName = name;
    Poco::RegularExpression re("[ #*,/?\\[\\]\\{\\}]");
    return re.subst(theName, "_", Poco::RegularExpression::RE_GLOBAL) <= 0;
}


bool ofxOscRouterBaseNode::isMatch(const string& s0, const string& s1)
{
    return 0 == Poco::icompare(s0, s1);
}


bool ofxOscRouterBaseNode::getArgAsBoolUnchecked(const ofxOscMessage& m, int index)
{
    if (index >= 0 && index < m.getNumArgs())
    {
        ofxOscArgType argType = m.getArgType(index);

        if (argType == OFXOSC_TYPE_INT32)
        {
            return m.getArgAsInt32(index) > 0;
        }
        else if (argType == OFXOSC_TYPE_FLOAT)
        {
            return m.getArgAsFloat(index) > 0.0f;
        }
        else if (argType == OFXOSC_TYPE_STRING)
        {
            std::string str = m.getArgAsString(index);
            return isMatch(str,"true") ||
                   isMatch(str,"t")    ||
                   isMatch(str,"yes")  ||
                   isMatch(str,"y");
        }
        else
        {
            ofLogError("ofxOscRouterBaseNode") << "getArgAsBoolean: invalid arg type. returning false";
            return false;
        }
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "getArgAsBoolean: Index " << index << " out of bounds!";
        return false;
    }
}


float ofxOscRouterBaseNode::getArgAsFloatUnchecked(const ofxOscMessage& m, int index)
{
    if (index >= 0 && index < m.getNumArgs())
    {
        ofxOscArgType argType = m.getArgType(index);

        if (argType == OFXOSC_TYPE_INT32)
        {
            return (float)m.getArgAsInt32(index);
        }
        else if (argType == OFXOSC_TYPE_FLOAT)
        {
            return (float)m.getArgAsFloat(index);
        }
        else
        {
            ofLogError("ofxOscRouterBaseNode") << "getArgAsFloatUnchecked: invalid arg type. returning 0.0f";
            return 0.0f;
        }
    } else {
        ofLogError("ofxOscRouterBaseNode") << "getArgAsFloatUnchecked: Index " << index << " out of bounds!";
        return 0.0f;
    }
}


int ofxOscRouterBaseNode::getArgAsIntUnchecked(const ofxOscMessage& m, int index)
{
    if (index >= 0 && index < m.getNumArgs())
    {
        ofxOscArgType argType = m.getArgType(index);

        if (argType == OFXOSC_TYPE_INT32)
        {
            return m.getArgAsInt32(index);
        }
        else if (argType == OFXOSC_TYPE_FLOAT)
        {
            return m.getArgAsFloat(index);
        }
        else
        {
            ofLogError("ofxOscRouterBaseNode") << "getArgAsIntUnchecked: Unknown argType = " << argType;
            return 0;
        }
    }
    else
    {
        ofLogError("ofxOscRouterBaseNode") << "getArgAsIntUnchecked: Index " << index << " out of bounds!";
        return 0;
    }
}


string ofxOscRouterBaseNode::getArgAsStringUnchecked(const ofxOscMessage& m, int index)
{
    if (index >= 0 && index < m.getNumArgs())
{
        ofxOscArgType argType = m.getArgType(index);
        if (argType == OFXOSC_TYPE_INT32)
{
            return ofToString(m.getArgAsInt32(index));
        } else if (argType == OFXOSC_TYPE_FLOAT)
{
            return ofToString(m.getArgAsFloat(index));
        } else if (argType == OFXOSC_TYPE_STRING)
{
            return m.getArgAsString(index);
        } else {
            ofLogError("ofxOscRouterBaseNode") << "getArgAsStringUnchecked: invalid arg type. returning empty string.";
            return "";
        }
    } else {
        ofLogError("ofxOscRouterBaseNode") << "getArgAsIntUnchecked: Index " << index << " out of bounds!";
        return "";
    }
}


ofColor ofxOscRouterBaseNode::getArgsAsColor(const ofxOscMessage& m, int index)
{
    return getArgsAsColor(m,index,m.getNumArgs());
}


ofPoint ofxOscRouterBaseNode::getArgsAsPoint(const ofxOscMessage& m, int index)
{
    return getArgsAsPoint(m,index,m.getNumArgs());
}


ofColor ofxOscRouterBaseNode::getArgsAsColor(const ofxOscMessage& m, int startIndex, int endIndex)
{
    vector<float> args = getArgsAsFloatArray(m,startIndex,endIndex);
    ofColor color;
    if (args.size() < 1)
    {
        ofLogError("ofxOscRouterBaseNode") << "getArgsAsColor: no args to parse, returning 0 color.";
        return color;
    }
    else if (args.size() < 2)
    {
        color.set(args[0]);
        return color;
    }
    else if (args.size() < 3)
    {
        color.set(args[0],args[1]);
        return color;
    }
    else if (args.size() < 4)
    {
        color.set(args[0],args[1],args[2]);
        return color;
    }
    else
    {
        color.set(args[0],args[1],args[2],args[3]);
        if (args.size() > 4)
        {
            ofLogWarning("ofxOscRouterBaseNode") << "getArgsAsColor: more than 4 args for color, ignoring extras.";
        }
        return color;
    }
}


ofPoint ofxOscRouterBaseNode::getArgsAsPoint(const ofxOscMessage& m, int startIndex, int endIndex)
{
    std::vector<float> args = getArgsAsFloatArray(m, startIndex, endIndex);
    ofPoint point;
    if (args.size() < 1)
    {
        ofLogError("ofxOscRouterBaseNode") << "getArgsAsPoint: no args to parse, returning 0,0,0 point.";
        return point;
    }
    else if (args.size() < 2)
    {
        point.set(args[0]);
        return point;
    }
    else if (args.size() < 3)
    {
        point.set(args[0],args[1]);
        return point;
    }
    else
    {
        point.set(args[0],args[1],args[2]);

        if (args.size() > 3)
        {
            ofLogWarning("ofxOscRouterBaseNode") << "getArgsAsPoint: more than 3 args for point, ignoring extras.";
        }
        return point;
    }
}


std::vector<bool> ofxOscRouterBaseNode::getArgsAsBoolArray(const ofxOscMessage& m, int index, int endIndex)
{
    std::vector<bool> array;
    if (index < 0)
    {
        ofLogError("ofxOscRouterBaseNode") << m.getAddress() << ": Invalid index < 0";
        return array;
    }
    
    endIndex = MAX(endIndex, index);
    
    for (int i = index; i < endIndex; i++)
    {
        array.push_back(getArgAsBoolUnchecked(m, i));
    }
    
    return array;
}



std::vector<float> ofxOscRouterBaseNode::getArgsAsFloatArray(const ofxOscMessage& m, int index, int endIndex)
{
    std::vector<float> array;
    if (index < 0)
    {
        ofLogError("ofxOscRouterBaseNode") << m.getAddress() << ": Invalid index < 0";
        return array;
    }
    
    endIndex = MAX(endIndex, index);
    
    for (int i = index; i < endIndex; i++)
    {
        array.push_back(getArgAsFloatUnchecked(m, i));
    }
    
    return array;
}


std::vector<int> ofxOscRouterBaseNode::getArgsAsIntArray(const ofxOscMessage& m, int index, int endIndex)
{
    std::vector<int> array;
    if (index < 0)
    {
        ofLogError("ofxOscRouterBaseNode") << m.getAddress() << ": Invalid index < 0";
        return array;
    }
    
    endIndex = MAX(endIndex, index);
    
    for (int i = index; i < endIndex; i++)
    {
        array.push_back(getArgAsIntUnchecked(m, i));
    }
    
    return array;
}


std::vector<std::string> ofxOscRouterBaseNode::getArgsAsStringArray(const ofxOscMessage& m, int index, int endIndex)
{
    std::vector<std::string> array;

    if (index < 0)
    {
        ofLogError("ofxOscRouterBaseNode") << m.getAddress() << ": Invalid index < 0";
        return array;
    }
    
    endIndex = MAX(endIndex, index);
    
    for(int i = index; i < endIndex; i++)
    {
        array.push_back(getArgAsStringUnchecked(m, i));
    }
    
    return array;
}


std::vector<bool> ofxOscRouterBaseNode::getArgsAsBoolArray(const ofxOscMessage& m, int index)
{
    return getArgsAsBoolArray(m,index,m.getNumArgs());
}


std::vector<float> ofxOscRouterBaseNode::getArgsAsFloatArray(const ofxOscMessage& m, int index)
{
    return getArgsAsFloatArray(m,index,m.getNumArgs());
}


std::vector<int> ofxOscRouterBaseNode::getArgsAsIntArray(const ofxOscMessage& m, int index)
{
    return getArgsAsIntArray(m,index,m.getNumArgs());
}


std::vector<std::string> ofxOscRouterBaseNode::getArgsAsStringArray(const ofxOscMessage& m, int index)
{
    return getArgsAsStringArray(m,index,m.getNumArgs());
}


std::string ofxOscRouterBaseNode::getMessageAsString(const ofxOscMessage& m)
{
    std::stringstream ss;
    ss << m.getAddress() << ":";
    ss << m.getRemoteIp() << ":";
    ss << m.getRemotePort() << ":";
    ss << m.getNumArgs() << ":";
    // TODO: ?
    return ss.str();
}


bool ofxOscRouterBaseNode::validateOscSignature(const string& signature, const ofxOscMessage& m)
{
    
    stringstream mSignature;
    // make the signature
    for (int i = 0; i < m.getNumArgs(); ++i)
    {
        mSignature << m.getArgTypeName(i).at(0);
    }
    
    Poco::RegularExpression re(signature);
    bool match = re.match(mSignature.str());
    
    if (!match)
    {
        ofLogError("ofxOscRouterBaseNode") << "validateOscSignature: " << m.getAddress() << ": Signature: " << signature << " did not match mSignature= " << mSignature.str();
    }
    
    return match;
}
