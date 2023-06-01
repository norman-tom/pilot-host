#ifndef ICLIENTINTERFACE_H
#define ICLIENTINTERFACE_H

#include <string>
#include <queue>
#include <condition_variable>
#include "URL.hpp"

using namespace std;

namespace nectar::network
{
    /**
     * A command to control the device
     */
    typedef struct command
    {
        string url;                 /**< The url string */
        unsigned int action;       /**< The command control value */
        double value;               /**< The value to use if required */
    } command;

    /**
     * Interface for the connection to a server or broker. Concrete class is to implement functionality of the connection and callback handling.
     * The server or broker to which the device connects to is responsible for handling transmitions between client and device.
     */
    class IClientInterface
    {
        protected:
        mutex cmdLock;                                          /**< Mutex lock to access and write to the command que */
        queue<command> commandQueue;                            /**< Holds a list of commands waiting to be executed FIFO */
        shared_ptr<condition_variable> recievedCommand;         /**< Flag to notify when a new command is recieved */

        /**
        * The callback function for the server or broker. Used for recieving data from the server/broker in an asynchronous way. 
        * @param data Pointer to the data recieved from the server or broker
        * @return success code, zero if successful
        */              
        virtual int responseCallback(void *data) = 0;
        
        
        public:
            /**
             *  Default destructor 
             */
            virtual ~IClientInterface() {}       

            /**
             * Connect to the server or broker being used to distrubute data between the device and client.
             * Pure virtual function to be overriden by concrete class
             * @return success code, zero if successful
             */                   
            virtual int connect() = 0;       

            /**
             * Disconnect from the server or broker.
             * @return success code, zero if successful
             */                       
            virtual int disconnect() = 0;    

            /**
             * Transmits data to the server or broker.
             * @param dp A data package which defines the tree heirarchy of the data and the data to be transmit
             * @see DataPackage Class
             * @return success code, zero if successful
             */                       
            virtual int transmit(URL const &url, string data) const = 0;    

            /**
             * Gets the next pending command in the command que.
             * @return The command structure
             * @see Connector.hpp
             */     
            virtual command nextCommand() = 0;

            /**
             * Query the command que to see if there are any pending commands.
             * @return true of there are pending commands in the ques, else false
             */
            virtual bool isCommand() = 0;
    };
}

#endif