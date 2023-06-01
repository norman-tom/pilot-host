#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

namespace nectar::core
{
    class Process
    {
        public:
            /* Null process - reserved */
            static const unsigned int ID_NULL = 0x00;
            /* Code to identify the parameters */
            static const unsigned int PARAM_NULL = 0x00;
            static const unsigned int PARAM_DESC = 0x01;
            static const unsigned int PARAM_SP = 0x02;
            static const unsigned int PARAM_KP = 0x03;
            static const unsigned int PARAM_KI = 0x04;
            static const unsigned int PARAM_KD = 0x05;
            static const unsigned int PARAM_TS = 0x06;

        private:
            condition_variable cv;
            bool running = false;
            bool wakeRequested = false;
            bool stopRequested = false;
            thread th;

        protected:
            mutex mux;
            unsigned int processCode;
            string description;
            double argument;

        protected:
            Process() { };
        
        public:
            /* Copy constructors and Move constructors explicitly used because we dont want to copy the mutex */
            Process(const Process &obj)
            {
                processCode = obj.processCode;
                description = obj.description;
                wakeRequested = obj.wakeRequested;
                stopRequested = obj.stopRequested;
            }

            Process& operator=(const Process&& obj)
            {
                processCode = obj.processCode;
                description = obj.description;    
                wakeRequested = obj.wakeRequested;
                stopRequested = obj.stopRequested;

                return *this;
            };

            Process(unsigned int processCode, string description);

            ~Process();

            /**
             * @brief Start the process
             */
            void start();

            /**
             * @brief Request that the process be stopped. Super class will need to handle the stop request to ensure the exit is graceful
             */
            void stop();

            /**
             * @brief Give an argument to the run command
             * @param arg The argument
             */
            void setArg(double arg);

            /**
             * @brief Is the thread process running
             */
            bool isRunning();

            /**
             * @brief The abstract function Run implements the process's main event loop. This function needs to implemented by the sub class of Process class
             */
            virtual void run() = 0;

        protected:
            /**
             * @brief Getter for the argument given for the run method
             * @return The argumnet
             */
            double getArg();

            /**
             * @brief Put the process to sleep for specified amount of time
             * @param ms The time in milliseconds to sleep
             */
            void sleepProcess(long ms);

            /**
            * @brief Request the process wake from it's wait cycle to do some work
            */
            void wakeProcess();

            /**
             * @brief Has the process been requested to stop
             * @return True if process should stop
             */
            bool shouldStop();
        
            /**
             * @brief Has the process been requested to wake up
             * @return True if the process should wake
             */
            bool shouldWake();
    };
}

#endif