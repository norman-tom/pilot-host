#include "Mapping.hpp"
#include <memory>
#include <iostream>


using namespace nectar::core;
using namespace std;

void Mapping::add(URL url)
{
    _URLs.insert(make_pair((string) url, url));
}

URL const &Mapping::retrieve(string const &url) const 
{
    try
    {
        return _URLs.at(url);
    }
    catch(const std::out_of_range& e)
    {
        throw out_of_range("invalid URL: " + url);
    }
}

unsigned int Mapping::machine(string const &url) const
{
    return *(retrieve(url).getParts().max);
}