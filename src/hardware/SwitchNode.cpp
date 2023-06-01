#include "SwitchNode.hpp"

#define MAX_POWER 100
#define MIN_POWER 0

using namespace nectar::hardware;

void SwitchNode::setPower(int _power)
{   
    if(power < MIN_POWER)
    {
        power.store(0, memory_order_relaxed);
    }
    else if(power > MAX_POWER)
    {
        power.store(100, memory_order_relaxed);
    }
    else 
    {
        power.store(_power, memory_order_relaxed);
    } 

    string mPower = to_string(power.load(memory_order_relaxed));
    const char *buf = mPower.c_str();
    int nbytes = sizeof(buf);

    SwitchNode::writeSysNode(0, buf, nbytes);
}

unsigned int SwitchNode::fetchPower()
{
    return power.load(memory_order_relaxed);
}