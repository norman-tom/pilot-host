#ifndef PID_PROCESS_H
#define PID_PROCESS_H

#include "lib/pid.hpp"
#include <mutex>

namespace nectar::core
{
    /**
     * A PID Process is one which requires PID functionality in the process's algorithm. 
     * The concrete class of the process will need to inherit from PIDProcess Class to add PID functionality.
     * @see pid library for the PID control algorithm methods. 
     */
    class PIDProcess
    {
        private:
            std::mutex mux;                     /**< Mutex for thread safe access to the PID parameters */
            pid::PidParameter PID;              /**< The PID parameters for executing the control alogrithm */
        
        protected:
            /**
             * Gets the PID parameters to be used in the control algorithm
             * @return A reference to the PID parameters aggregated to the process. 
             * @see pid library for PidParameters class.
             */
            pid::PidParameter &getPidParams();

        public:
            /**
             * Set the parameters for the PID algorithm
             * @param SP setpoint
             * @param KP proportional coefficient
             * @param KI intergral coefficient
             * @param KD differential coefficient
             * @param TS time step
             */
            void setPidParams(double SP, int KP, int KI, int KD, int TS);

            /**
             * Reads a PID parameter
             * @param parameter The parameter to read
             * @see Proccess Class for the PID parameter designators
             * @param val A buffer for the return value. Can be either double or integer depending on parameter being read 
             */
            void readParam(unsigned int parameter, double &val);
            
            /**
             * Write a new value to one of the PID parameters
             * @param parameter The PID parameter to write the new value too
             * @see Proccess Class for the PID parameter designators
             * @param val The new value
             */
            void writeParam(unsigned int parameter, double val);

    };
}

#endif