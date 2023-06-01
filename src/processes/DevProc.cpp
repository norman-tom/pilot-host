#include "processes/DevProc.hpp"
#include "BrewModule.hpp"
#include "Device.hpp"

using namespace nectar::processes;
using namespace nectar::hardware;
using namespace nectar::core;

void CalibrateScale::run()
{  
    Device *machine = Device::getInstance();
    unsigned int brewModuleID = getContext();

    //The calibration points for the load cell
    calibrationPoint calPoints0[] = 
    {
        {
            .rawValue = 0x5ff14,
            .mass = 0
        },
        {
            .rawValue = 0x95eeb,
            .mass = 5
        }
    };

    calibrationPoint calPoints1[] = 
    {
        {
            .rawValue = (int) 0xFFFF83C5,
            .mass = 0
        },
        {
            .rawValue = 0xF0903,
            .mass = 5
        }
    };

    BrewModule *brewModule = machine->getModule(brewModuleID);

    //pass the calibration points to the BrewModule
    if(brewModule != nullptr)
    {
        if(brewModuleID == 1)
            brewModule->calibrateLoadCell("primary", calPoints0, 2);
        
        if(brewModuleID == 2)
            brewModule->calibrateLoadCell("primary", calPoints1, 2);

        if(brewModuleID == 3)
            brewModule->calibrateLoadCell("primary", calPoints0, 2);
    }
}

void MeasureScale::run()
{
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);

    if(brewModule == nullptr)
    {
        stop();
    }
    
    /* Continuely loop getting the updated temperature */
    while(!shouldStop())
    {
        try
        {        
            brewModule->getMass("primary");
            sleepProcess(getPidParams().getTS());
        }
        catch(const std::exception& e)
        {
            std::cerr << "undefined node" << '\n';
            break;
        }
    }
}

void MeasureTemperature::run()
{
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);

    if(brewModule == nullptr)
        stop();

    /* Continuely loop getting the updated temperature */
    while(!shouldStop())
    {
        try
        {
            brewModule->getTemperature("primary");
            sleepProcess(getPidParams().getTS());
        }
        catch(const std::exception& e)
        {
            std::cerr << "undefined node" << '\n';
            break;
        }
    }
}

void SwitchElement::run()
{
    pe_t usageCode;
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);
    unsigned int power = getArg();

    if(brewModule == nullptr)
    {
        stop();
        return;
    }

    usageCode = brewModule->getExclusive(processCode, BrewModule::NODE_ELEMENT);
    brewModule->setPower("element", power);
    brewModule->releaseUsage(usageCode);
}

void SwitchPump::run()
{
    pe_t usageCode;
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);
    unsigned int power = getArg();

    if(brewModule == nullptr)
    {
        stop();
        return;
    }

    usageCode = brewModule->getExclusive(processCode, BrewModule::NODE_PUMP);
    brewModule->setPower("pump", power);
    brewModule->releaseUsage(usageCode);
}

void ControlTemperature::run()
{
    double retVal;
    double temperature;
    pe_t usageCode;
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);

    if(brewModule == nullptr)
    {
        stop();
        return;
    }

    /* Get exclusive use of the heating element of the HLT */
    usageCode = brewModule->getExclusive(processCode, BrewModule::NODE_ELEMENT);

    pid::PidParameter &pidParam = getPidParams();

    /* Loop the PID control algorithm */
    while(!shouldStop())
    {
        temperature = brewModule->getTemperature("primary");
        pid::calcErrors(pidParam, temperature);
        retVal = pid::doControl(pidParam);
        brewModule->setPower("element", retVal);
        sleepProcess(pidParam.getTS());
    }

    /* Shutdown code */
    brewModule->setPower("element", 0);
    brewModule->releaseUsage(usageCode); //Indicate that we no longer need the equipment 
}

void ControlWeight::run()
{

}

void Boil::run()
{
    double retVal;
    double temperature;
    pe_t usageCode;
    Device *machine = Device::getInstance();
    unsigned int bmID = getContext();
    BrewModule *brewModule = machine->getModule(bmID);

    if(brewModule == nullptr)
    {
        stop();
        return;
    }

    usageCode = brewModule->getExclusive(processCode, BrewModule::NODE_ELEMENT);

    pid::PidParameter &pidParam = getPidParams();

    while(!shouldStop())
    {
        temperature = brewModule->getTemperature("primary");
        pid::calcErrors(pidParam, temperature);
        retVal = pid::doControl(pidParam);
        brewModule->setPower("element", retVal);
        sleepProcess(pidParam.getTS());
    }

    /* Shutdown code */
    brewModule->setPower("element", 0);
    brewModule->releaseUsage(usageCode); //Indicate that we no longer need the equipment 
}