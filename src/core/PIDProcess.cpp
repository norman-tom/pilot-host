#include "PIDProcess.hpp"
#include "Process.hpp"
#include <exception>
#include <iostream>

using namespace nectar::core;
using namespace std;

void PIDProcess::setPidParams(double SP, int KP, int KI, int KD, int TS)
{
    PID.setSetPoint(SP);
    PID.setKP(KP);
    PID.setKI(KI);
    PID.setKD(KD);
    PID.setTS(TS);
}

pid::PidParameter &PIDProcess::getPidParams()
{
    lock_guard<mutex> lk(mux);
    return PID;    
}

void PIDProcess::readParam(unsigned int parameter, double &val)
{
    switch(parameter)
    {
        case Process::PARAM_SP:
            val = PID.getSetPoint();
            return;
        case Process::PARAM_KP:
            val = PID.getKP();
            return;
        case Process::PARAM_KI:
            val = PID.getKI();
            return;
        case Process::PARAM_KD:
            val = PID.getKD();
            return;
        case Process::PARAM_TS:
            val = PID.getTS();
            return;
        default:
            throw invalid_argument("undefined parameter");
    }
}

void PIDProcess::writeParam(unsigned int parameter, double val)
{
    switch(parameter)
    {
        case Process::PARAM_SP:
            PID.setSetPoint(val);
            return;
        case Process::PARAM_KP:
            PID.setKP(val);
            return;
        case Process::PARAM_KI:
            PID.setKI(val);
            return;
        case Process::PARAM_KD:
            PID.setKD(val);
            return;
        case Process::PARAM_TS:
            PID.setTS(val);
            return;
        default:
            throw invalid_argument("undefined parameter");
    }
}