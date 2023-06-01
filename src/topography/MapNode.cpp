#include "topography/MapNode.hpp"

using namespace nectar::topography;

MapNode::MapNode(string const &name, unsigned int machine, Level lvl)
{
    _parent = nullptr;
    _name = name;
    _machine = machine;
    _level = lvl;
}

MapNode::MapNode(MapNode* parent, string const &name, unsigned int machine, Level lvl)
{
    _parent = parent;
    _name = name;
    _machine = machine;
    _level = lvl;
}

MapNode* MapNode::newChild(string const &name, bool run, bool read, bool write)
{
    // Get the parent which is 'this' object
    MapNode *parent = this;

    // Get the next machine ID from the previous child
    unsigned int machine = _degree + 1;

    // Get the level from the parent (parent level + 1)
    Level lvl = _level;
    switch(lvl)
    {
        case(Level::NONE):
            lvl = Level::NONE;
            break;
        case(Level::ROOT):
            lvl = Level::DEVICE;
            break;
        case(Level::DEVICE):
            lvl = Level::MODULE;
            break;
        case(Level::MODULE):
            lvl = Level::NODE;
            break;
        case(Level::NODE):
            lvl = Level::TAG;
            break;
        case(Level::TAG):
            throw out_of_range("Maximum level exceeded");
        default:
            lvl = Level::NONE;
    }

    // Create the child
    MapNode *child = new MapNode(parent, name, machine, lvl);
    child->_run = run;
    child->_read = read;
    child->_write = write;

    // Check if the child already exist, if so return the child
    vector<MapNode*>::iterator itr = _children.begin();
    pair<set<shared_ptr<MapNode>>::iterator, bool> p;
    for(itr; itr != _children.end(); ++itr)
    {
        if(*child == *(*itr))
        {
            delete child;
            return *itr;
        }
    }

    // If doesnt exist, insert the object into the set of children
    child->_next = nullptr;
    _children.push_back(child);
    
    ++_degree;

    return child;
}

MapNode* MapNode::getParent() const
{
    return _parent;
}

string MapNode::getName() const
{
    return _name;
}

Level MapNode::getLevel() const
{
    return _level;
}

unsigned int MapNode::getMachine() const
{
    return _machine; 
}

bool MapNode::isRunnable()
{
    return _run;
}

bool MapNode::isReadable()
{
    return _read;
}

bool MapNode::isWriteable()
{
    return _write;
}

bool MapNode::operator ==(const MapNode &obj) const
{
    //Object is the same if
    //1. Has the same parent, and
    //2. Has the same name 

    bool flagParent = true;
    bool flagName = true;

    if(obj._parent != _parent)
        flagParent = false;

    if(obj._name.compare(_name) != 0)
        flagName = false;

    return (flagParent && flagName);
}

bool MapNode::operator <(const MapNode &obj) const
{
    //Node is less if its level is lower, or
    //if same level insertsion order  
    const Level objLvl = obj._level;

    if(_level < objLvl)
        return true;
    
    if(_level > objLvl)
        return false;

    if(_machine < obj._machine)
        return true;
    else
        return false;
}