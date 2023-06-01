#include "Controller.hpp"
#include <string>
#include <stdio.h>
#include "lib/pid.hpp"
#include "Device.hpp"
#include "Process.hpp"
#include "PIDProcess.hpp"

using namespace nectar::core;
using namespace nectar::hardware;

int Controller::start(unsigned int processCode, double &data)
{
    Device *machine = Device::getInstance();
    Process *mProcess = machine->getControlProcess(processCode); 

    if(mProcess == nullptr)
    {
        return 1;
    }

    //set the argument to run the command with
    mProcess->setArg(data);

    mProcess->stop();           //Stop the process if running. Call to stop if not running will do nothing
    mProcess->start();          //Start the process

    return 0;
}

int Controller::stop(unsigned int processCode)
{
    Device *machine = Device::getInstance();
    Process *mProcess = machine->getControlProcess(processCode);
    
    if(mProcess == nullptr)
    {
        return 1;
    }

    mProcess->stop();

    return 0;
}

int Controller::read(URL const &url, Mapping const &map, double &buf) const
{
    unsigned int code = *url.getParts().device;             //get the device level machine code.

    if(!url.isReadable())                                   //if the URL is not flagged as readable return without reading
    {
        cout << "URL is not readable" << "\n";
        return 1;
    }
    if(code == map.machine("/device"))                      //if it is a module read
    {
        return readModule(*url.getParts().module, url, buf);      
    }
    else if(code == map.machine("/process"))                //if it is a process read
    {
        return readProcess(*url.getParts().module, url, buf);
    }
    else
    {
        cout << "undefined device to read" << "\n";
        return 2;
    }    
}

int Controller::readProcess(unsigned int processCode, unsigned int parameter, double &buffer) const
{
    int ret = 0;
    Device *machine = Device::getInstance();
    Process *mProcess = machine->getControlProcess(processCode);
    PIDProcess *mPIDProc;

    if(mProcess == nullptr)
    {
        return 1;
    }

    //if the procecss is a PID process then parameters can be read else it cannot be read
    if(mPIDProc = dynamic_cast<PIDProcess*>(mProcess))
    {
        try
        {
            mPIDProc->readParam(parameter, buffer);
            return 0;
        }
        catch(const invalid_argument& e)
        {
            std::cerr << e.what() << endl;
            return 3;
        }
    }
    else
    {
        return 2;
    }
}

int Controller::readModule(unsigned int moduleID, unsigned int parameter, double &buffer) const
{
    #define fetch_power(i) buffer = bm->fetchPower(i); break;

    Device *machine = Device::getInstance();
    BrewModule *bm = machine->getModule(moduleID);

    if(bm == nullptr)
    {
        cout << "undefined module" << "\n";
        return 1;
    }

    try
    {
        switch(parameter)
        {
            case BrewModule::CACHED_TEMP:
                buffer = bm->fetchTemperature("primary");
                break;
            case BrewModule::CACHED_MASS:
                buffer = bm->fetchMass("primary");
                break;
            case BrewModule::CACHED_ELEMENT:
                buffer = bm->fetchPower("element"); 
                break;
            case BrewModule::CACHED_PUMP:
                buffer = bm->fetchPower("pump");
                break;
            default:
                cout << "undefined node" << "\n";
                return 2;
        }
    }
    catch(const out_of_range& e)
    {
        buffer = 0;
        std::cerr << "undefined node" << '\n';
        return 2;
    }

    return 0;
}

int Controller::write(unsigned int processCode, unsigned int parameter, double value)
{
    Device *machine = Device::getInstance();
    Process *mProcess = machine->getControlProcess(processCode);
    PIDProcess *mPIDProc;

    if(mProcess == nullptr)
    {
        return 1;
    }

    if(mPIDProc = dynamic_cast<PIDProcess*>(mProcess))
    {
        try
        {
            mPIDProc->writeParam(parameter, value);
            return 0;
        }
        catch(const invalid_argument& e)
        {
            cerr << e.what() << endl;
            return 3;
        }
    }
    else
    {
        return 2;
    }
} 