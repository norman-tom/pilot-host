#ifndef BREW_MODULE_H
#define BREW_MODULE_H

#include "LoadCellNode.hpp"
#include "TemperatureNode.hpp"
#include "SwitchNode.hpp"
#include <unordered_map>
#include <mutex>

/**
 * @brief Transform from processID and equipmentID to usageID PROCEQ
 * @param p ProcessID
 * @param e EquipmentID
 */
#define PE(p, e) ((p << 16) | (e & 0xFFFF))
/**
 * @brief Transform usageID back to processID
 * @param pe usageID
 */
#define P(pe) ((pe >> 16) & 0xFFFF)
/**
 * @brief Transforms usageID back to equipmentID
 * @param pe usageID
 */
#define E(pe) (pe & 0xFFFF)

using namespace std;

namespace nectar::hardware
{
    /* usage ID type */
    typedef unsigned int pe_t;
    typedef struct excl
    {
        pe_t usageID;
        bool exclusive;
    } excl;

    /**
     * @brief The Brew Module Class represents a single tank housing the optional add ons; heater, scale, temperature sensor and pump.
     */
    class BrewModule
    {
        private:
            mutex mux;

            /* Class members to hold that state of the BrewModule */
            int _hasTemperature = 0;    //hold if there are temperature nodes and how many.
            int _hasScale = 0;          //hold if there are scale nodes and how many.
            int _hasSwitch = 0;         //hold if there are switches and how many.

            /* Load Cell associated with the Brew Module, check _hasScale*/
            unordered_map<string, LoadCellNode*> loadCells;
            
            /* Temperature sensor associated with the Brew Module, check _hasTemperature */
            unordered_map<string, TemperatureNode*> tempSensors;

            /* Vector of switching devices, these all operate in the same fashion */
            unordered_map<string, SwitchNode*> switchDevices;

            /* hold the process codes which are using equipment and flags whether it is exclusive use or not */
            vector<excl> usedList;

            /* Hold the last access position of usedList */
            int itrPosition;

            /** 
             * @brief indicated whether a call to get equipment has been called. 
             * used to indicate if religuish has been called and a call to releaseUsage should be ignored.
             */
            pe_t relinguishingEquip = 0;

            /**
             * @brief Loads the desired temperature probe node into the BrewModule
             * @param node A valid node name from sysfs for the temperature probe
             */
            void loadTempProbe(pair<string, string> node);

            /**
             * @brief Loads the desired load cell node into the BrewModule
             * @param node A valid node name from sysfs for the load cell
             */
            void loadLoadCell(pair<string, string> node);

            /**
             * @brief Loads the desired switching node into the BrewModule
             * @param node A valid node name from sysfs for the switch
             */
            void loadSwitch(pair<string, string> node);

            /**
             * @brief Initialise the temperature sensor settings. Neccessary to calculate temperature correctly.
             * Function reads the sysfs class of the device to determine the correct setting to set.
             */
            void initTempSensor(string nodeLabel);   

            /**
             * @brief check whether a piece of equipment is in use by a process.
             * equipment may be used but not necessarily exclusively.
             * Subsequent calls will return the next process which is using the equipment. Return of plant::ID_NULL
             * indicates that no other process is using the equipment. First two bytes hold process ID and last two hold exclusive flag
             * These can be access with P(return value) for process ID and E(return value) for exclusive flag.
             * @param equipmentID the equipment id to check usage for
             * @return The process IDs which are using the equipment and if it is exclusive. Returns plant::NULL_ID when no more process have been found.
             * The return is in the format of the usage ID. four MSB indicate the process ID and the last four MSB indicate a flag of whether it is exclusive or not.
             * The macros P(pe_t) and E(pe_t) can be used to get process ID and exclusive flag respectively.
             */
            unsigned int getUsage(unsigned int equipmentID);

            /**
             * @brief Add equipment usage to the Brew modules list of used equipment
             * @param usageID Usage ID to add to the list
             * @param exclusive Whether the usage is to be exclusive or not
             * @return The position in the vector the usage was added too.
             */
            unsigned int addUsage(pe_t usageID, bool exclusive);

             /**
             * @brief Relinguish exclusive use of some equipment
             * @param processID processID which has exlusive use
             * @param equipmentID equipment ID which is being used
             * @return void
             */
            void relinguish(pe_t usageID);

            /**
             * @brief Checks whether or not the node name passes to the Brew Module is valid
             * @param tryNode The node naming to be tried for validity
             * @returns A interger associated with the valid node: 1 = temperature probe, 2 = load_cell, 3 = switch or zero for invalid
             */
            int isValidNode(string tryNode);

        public:
            /* Modules IDs */
            static const unsigned int MOD_NULL = 0x00;
            static const unsigned int MOD_HLT = 0x01;
            static const unsigned int MOD_MASH = 0x02;
            static const unsigned int MOD_KETTLE = 0x03;

