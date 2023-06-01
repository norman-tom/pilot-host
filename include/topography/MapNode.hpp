#ifndef MAP_NODE_H
#define MAP_NODE_H

#include <string>
#include <memory>
#include <set>
#include "URL.hpp"

using namespace std;

namespace nectar::topography
{
    class MapNode
    {
        private:
            MapNode* _parent;
            MapNode* _next;
            vector<MapNode*> _children;
            string _name;
            unsigned int _machine;
            Level _level;
            unsigned int _degree = 0;

            //if any of these are set, the url will need to be created to accept commands
            bool _run = false;
            bool _read = false;
            bool _write = false;

        protected:
            MapNode* newChild(string const &name, bool run = false, bool read = false, bool write = false);

        public:
            /**
             * @param name the name of the node
             * @param machine the machine ID of the node
             * @param lvl the level of the node 
             */
            MapNode(string const &name, unsigned int machine, Level lvl);
            /**
             * @param parent pointer to the nodes parent
             * @param name the name of the node
             * @param machine the machine ID of the node
             * @param lvl the level of the node 
             */
            MapNode(MapNode* parent, string const &name, unsigned int machine, Level lvl);
            string getName() const;
            unsigned int getMachine() const;
            Level getLevel() const;
            MapNode* getParent() const;
            bool isRunnable();
            bool isReadable();
            bool isWriteable();
            bool operator ==(const MapNode &obj) const;
            bool operator <(const MapNode &obj) const;

            friend struct hash<MapNode>;
            friend class MapTree;
    };
    
}

namespace std
{
    using namespace nectar::topography;

    template<>
    struct hash<MapNode>
    {
        size_t operator()(const MapNode &obj) const 
        {
            return hash<string>()(obj._name);
        }
    };
}

#endif