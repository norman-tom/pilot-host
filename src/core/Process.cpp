#include "Process.hpp"
#include "Device.hpp"

using namespace nectar::core;

Process::Process(unsigned int processCode, string description)
{
    Process::processCode = processCode;
    Process::description = description;
    Process::argument = 0;
    Device::getInstance()->newControlProcess(Process::processCode, this);         //if duplicate will not add to list of processes and you will have a dangling process.
}

Process::~Process()
{
    Device::getInstance()->delControlProcess(Process::processCode);
}

void Process::start()
{
    lock_guard<mutex> lk(mux);
    
    if(running)
        return;

    stopRequested = false;
    wakeRequested = false;
    running = true;
    Process::th = std::thread(&Process::run, this);
}

void Process::stop()
{ 
    if(!running)
        return;

    {
        lock_guard<mutex> lk(mux);
        wakeRequested = true;
        stopRequested = true;     
    }

    cv.notify_all(); 

    th.join(); 
    
    {
        lock_guard<mutex> lk(mux);
        running = false; 
    }

}

bool Process::isRunning()
{
    lock_guard<mutex> lk(mux);
    return running;
}

void Process::sleepProcess(long ms)
{
    unique_lock<mutex> lk(mux);
    while(cv.wait_for(lk, std::chrono::milliseconds(ms)) == std::cv_status::no_timeout)
    {
        if(wakeRequested)
            break;
    }
}

void Process::wakeProcess()
{ 
    lock_guard<mutex> lk(mux);
    wakeRequested = true;    
    cv.notify_all();
}

bool Process::shouldWake()
{   
    lock_guard<mutex> lk(mux);
    return wakeRequested;    
}

bool Process::shouldStop()
{
    lock_guard<mutex> lk(mux);
    return stopRequested;
}

void Process::setArg(double arg)
{
    lock_guard<mutex> lk(mux);
    argument = arg;
}

double Process::getArg()
{
    lock_guard<mutex> lk(mux);
    return argument;
}