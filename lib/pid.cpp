#include "lib/pid.hpp"
#include <unistd.h>
#include <iostream>
#include <time.h>

using namespace pid;

PidParameter::PidParameter()
{
    for(int i = 0; i < PID_MAX_ERROR; i++)
    {
        err[i].store(0, std::memory_order_release);
    }
}

PidParameter::PidParameter(double SP, int KP, int KI, int KD, int TS)
{
    setPoint.store(SP, std::memory_order_relaxed);
    kp.store(KP, std::memory_order_relaxed);
    ki.store(KI, std::memory_order_relaxed);
    kd.store(KD, std::memory_order_relaxed);
    ts.store(TS, std::memory_order_relaxed);

    //store the default values in the error array
    for(int i = 0; i < PID_MAX_ERROR; i++)
    {
        err[i].store(0, std::memory_order_release);
    }
}

void PidParameter::setSetPoint(double SP)
{
    setPoint.store(SP, std::memory_order_relaxed);    
}

void PidParameter::setKP(int KP)
{
    kp.store(KP, std::memory_order_relaxed);
}

void PidParameter::setKI(int KI)
{
    ki.store(KI, std::memory_order_relaxed);
}

void PidParameter::setKD(int KD)
{
    kd.store(KD, std::memory_order_relaxed);
}

void PidParameter::setTS(int TS)
{
    ts.store(TS, std::memory_order_relaxed);
}

void PidParameter::setOutput(double _output)
{
    output.store(_output, std::memory_order_relaxed);
}

void PidParameter::pushError(double _err)
{
    //push new error to the back of the array and bubble existing to the top
    double newErros[] = 
    {
        err[1].load(std::memory_order_acquire),
        err[2].load(),
        _err
    };

    //store the temporary array values in the errors array
    for(int i = 0; i < PID_MAX_ERROR; i++)
    {
        err[i].store(newErros[i], std::memory_order_release);
    }        
}

double PidParameter::getSetPoint()
{
    return setPoint;
}

int PidParameter::getKP()
{
    return kp;
}

int PidParameter::getKI()
{
    return ki;
}

int PidParameter::getKD()
{
    return kd;
}

int PidParameter::getTS()
{
    return ts;
}

double PidParameter::getOutput()
{
    return output;
}

double PidParameter::getError(int pos)
{
    return err[pos];
}

double pid::outputSignal(double u1,
                    int kp, int ki, int kd, 
                    int ts, 
                    double err0, double err1, double err2)
{   
    double secs = (double) ts / (double) 1000;

    return (u1 +    
            A(kp, ki, kd, secs) * err0 + 
            B(kp, ki, kd, secs) * err1 + 
            C(kd, secs) * err2);
}

double pid::clipSignal(double signal, double min, double max)
{
    if(signal < min)
        return min;
    
    if(signal > max)
        return max;
    
    return signal;
}

void pid::calcErrors(PidParameter &parameters, double read)
{
    double newErr = parameters.getSetPoint() - read;
    parameters.pushError(newErr);
}

double pid::doControl(PidParameter &parameters)
{
    /* Calculate the new output signal */
    double newOutput = pid::outputSignal(parameters.getOutput(), 
                                    parameters.getKP(), 
                                    parameters.getKI(), 
                                    parameters.getKD(), 
                                    parameters.getTS(), 
                                    parameters.getError(2), 
                                    parameters.getError(1), 
                                    parameters.getError(0));

    /* Clip the output signal to within allowable band */
    parameters.setOutput(pid::clipSignal(newOutput, 0.0, 100.0));
    
    return parameters.getOutput();
}   
