#include "topography/URLFactory.hpp"
#include <vector>
#include <stdexcept>
#include <string.h>
#include <iostream>

using namespace nectar::topography;
using namespace std;

URLFactory::URLFactory(Mapping *map)
{
    _map = map;
}

void URLFactory::produce(MapTree &tree)
{
    //go through the tree and produce the URL parts
    pair<unordered_map<MapNode*, shared_ptr<URLPart>>::iterator, bool> p;
    shared_ptr<URLPart> part;
    MapNode *mNode, *start;
    MapTree::iterator itr = tree.begin();           //begining of the tree is the root node. Traverses in insert order. 
    bool canRun, canRead, canWrite;
    Level maxLevel;

    start = itr;

    for(itr; itr != tree.end(); ++itr)
    {
        urlparts parts;
        mNode = itr;

        canRun = itr->isRunnable();
        canRead = itr->isReadable();
        canWrite = itr->isWriteable();

        maxLevel = itr->getLevel();

        while(mNode != start)
        {
            part = make_shared<URLPart>(mNode->getMachine(), mNode->getName(), mNode->getLevel());
            pair<MapNode*, shared_ptr<URLPart>> in(mNode, part);
            p = _currentParts.insert(in);
            part = (*(p.first)).second;

            switch(mNode->getLevel())
            {
                case(Level::DEVICE):
                    parts.device = part;
                    break;
                case(Level::MODULE):
                    parts.module = part;
                    break;
                case(Level::NODE):
                    parts.node = part;
                    break;
                case(Level::TAG):
                    parts.tag = part;
                    break;
                default:
                    break;
            }
            mNode = mNode->getParent();
        }

        switch (maxLevel)
        {
            case Level::DEVICE:
                parts.max = parts.device;
                break;
            case Level::MODULE:
                parts.max = parts.module;
                break;
            case Level::NODE:
                parts.max = parts.node;
                break;
            case Level::TAG:
                parts.max = parts.tag;
                break;
            default:
                break;
        }

        URL url(parts);

        if(canRun) { url.setRun(); }

        if(canRead) { url.setRead(); }

        if(canWrite) { url.setWrite(); }

        _map->add(url);
    }
}
