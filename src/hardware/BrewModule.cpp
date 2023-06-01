#include <fcntl.h>
#include <stdarg.h>
#include "BrewModule.hpp"
#include "Device.hpp"
#include "Controller.hpp"
#include "Process.hpp"

using namespace std;
using namespace nectar::hardware;
using namespace nectar::core;

BrewModule::BrewModule(unsigned int moduleID, string description)
{
    BrewModule::moduleID = moduleID;
    BrewModule::description = description;
}

BrewModule::BrewModule(unsigned int moduleID, string description, vector<pair<string, string>> nodes)
{
    vector<pair<string, string>>::iterator itr;

    int validNode;

    /* Get nodes which are controlled by this module */
    for(itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        validNode = isValidNode((*itr).second);

        switch(validNode)
        {
            case 0:
                break; //invalid node, keep going
            case 1:
                loadTempProbe(*itr);    //was a probe node
                break;
            case 2:
                loadLoadCell(*itr);     //was a load cell node
                break;
            case 3:
                loadSwitch(*itr);       //was a switch node
                break;
            default:
                break;
        }

    }

    BrewModule::moduleID = moduleID;
    BrewModule::description = description;
}

BrewModule::~BrewModule()
{
    unordered_map<string, LoadCellNode*>::iterator lItr = loadCells.begin();
    unordered_map<string, TemperatureNode*>::iterator tItr = tempSensors.begin();
    unordered_map<string, SwitchNode*>::iterator sItr = switchDevices.begin();

    while(lItr != loadCells.end())
    {
        (*lItr).second->removeSysNode(0);
        delete (*lItr).second;
        _hasScale--;
    }

    while(tItr != tempSensors.end())
    {
        (*tItr).second->removeSysNode(0);
        (*tItr).second->removeSysNode(1);
        (*tItr).second->removeSysNode(2);
        delete (*tItr).second;      //delete the memory
        _hasTemperature--;
    }

    while(sItr != switchDevices.end())
    {
        (*sItr).second->removeSysNode(0);
        delete (*sItr).second;
        _hasSwitch--;
    }
}

void BrewModule::loadTempProbe(pair<string, string> node)
{
    int sysIndex;
    pair<unordered_map<string, TemperatureNode*>::iterator, bool> ret;
    
    TemperatureNode *ptr = new TemperatureNode(TEMPERATURE_PATH + node.second + "/");
    ret = tempSensors.insert(make_pair(node.first, ptr));

    if(!ret.second)
    {
        cout << "dublicated label" << endl;
        delete ptr;
        return;
    }

    //Add the first attribute to the Sys Node, returning index of the added attribute
    sysIndex = ptr->addSysNode("rtd_impedence");
    ptr->openSysNode(sysIndex, O_RDONLY); //use returned index to open the sys node for use
    
    //Add the second attribute
    sysIndex = ptr->addSysNode("ref_impedence");
    ptr->openSysNode(sysIndex, O_RDONLY);
    
    //Add the third attribute
    sysIndex = ptr->addSysNode("pga_gain");
    ptr->openSysNode(sysIndex, O_RDONLY);
    
    initTempSensor(node.first);

    _hasTemperature++;
}

void BrewModule::loadLoadCell(pair<string, string> node)
{
    int sysIndex;
    pair<unordered_map<string, LoadCellNode*>::iterator, bool> ret;

    LoadCellNode *ptr = new LoadCellNode(LOAD_PATH + node.second + "/");
    ret = loadCells.insert(make_pair(node.first, ptr));

    if(!ret.second)
    {
        cout << "duplicated label" << endl;
        delete ptr;
        return;
    }
    
    sysIndex = ptr->addSysNode("raw");
    ptr->openSysNode(sysIndex, O_RDONLY);
    
    BrewModule::_hasScale++;
}

void BrewModule::loadSwitch(pair<string, string> node)
{
    int sysIndex;
    pair<unordered_map<string, SwitchNode*>::iterator, bool> ret;

    SwitchNode *ptr = new SwitchNode(SWITCH_PATH + node.second + "/");
    ret = switchDevices.insert(make_pair(node.first, ptr));

    if(!ret.second)
    {
        cout << "duplicated label" << endl;
        delete ptr;
        return;
    }

    sysIndex = ptr->addSysNode("power_level");
    ptr->openSysNode(sysIndex, O_RDWR);
    
    BrewModule::_hasSwitch++;
}

