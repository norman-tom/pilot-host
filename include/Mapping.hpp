#ifndef MAPPING_H
#define MAPPING_H

#include <unordered_map>
#include "URL.hpp"
#include <memory>

using namespace std;

namespace nectar::core
{
    class Mapping
    {
        private:
            unordered_map<string, URL> _URLs;

        
        public:
            void add(URL url);
            
            /**
             * @brief Retrieves a URL from a url string.
             * @param url A url string which has been mapped to the URL
             * @return A URL object. Throws out_out_of_range exception if string is not a valid URL.
             * @throws out_of_range exception
             */
            URL const &retrieve(string const &url) const;
            
            /**
             * @brief Return the machine code for the higest level part of the URL. Must be a valid URL built from the MappingBuilder otherwise will throw out_of_range exception
             * @param url a url string to find the machine code for
             * @return The mapped machine code. Throws out_of_range exception if url string is not valid
             */
            unsigned int machine(string const &url) const; 
    };
}

#endif