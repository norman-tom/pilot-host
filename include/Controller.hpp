#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "BrewModule.hpp"
#include "URL.hpp"
#include "Mapping.hpp"

namespace nectar::core
{
    /**
     * @brief The Controller Class is responsible for the operation of the device.
     * 
     * It is used to start, stop, read and write to the device's control processes.
     */
    class Controller
    {
        private:

        /* Argument constants */
        public:
            static const unsigned int CMD_NULL = 0x00;
            static const unsigned int CMD_EXIT = 0x01;
            static const unsigned int CMD_START = 0x02;
            static const unsigned int CMD_STOP = 0x03;
            static const unsigned int CMD_READ = 0x04;
            static const unsigned int CMD_WRITE = 0x05;
        
        public:
            /**
             * @brief Starts a controll proccess in its own thread
             * @param processCode The unique ID of the process code to start
             * @param data Initialisation data for the process. For example the Brew Module to calibrate.
             * @return Zero on success or error
             */
            int start(unsigned int processCode, double &data);

            /**
             * @brief Signals to stop a controll process. The control process function decides what to do with the signal.
             * @param processCode The unique ID of the process code to stop
             * @return Zero on success or error
             */
            int stop(unsigned int processCode);

            /**
             * @brief Reads the value at the URL
             * @param url The URL which the value resides at
             * @param buf The return buffer
             * @return Zero on success or error
             */
            int read(URL const &url, Mapping const &map, double &buf) const;

            /**
             * @brief Reads the parameter value from a control process
             * @param processCode The unique ID of the process to read results from
             * @param parameter The parameter to read
             * @param buffer Buffer to store the return value
             * @return Zero on success or error
             */
            int readProcess(unsigned int processCode, unsigned int parameter, double &buffer) const;

            /**
             * @brief Reads a value from the BrewModule. These are the cached values returned from a process.
             * @param moduleID the BrewModule to read the value from. 
             * @param parameter the pramaters identification code @see BrewModule.hpp 
             * @param buffer A reference to the buffer which is to store the value.
             * @return Zero on success or error
             */
            int readModule(unsigned int moduleID, unsigned int parameter, double &buffer) const;

            /**
             * @brief Writes a parameter to the control process.
             * @param processCode The unique ID of the process to write to
             * @param parameter The unique parameter ID to update @see Plant.hpp
             * @param value The parameter value to write
             * @return Zero on success or error
             */
            int write(unsigned int processCode, unsigned int parameter, double value);  

    };    
}

#endif
