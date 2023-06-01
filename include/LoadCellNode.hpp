#ifndef LOAD_CELL_NODE_H
#define LOAD_CELL_NODE_H

#include "SysHandler.hpp"
#include <atomic>

namespace nectar::hardware
{
    struct calibrationPoint
    {
        int rawValue;
        double mass;
    };

    class LoadCellNode: public nectar::core::SysHandler
    {
        private:
            int gradient;                                       /**< The raw value - mass linear relationship */
            int zeroValue;                                      /**< Raw value which represents a zero mass */
            int massRaw;                                        /**< Raw value from the sensor */
            atomic<double> mass{0};                                /**< The cached mass value */
            vector<struct calibrationPoint *> calPoints;

            void addCalPoint(int raw, double mass);

        public:
            using SysHandler::SysHandler;
            void clearCalPoints(void);

            /**
             *  Updates the raw value from the sysfs, recalculats the mass and stores it in the cache
             * @return The most current mass
             */
            double getMass(void);  

            /**
             * Gets the cached mass.
             * @return The cached mass
             */             
            double fetchMass(void);   

            /**
             * Zeros the Load Cell
             */          
            void zero(void);
            
            /**
             * @brief Calibrates the load cell to a mass
             * @param calPoints An array of calibration points to calibrate the load cell against
             * @param points The number of calibration points in the array
             */
            void calibrate(struct calibrationPoint calPoints[], int points);
    };
}

#endif