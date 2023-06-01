#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <map>
#include <mutex>
#include"Process.hpp"
#include "BrewModule.hpp"

using namespace nectar::hardware;

namespace nectar::core
{
    /**
     * @brief The main data structure for the machine.
     * Acts as an address book for all the Brew Modules and Control Processes.
     * Handles creating the objects, and adding and retrieving the addresses.
     */
    class Device
    {
        /* Members to handle the Singleton pattern creation */
        private:
            static Device *pInstance_;
            static std::mutex mutex_;
            Device();
            ~Device() { }

        public:
            Device(Device &other) = delete;
            void operator=(const Device &) = delete;
            static Device *getInstance();

        /* Buisness logic members and methods */
        private:
            /* A list of pointers to all the intialised Brew Modules */
            map<unsigned int, BrewModule *> brewModules;
            /* A list of pointers to all the intialised control process*/
            map<unsigned int, Process *> controlProcesses;

        public:

            /**
             * @brief Creates a new Brew Module and adds to the managed list of Brew Modules
             * @param moduleID A unique ID for the BrewModule
             * @param moduleName Name of the Brew Module to add to the device
             * @return Pointer to the new module
             */
            BrewModule *newModule(unsigned int moduleID, std::string moduleDesc);

            /**
             * @brief Creates a new Brew Module and adds to the managed list of Brew Modules
             * @param moduleID A unique ID for the BrewModule
             * @param moduleName Name of the Brew Module to add to the device
             * @param nodes An array of <label - node> pairs associated with this BrewModule. Label is used to associate an identifier with the node.
             * @return Pointer to the new module
             */
            BrewModule *newModule(unsigned int moduleID, std::string moduleName, std::vector<std::pair<std::string, std::string>> nodes);

            /**
             * @brief Removes the Brew Module pointer from the list and destroys the object.
             * @param moduleName The name of the Brew Module to delete
             * @return Zero on success or error
             */
            int removeModule(unsigned int moduleID);

            /**
             * @brief Get a Brew Module by it's unique ID if it exists
             * @param moduleID The unique ID of the Brew Module to return
             * @return A pointer to the requested Brew Module
             */
            BrewModule *getModule(unsigned int moduleID);

            /**
             * @brief Creates a new control process
             * @param processID A unique identifiation number for the process
             * @param processDesc A description for the process
             * @param processFunc The function associated with the process to be executed by the Controller
             * @return A pointer to the newly created process
             */
            int newControlProcess(unsigned int processID, Process *process);

            /**
             * @brief Delets a controll process and frees the memory
             * @param processID The process code of the control process to be deleted
             * @return Zero for sucess or error
             */
            void delControlProcess(unsigned int processID);

            /**
             * @brief Retrieves a control process by it's unique ID
             * @param processID The unique ID of the control process
             * @return A pointer to the control process or nullptr on failure
             */
            Process *getControlProcess(unsigned int processID);

            /**
             * @brief Gets all the intialised control process IDs from the device
             * @return A vector of the control process IDs which have been initialised with the device
             */
            std::vector<unsigned int> getAllControlProcesses();
    };
}

#endif