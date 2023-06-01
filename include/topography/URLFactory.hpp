#ifndef URL_FACTORY_H
#define URL_FACTORY_H

#include "URL.hpp"
#include "Mapping.hpp"
#include "topography/MapNode.hpp"
#include "topography/MapTree.hpp"
#include <unordered_map>

using namespace nectar::core;

namespace nectar::topography
{
    class URLFactory
    {
        private:
            Mapping* _map;
            unordered_map<MapNode*, shared_ptr<URLPart>> _currentParts;
            
        public:
            URLFactory(Mapping *map);
            void produce(MapTree &tree);
    };
}

#endif