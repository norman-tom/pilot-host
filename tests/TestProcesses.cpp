#include "TestProcesses.hpp"
#include "Device.hpp"
#include "BrewModule.hpp"
#include <iostream>

using namespace nectar::tests;
using namespace devices;
using namespace core;

void *TestProcesses::testProcess_1(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_1);

    BrewModule *bm = machine->getModule(1);

    bm->getExclusive(pCode, BrewModule::NODE_ELEMENT);

    while(!mProcess->stop)
    {
        cout << "Im doing process 1" << endl;
        Plant::timedPause(mProcess);
    }

    __plant_func_end;
    
    return 0;
}

void *TestProcesses::testProcess_2(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_2);

    BrewModule *bm = machine->getModule(1);
    bm->getExclusive(pCode, BrewModule::NODE_PUMP);

    while(!mProcess->stop)
    {
        cout << "Im doing process 2" << endl;
        Plant::timedPause(mProcess);
    }

    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_3(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_3);

    BrewModule *bm = machine->getModule(1);
    bm->getNonExclusive(pCode, BrewModule::NODE_PUMP);

    while(!mProcess->stop)
    {
        cout << "Im doing process 3" << endl;
        Plant::timedPause(mProcess);
    }

    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_4(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_4);

    BrewModule *bm = machine->getModule(1);
    bm->getNonExclusive(pCode, BrewModule::NODE_ELEMENT);

    while(!mProcess->stop)
    {
        cout << "Im doing process 4" << endl;
        Plant::timedPause(mProcess);
    }

    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_5(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_5);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        cout << "P5" << bm->getTemperature() << endl;
        cout << "P5" << bm->getMass() << endl;
    }
    
    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_6(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_6);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        cout << "P6" << bm->getTemperature() << endl;
        cout << "P6" << bm->getMass() << endl;
    }
    
    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_7(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_7);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        cout << "P7" << bm->getTemperature() << endl;
        cout << "P7" << bm->getMass() << endl;
    }
    
    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_8(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_8);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        bm->setPower(0, mProcess->pidParam.setPoint);
        cout << "P8" << endl;
    }
    
    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_9(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_9);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        bm->setPower(0, mProcess->pidParam.setPoint);
        cout << "P9" << endl;
    }
    
    __plant_func_end;

    return 0;
}

void *TestProcesses::testProcess_10(void *data)
{
    __plant_func_start(TestProcesses::PROCESS_10);

    BrewModule *bm = machine->getModule(1);
    
    while(!mProcess->stop)
    {
        bm->setPower(0, mProcess->pidParam.setPoint);
        cout << "P10" << endl;
    }
    
    __plant_func_end;

    return 0;
}