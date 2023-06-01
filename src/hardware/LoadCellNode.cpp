#include "LoadCellNode.hpp"
#include "lib/nectariio.hpp"
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <array>

using namespace nectar::hardware;

void LoadCellNode::addCalPoint(int raw, double mass)
{
    struct calibrationPoint *cpptr;

    cpptr = (struct calibrationPoint *) malloc(sizeof(*cpptr));
    cpptr->rawValue = raw;
    cpptr->mass = mass;

    LoadCellNode::calPoints.push_back(cpptr);
}

void LoadCellNode::clearCalPoints(void)
{
    vector<struct calibrationPoint *>::iterator itr = LoadCellNode::calPoints.begin();

    for(itr; itr != LoadCellNode::calPoints.end(); itr++)
    {
        free(*itr);
    }

    LoadCellNode::calPoints.clear();
}

double LoadCellNode::getMass(void)
{
    //update the raw value
    massRaw = updateNode(0, 16);       

    //do the calculation to get the real mass
    double mMass = nectariio::linearResult((double)gradient, 
                                            (double)zeroValue,
                                            (double)massRaw);

    //add to cached value
    mass.store(mMass, memory_order_relaxed);
    
    return mMass;    
}

double LoadCellNode::fetchMass()
{
    return mass.load(std::memory_order_relaxed);
}

void LoadCellNode::zero(void)
{   
    zeroValue = LoadCellNode::updateNode(0, 16);
}

void LoadCellNode::calibrate(struct calibrationPoint calPoints[], int points)
{
    int i, x0, x1;
    double y0, y1;

    //clear the old calibration points
    LoadCellNode::clearCalPoints();

    //update the calibration points vector
    for(i = 0; i < points; i++)
    {
        LoadCellNode::addCalPoint(calPoints[i].rawValue, calPoints[i].mass);
    }

    /*  calculate the new gradient, inverse of ri/ru so ru/ri.
        run is the difference of the output code,
        rise is the difference in mass.    */
    x0 = LoadCellNode::calPoints[0]->rawValue;
    x1 = LoadCellNode::calPoints[1]->rawValue;
    y0 = LoadCellNode::calPoints[0]->mass;
    y1 = LoadCellNode::calPoints[1]->mass;

    LoadCellNode::gradient = nectariio::gradient(x0, x1, y0, y1);

    LoadCellNode::zero();
}