#ifndef MEASURE_TEMPERATURE_H
#define MEASURE_TEMPERATURE_H

#include "Process.hpp"
#include "ContextedProcess.hpp"
#include "PIDProcess.hpp"

using namespace nectar::core;

namespace nectar::processes
{
    typedef const unsigned int procid_t;

    static procid_t NULL_PROCESS                = 0x100;
    static procid_t HLT_CALIBRATE_SCALE         = 0x101;
    static procid_t HLT_MEASURE_TEMP            = 0x102;
    static procid_t HLT_MEASURE_WEIGHT          = 0x103;
    static procid_t HLT_MANUAL_ELEMENT          = 0x104;
    static procid_t HLT_MANUAL_PUMP             = 0x105;
    static procid_t HLT_CONTROL_TEMP            = 0x106;
    static procid_t HLT_CONTROL_WEIGHT          = 0x107;

    static procid_t MASH_CALIBRATE_SCALE        = 0x108;
    static procid_t MASH_MEASURE_TEMP           = 0x109;
    static procid_t MASH_MEASURE_WEIGHT         = 0x10A;
    static procid_t MASH_MANUAL_PUMP            = 0x10B;
    static procid_t MASH_CONTROL_TEMP           = 0x10C;
    static procid_t MASH_CONTROL_WEIGHT         = 0x10D;

    static procid_t KETTLE_CALIBRATE_SCALE      = 0x10E;
    static procid_t KETTLE_MEASURE_TEMP         = 0x10F;
    static procid_t KETTLE_MEASURE_WEIGHT       = 0x110;
    static procid_t KETTLE_BOIL                 = 0x111;


    class CalibrateScale : public Process, public ContextedProcess<int>
    {
        public:                             
            using Process::Process;         
                                            
        private:                            
            void run() override;            
    };

    class MeasureScale : public Process, public ContextedProcess<int>, public PIDProcess
    {
        public:                             
            using Process::Process;         
                                            
        private:                            
            void run() override;            
    };

    class MeasureTemperature : public Process, public ContextedProcess<int>, public PIDProcess
    {
        public:                             
            using Process::Process;         
                                            
        private:                            
            void run() override;            
    };

    class SwitchElement : public Process, public ContextedProcess<int>
    {
        public:                             
            using Process::Process;         
                                            
        private:                            
            void run() override;            
    };

    class SwitchPump : public Process, public ContextedProcess<int>
    {
        public:
            using Process::Process;

        private:
            void run() override;
    };

    class ControlTemperature : public Process, public ContextedProcess<int>, public PIDProcess
    {
        public:
            using Process::Process;

        private:
            void run() override;
    };

    class ControlWeight : public Process, public ContextedProcess<int>, public PIDProcess
    {
        public:
            using Process::Process;

        private:
            void run() override;
    };

    class Boil : public Process, public ContextedProcess<int>, public PIDProcess
    {
        public:
            using Process::Process;

        private:
            void run() override;
    };

}

#endif