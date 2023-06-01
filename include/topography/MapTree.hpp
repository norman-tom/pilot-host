#ifndef MAP_TREE_H
#define MAP_TREE_H

#include "topography/MapNode.hpp"
#include <iterator>

namespace nectar::topography
{
    class MapTree
    {
        private:
            MapNode *_start;
            MapNode *_end;

        public:
            MapTree();
            ~MapTree();
            MapNode *newChild(MapNode * parent, const string &name, bool run = false, bool read = false, bool write = false);
            MapNode *getRoot();

            struct iterator
            {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = MapNode;
                using pointer = MapNode*;
                using reference = MapNode&;

                iterator(pointer ptr) : _ptr(ptr) {}

                reference operator*() const { return *_ptr; }
                pointer operator->() { return _ptr; }
                iterator& operator++() { _ptr = _ptr->_next; return *this; }
                iterator operator++(int) { iterator tmp = *this; _ptr = _ptr->_next; return tmp; }
                operator MapNode*() { return _ptr; }
                friend bool operator==(const iterator &a, const iterator &b) { return a._ptr == b._ptr; }
                friend bool operator!=(const iterator &a, const iterator &b) { return a._ptr != b._ptr; }

                private:
                    pointer _ptr;
            };

            iterator begin();
            iterator end();            
    };
}

#endif