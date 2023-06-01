#ifndef SWITCH_NODE_H
#define SWITCH_NODE_H

#include "SysHandler.hpp"
#include <atomic>

namespace nectar::hardware
{
    class SwitchNode: public nectar::core::SysHandler
    {
        private:
            atomic<unsigned int> power{0};         //cached power

        public:
            using SysHandler::SysHandler;

            /**
             * Sets the power level of the switching device
             * @param power The desired power level
             */
            void setPower(int power);

            /**
             * Get the cached power level
             * @return The power level in percentage
             */
            unsigned int fetchPower();
    };
}

#endif