#include "topography/MapTree.hpp"

using namespace nectar::topography;


MapTree::MapTree()
{
    _start = new MapNode("root", 0, Level::ROOT);
    _end = _start;
}

MapNode *MapTree::newChild(MapNode *parent, const string &name, bool run, bool read, bool write)
{
    //this assumes the child was a new node, may return and existing node which shouldnt be modified
    MapNode *child;
    child = parent->newChild(name, run, read, write);
    
    if((child->_next == nullptr) && (_end != child))
    {
        _end->_next = child;        //previous end now is behind the new node
        _end = child;               //end of the linked list is now the child
    }

    return child;
}

MapNode *MapTree::getRoot()
{
    return _start;
}

MapTree::iterator MapTree::begin()
{
    return iterator(_start);
}

MapTree::iterator MapTree::end()
{
    return iterator(nullptr);
}

MapTree::~MapTree()
{
    //clean up all the created nodes when the tree is destoryed
    MapNode *current = _start;
    MapNode *next;

    while(current != nullptr)
    {
        next = current->_next;
        delete current;
        current = next;        
    }
}