void BrewModule::initTempSensor(string nodeLabel)
{
    BrewModule::tempSensors[nodeLabel]->setGain();
    BrewModule::tempSensors[nodeLabel]->setRefZ();
}

unsigned int BrewModule::getUsage(unsigned int equipmentID)
{
    vector<excl>::iterator itr; 
    pe_t key;
    unsigned int eqID, procID;
    bool isExcl;

    /* If the used list has been emptied, reset the iterator postion to the start, else get it's previous position and continue looking */
    if(BrewModule::usedList.empty())
    {
        itr = BrewModule::usedList.begin();
    }
    else
    {
        itr = BrewModule::usedList.begin() + BrewModule::itrPosition;
    }

    /* Look for the next instance of the usage and return it */
    for( ; itr != BrewModule::usedList.end(); ++itr)
    {
        ++BrewModule::itrPosition;
        key = (*itr).usageID;
        eqID = E(key);

        if(eqID == equipmentID)
        {
            isExcl = (*itr).exclusive;
            procID = P(key);
            return PE(procID, isExcl);
        }   
    }
    
    /* If nothing was found reset the iterator position */
    BrewModule::itrPosition = 0;
    
    return Process::ID_NULL;
}

unsigned int BrewModule::addUsage(pe_t usageID, bool exclusive)
{
    vector<excl>::iterator itr;

    excl mUsed = {
        .usageID = usageID, 
        .exclusive = exclusive,
    };

    //check if the usageID is already in the list and then only update the exlusive value
    for(itr = BrewModule::usedList.begin(); itr != BrewModule::usedList.end(); ++itr)
    {
        if((*itr).usageID == usageID)
        {
            (*itr).exclusive = exclusive;
            return BrewModule::usedList.begin() - itr;
        }
    }

    BrewModule::usedList.push_back(mUsed);

    return std::distance(BrewModule::usedList.begin(), BrewModule::usedList.end());
}

pe_t BrewModule::getExclusive(unsigned int processCode, unsigned int equipmentID)
{
    unsigned int ret;
    pe_t requestingUsageID = PE(processCode, equipmentID);
    pe_t holdingUsageID;

    if(equipmentID == BrewModule::NODE_NULL)
    {
        cout << "equipment ID 0 is reserved" << endl;
        return 0;
    }

    //stop the current processes which are using the equipment
    while((ret = getUsage(equipmentID)) != Process::ID_NULL)
    {
        holdingUsageID = PE(P(ret), equipmentID); 

        //if the requesting ID already has the usage.
        if(requestingUsageID == holdingUsageID)
        {
            //do nothing as it already has usage
            continue;
        }
        else
        {
            relinguish(holdingUsageID);  
        }      
    }
    //add the new process to the list
    BrewModule::addUsage(requestingUsageID, true);

    return requestingUsageID;
}

void BrewModule::getNonExclusive(unsigned int processCode, unsigned int equipmentID)
{
    unsigned int ret;
    pe_t holdingUsage;
    pe_t requestingUsage = PE(processCode, equipmentID);

    if(equipmentID == BrewModule::NODE_NULL)
    {
        cout << "equipment ID 0 is reserved" << endl;
        return;
    }

    while((ret = getUsage(equipmentID)) != Process::ID_NULL)
    {   
        /* If the returned process doesnt have exclusive access keep looking as it can be shared */
        if(E(ret) == 0)
        {
            continue;
        }
        else
        {   
            /* Stop the process that has the current usage and it requires it in an exclusive way */
            holdingUsage = PE(P(ret), equipmentID);
            relinguish(holdingUsage);
        }
    }
    /* add the new usage id */
    BrewModule::addUsage(requestingUsage, false);
}

