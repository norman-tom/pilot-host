#include "Device.hpp"
#include "BrewModule.hpp"
#include "Process.hpp"
#include <typeinfo>
#include <array>
#include <err.h>

using namespace nectar::core;
using namespace nectar::hardware;

Device *Device::pInstance_{nullptr};
std::mutex Device::mutex_;

Device::Device()
{
    /* Add the null BrewMOdule and Processes to the list - This reserves these places and prevents the user from allocating these IDs */
    Device::newModule(BrewModule::MOD_NULL, "default");   
    Device::newControlProcess(Process::ID_NULL, nullptr);
}

Device *Device::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(pInstance_ == nullptr)
    {
        pInstance_ = new Device();
    }
    return pInstance_;
}

BrewModule *Device::newModule(unsigned int moduleID, std::string moduleDesc)
{
    BrewModule *ptrBm = new BrewModule(moduleID, moduleDesc);

    std::pair<std::map<unsigned int, BrewModule *>::iterator,bool> ret;
    ret = Device::brewModules.insert(std::pair<unsigned int, BrewModule *>(moduleID, ptrBm));

    /* If the insertion failed - possible duplication */
    if(!ret.second)
    {
        delete ptrBm;
        return nullptr;
    }

    return ptrBm;
}

BrewModule *Device::newModule(unsigned int moduleID, std::string moduleDesc, std::vector<std::pair<std::string, std::string>> nodes)
{
    /* Create a new Brew Module - Need to use the 'new' keyword as this creates a dynamic object which is not destroyed when function returns - dont use malloc, breaks readNode */
    BrewModule *ptrBm = new BrewModule(moduleID, moduleDesc, nodes);

    std::pair<std::map<unsigned int, BrewModule *>::iterator,bool> ret;
    ret = Device::brewModules.insert(std::pair<unsigned int, BrewModule *>(moduleID, ptrBm));

    /* If the insertion failed - possible duplication */
    if(!ret.second)
    {
        delete ptrBm;
        return nullptr;
    }

    return ptrBm;
}

int Device::removeModule(unsigned int moduleID)
{
    int ret = -1;
    std::map<unsigned int, BrewModule *>::iterator itr;

    if((itr = Device::brewModules.find(moduleID)) != Device::brewModules.end())
    {
        delete ((*itr).second);
        Device::brewModules.erase(itr);
        ret = 0;
    }

    return ret;
}

BrewModule *Device::getModule(unsigned int moduleID)
{
    std::map<unsigned int, BrewModule *>::iterator itr;

    if((itr = Device::brewModules.find(moduleID)) != Device::brewModules.end())
    {
        return (*itr).second;
    }

    return nullptr;
}

int Device::newControlProcess(unsigned int processID, Process *process)
{
    std::pair<std::map<unsigned int, Process *>::iterator, bool> ret;

    ret = Device::controlProcesses.insert(std::pair<unsigned int, Process *>(processID, process));

    /* If the insertion failed, possible duplication */
    if(!ret.second)
    {
        return -1;   
    }

    return 0;
}

void Device::delControlProcess(unsigned int processID)
{
    int ret = -1;
    
    std::map<unsigned int, Process *>::iterator itr;

    if((itr = Device::controlProcesses.find(processID)) != Device::controlProcesses.end())
    {
        Device::controlProcesses.erase(itr);
    }
}

Process *Device::getControlProcess(unsigned int processID)
{
    std::map<unsigned int, Process *>::iterator itr;

    if((itr = Device::controlProcesses.find(processID)) != Device::controlProcesses.end())
    {
        return (*itr).second;
    }

    return nullptr;
}

std::vector<unsigned int> Device::getAllControlProcesses()
{
    std::vector<unsigned int> processes;
    std::map<unsigned int, Process *>::iterator itr;

    for(itr = Device::controlProcesses.begin(); itr != Device::controlProcesses.end(); itr++)
    {
        processes.push_back(itr->first);
    }

    return processes;
}