            /* Equipment IDs */
            static const unsigned int NODE_NULL = 0x00;
            static const unsigned int NODE_TEMP = 0x01;
            static const unsigned int NODE_LOAD = 0x02;
            static const unsigned int NODE_ELEMENT = 0x03;
            static const unsigned int NODE_PUMP = 0x04;


            /* Read parameters IDs */
            static const unsigned int CACHED_NULL = 0x00;
            static const unsigned int CACHED_TEMP = 0x01;
            static const unsigned int CACHED_MASS = 0x02;
            static const unsigned int CACHED_ELEMENT = 0x03;
            static const unsigned int CACHED_PUMP = 0x04;
            /**
             * @brief Unique ID of the BrewModule
             */
            unsigned int moduleID;

            /**
             * @brief The description of the BreModule.
             */
            string description;

            /**
             * @brief Create a new Brew Module with no nodes associated with it
             * @param moduleID a unique module ID, used to retrieve the Brew Module and perform operations on it
             * @param description A description given to the Brew Module, used to identify the module.
             */
            BrewModule(unsigned int moduleID, string description);

            /**
             * @brief Create a new Brew Module.
             * @param moduleID a unique module ID, used to retrieve the Brew Module and perform operations on it
             * @param description A description given to the Brew Module, used to identify the module.
             * @param nodes An vector of nodes that are to be associated with this Brew Module
             */
            BrewModule(unsigned int moduleID, string description, vector<pair<string, string>> nodes);
            
            /**
             * @brief Destructor
             */
            ~BrewModule();

            /**
             * @brief Nominate exclusive use of a piece of equipment attached to the Brew Module.
             * Get exclusive will pass equipment control over to the latest control process that calls it.
             * Previous control process will be stopped.
             * Does not check for out of bound equipment IDs (i.e ID that are not assigned to equipment).
             * It will create an exculive use of that equipment with the passed ID regardless of existance. 
             * @param processCode The process ID which want exclusive useage
             * @param equipmentID The ID of the equipment that the process is using
             * @return A usage code. Used to release usage later with releaseUsage.
             * Returns 0 to indicate reserved equipment ID.
             */
            pe_t getExclusive(unsigned int processCode, unsigned int equipmentID);

            /**
             * @brief Nominate non exclusive use of equipment.
             * If there is another process running the requested equipment non exclusively, multiple processes will control the equipment concurrently.
             * If a process currently has exclusive use of the equipment it will be stopped.
             * @param usageID Usage ID for the equipment to get non exclusive use for
             * @return void
             */
            void getNonExclusive(unsigned int processCode, unsigned int equipmentID);

            /**
             * @brief Indicate to the system that the process is finised with the equipment and it can be removed from the used list
             * @param usageID The usage ID that wished to releases the equipment. 
             */
            void releaseUsage(pe_t usageID);

            /**
             * @brief Get the temperature of the Brew Module's temperature sensor
             * @return Temperature in degrees celsius
             * @throws out_of_range exception if the label is not found
             */         
            double getTemperature(const string &label);

            /**
             * @brief Get the mass of the Brew Module's content.
             * @return Mass in kilograms
             * @throws out_of_range exception if the label is not found
             */
            double getMass(const string &label);

            /**
             * @brief Calibrate the load cell attached to the Brew Module
             * @param calPoints An array of calibration points to calibrate the load cell against
             * @param points The number of calibration points in the array
             */
            void calibrateLoadCell(const string &label, calibrationPoint *calPoint, int points);

            /**
             * @brief Sets the desired power level for the switching device.
             * @param index The index of the switching device to set the power level for. BrewModules can control multiple switching nodes.
             * @param power Power to set the switching device at.
             */
            void setPower(const string &label, int power);

            /**
             * @brief Check if the Brew Module has a temperature sensor
             * @return True if has temperature sensor
             */
            bool hasTemperature();

            /**
             * @brief Check if the Brew Module has a load cell
             * @return True if has load cell
             */
            bool hasScale();

            /**
             * @brief Check if the Brew Module has a switching devices associated with it.
             * @return True if has a device.
             */
            bool hasSwitch();

            /**
             * @brief Gets the cached temperature
             * @param label The identifier string of the load cell to return the temperature for.
             * @return The cached temperature.
             * @throws out_of_range exception if label is not found
             */
            double fetchTemperature(const string &label);

            /**
             * @brief Gets the cached mass
             * @param label The identifier string of the load cell to return the mass for.
             * @return The cached mass.
             * @throws out_of_range exception if label is not found
             */
            double fetchMass(const string &label);

            /**
             * @brief Gets the cached power level
             * @param label The identifier string of the switching device to return the power for.
             * @return The set power level in percentage (0 - 100).
             * @throws out_of_range exception if label is not found
             */
            int fetchPower(const string &label);
    };
}
#endif