#include "TemperatureNode.hpp"
#include "lib/nectariio.hpp"
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <iostream>

#define FS 0x400000
#define R0 100.0
#define ALPHA 3.9083
#define BETA -5.775
#define GAMMA -4.183

using namespace nectar::hardware;

double TemperatureNode::rawToImpedence(unsigned int raw)
{
    return nectariio::rawRealConverstion(FS, TemperatureNode::adcGain, raw, TemperatureNode::adcRefImpedence);
}

void TemperatureNode::setGain()
{
    //set the adcGain in the temp sensor
    TemperatureNode::adcGain = TemperatureNode::updateNode(2, 10);  //pga sys node is 2
}

void TemperatureNode::setRefZ()
{
    //set the reference impedence in the temp sensor
    TemperatureNode::adcRefImpedence = TemperatureNode::updateNode(1, 10);  //ref z sys node index is 1
}

double TemperatureNode::getTemperature(void)
{   
    double tempVal;
    //update node to latest value
    long rawVal = updateNode(0, 16);    
    
    //convert raw to impednece and set the member with the value
    rtdImpedence = TemperatureNode::rawToImpedence(rawVal);
    tempVal = nectariio::impTempConversion(R0, rtdImpedence, ALPHA, BETA);

    temperature.store(tempVal, memory_order_acquire);       //store in cache
    return temperature.load();
}

double TemperatureNode::fetchTemperature()
{
    return temperature.load(memory_order_relaxed);
}