void BrewModule::releaseUsage(pe_t usageID)
{
    //Ignore the call to releaseUsage if relinguish has been called and has therfore released the equipment
    if(BrewModule::relinguishingEquip == usageID)
    {
        return;
    }

    //get the device and process
    Device *machine = core::Device::getInstance();
    Process *mProcess = machine->getControlProcess(P(usageID));  
    vector<excl>::iterator itr;
        
    if(mProcess != nullptr)
    {   
        //loop through the used list to find the process using the equipment
        for((itr = BrewModule::usedList.begin()); itr != BrewModule::usedList.end(); ++itr)
        {
            if((*itr).usageID == usageID)
            {
                BrewModule::usedList.erase(itr);
                break;
            }
        }
    }
}

void BrewModule::relinguish(pe_t usageID) 
{
    /* Indicate which usage is being relingushed so that calls to releaseUsage are ignored */
    BrewModule::relinguishingEquip = usageID;

    //get the device and process
    Device *machine = core::Device::getInstance();
    Process *mProcess = machine->getControlProcess(P(usageID));  
    vector<excl>::iterator itr;
        
    if(mProcess != nullptr)
    {   
        //loop through the used list to find the process using the equipment
        for((itr = BrewModule::usedList.begin()); itr != BrewModule::usedList.end(); ++itr)
        {
            if((*itr).usageID == usageID)
            {
                BrewModule::usedList.erase(itr); //remove from the list tracking if it is in use
                BrewModule::itrPosition--;      //decrease iterator position to account for item being erased and vector reducing by 1.
                mProcess->stop();
                break;
            }
        }
    }
    
    /* reset back to null */
    BrewModule::relinguishingEquip = 0;
}

bool isNumber(const string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int validComparator(int pos, string tryStr, std::string an)
{
    bool ok;
    string index;

    ok = tryStr.compare(0, pos, an) == 0;

    if(ok)
    {
        index = tryStr.substr(pos, 2);
        if(isNumber(index))
        {
            cout << "valid node: " << (tryStr) << endl;
            return 0;
        }
    }
    
    return -1;
}

int BrewModule::isValidNode(string tryNode)
{
    string node, index;
    string acceptableNodes[] = { "probe", "load_cell", "switch" };
    const int invalid = 0;
    const int probe = 1;
    const int loadCell = 2;
    const int switching = 3;

    //if the node is a probe node
    if(validComparator(5, tryNode, acceptableNodes[0]) == 0)
    {
        return probe;
    }
    if(validComparator(9, tryNode, acceptableNodes[1]) == 0)
    {
        return loadCell;
    }
    if(validComparator(6, tryNode, acceptableNodes[2]) == 0)
    {
        return switching;
    }
    else
    {
        cout << "invalid node: " << tryNode << endl;
        return invalid;
    }
}

double BrewModule::getTemperature(const string &label)
{
    return BrewModule::tempSensors.at(label)->getTemperature();
}

double BrewModule::getMass(const string &label)
{   
    return BrewModule::loadCells.at(label)->getMass();
}

void BrewModule::calibrateLoadCell(const string &label, calibrationPoint *calPoint, int points)
{
    try
    {
        BrewModule::loadCells.at(label)->calibrate(calPoint, points);
    }
    catch(const std::out_of_range& e)
    {
        cout << "undefined node" << "\n";
    }
}

void BrewModule::setPower(const string &label, int power)
{
    try
    {
        BrewModule::switchDevices.at(label)->setPower(power);
    }
    catch(const std::out_of_range& e)
    {
        cout << "undefined node" << "\n";
    }
}

bool BrewModule::hasTemperature()
{
    return !(BrewModule::_hasTemperature == 0);
}

bool BrewModule::hasScale()
{
    return !(BrewModule::_hasScale == 0);
}

bool BrewModule::hasSwitch()
{
    return !(BrewModule::_hasSwitch == 0);
}

double BrewModule::fetchTemperature(const string &label)
{
    return BrewModule::tempSensors.at(label)->fetchTemperature();
}

double BrewModule::fetchMass(const string &label)
{
    return BrewModule::loadCells.at(label)->fetchMass();        
}

int BrewModule::fetchPower(const string &label)
{
    return BrewModule::switchDevices.at(label)->fetchPower();
}