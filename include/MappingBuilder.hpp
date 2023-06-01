#ifndef MAPPING_BUILDER_H
#define MAPPING_BUILDER_H

#include "Mapping.hpp"
#include <memory>

namespace nectar::core
{
    class MappingBuilder
    {
        public:
            static Mapping build();
    };
}

#endif