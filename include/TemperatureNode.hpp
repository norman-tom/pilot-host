#ifndef TEMPERATURE_NODE_H
#define TEMPERATURE_NODE_H

#include "SysHandler.hpp"
#include <atomic>

namespace nectar::hardware
{
    class TemperatureNode: public nectar::core::SysHandler
    {   
        private:
            unsigned int adcRefImpedence;
            unsigned int adcGain;
            double rtdImpedence;
            double leadImpedence;
            atomic<double> temperature{0};         //cached temperature

            double rawToImpedence(unsigned int raw);

        public:
            unsigned int rtdRaw;
            unsigned int leadRaw;
            
            using SysHandler::SysHandler;
            /**
             * Set the gain of the sensors amplifier. Used for raw to real conversion.
             */
            void setGain();

            /**
             * Set the reference value of the sensor. Used for raw to real conversion. 
             */
            void setRefZ();

            /**
             * Gets the latest raw value from sysfs, recalculates temperature and stores in the cache
             * @return The latest temperature value in degrees celcius
             */
            double getTemperature(void);

            /**
             * Get the cached temperature value
             * @return Temperature in degrees celcius
             */
            double fetchTemperature(void);
    };
}
#endif