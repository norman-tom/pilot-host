#include "URL.hpp"

using namespace nectar::core;

URL::URL(urlparts &parts)
{
    const string d = "/";
    string urlStr = "";
    _parts = parts;

    Level l0 = *(parts.device);
    Level l1 = *(parts.module);
    Level l2 = *(parts.node);
    Level l3 = *(parts.tag);

    //Build the URL from the Parts
    #define add urlStr = urlStr + d + (string)

    if(l0 != Level::NONE)
        add *(parts.device);
    if(l1 != Level::NONE)
        add *(parts.module);
    if(l2 != Level::NONE)
        add *(parts.node);
    if(l3 != Level::NONE)
        add *(parts.tag);
    
    _URLString = urlStr;
}

urlparts URL::getParts() const 
{ 
    return _parts; 
}

void URL::setRun()
{
    _run = true;
}

void URL::setRead()
{
    _read = true;
}

void URL::setWrite()
{
    _write = true;
}

bool URL::isRunnable() const
{
    return _run;
}
bool URL::isReadable() const 
{
    return _read;
}
bool URL::isWriteable() const
{
    return _write;
}

bool URLPart::operator== (URLPart const &obj) const
{
    bool nameFlag = false;
    bool machineFlag = false;
    bool levelFlag = false;

    if(_string.compare(obj) == 0)
        nameFlag = true;
    if(_machine == obj._machine)
        machineFlag = true;
    if(_level == obj._level)
        levelFlag = true;

    return (nameFlag && machineFlag && levelFlag);
}