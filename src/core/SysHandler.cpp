#include "SysHandler.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <cstring>

using namespace nectar::core;

SysHandler::SysHandler(void)
{
    
}

SysHandler::SysHandler(string basePath)
{
    SysHandler::basePath = basePath;
    SysHandler::parent = this;
}

SysHandler::SysHandler(string basePath, SysHandler *parent)
{
    SysHandler::basePath = basePath;
    SysHandler::parent = parent;
}

/* Copy constructor because of mutex */
SysHandler::SysHandler(const SysHandler &obj)
{
    basePath = obj.basePath;
    parent = obj.parent;    
}

int SysHandler::addSysNode(string attribute)
{
    struct sysNode mNode = {
        .fd = 0,
        .attribute = attribute,
        .name = attribute,
    };

    SysHandler::sysNodes.push_back(mNode);

    return SysHandler::sysNodeCount++;
}

int SysHandler::addSysNode(string attribute, string name)
{
    struct sysNode mNode = {
        .fd = 0,
        .attribute = attribute,
        .name = name,
    };

    SysHandler::sysNodes.push_back(mNode);

    return SysHandler::sysNodeCount++;
}

void SysHandler::removeSysNode(int index)
{
    if(SysHandler::sysNodes[index].fd > 0)
    {
        SysHandler::closeSysNode(index);
    }

    SysHandler::sysNodes.erase(SysHandler::sysNodes.begin() + index);

    SysHandler::sysNodeCount--;
}

int SysHandler::openSysNode(int index, int permissions)
{   
    int fd;
    string filePath;
    sysNode *mNode = &SysHandler::sysNodes[index];
    
    filePath = SysHandler::basePath + mNode->attribute;

    fd = open(filePath.c_str(), permissions);
    
    if(fd < 0)
    {
        cout << "unable to load directory: " << filePath << " error: " << fd << endl;
    }
    else
    {
        cout << "node opened: " << filePath << endl;
        mNode->fd = fd;
    }
    
    return fd;
}

int SysHandler::readSysNode(int index, char *buf, size_t nbytes)
{
    /* Lock so there is only one read accessed at the time */
    std::lock_guard<std::mutex> lock(SysHandler::mutex_);

    int ret;
    struct sysNode *mNode = &SysHandler::sysNodes[index];

    ret = read(mNode->fd, buf, nbytes);

    if(ret < 0){
        cout << "Error reading bytes, error: " << ret << endl;
    }

    lseek(mNode->fd, 0, SEEK_SET);

    return ret;
}

int SysHandler::writeSysNode(int index, const char *buf, size_t nbytes)
{
    /* Lock so there is only one write accessed at the time */
    std::lock_guard<std::mutex> lock(SysHandler::mutex_);

    int ret;
    struct sysNode *mNode = &SysHandler::sysNodes[index];

    ret = write(mNode->fd, buf, nbytes);

    if(ret < 0){
        cout << "Error writing bytes error: " << errno << endl;
    }

    return ret;
}

int SysHandler::closeSysNode(int index)
{
    int ret;
    ret = close(SysHandler::sysNodes[index].fd);
    SysHandler::sysNodes[index].fd = 0;
    return ret;
}

long SysHandler::updateNode(int nodeIndex, int base)
{
    char buf[32];
    size_t nbytes = sizeof(buf);
    std::memset(buf, 0, nbytes);
    
    //read the raw sys node value
    readSysNode(nodeIndex, buf, nbytes);
    
    //convert from string to a long
    return std::stoul((string) buf, &nbytes, base);
}