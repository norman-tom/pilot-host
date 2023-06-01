#ifndef CONTEXTED_PROCESS_H
#define CONTEXTED_PROCESS_H

#include <mutex>

namespace nectar::core
{
    /**
     * A Context process is a process which can be given further information to use in the process algorithm. That is, above and beyond a single argument.
     * The concrete class of the process will need to inherit the ContextProcess Class to add context functionality to the process.  
     * Generic class so that the context can be any data structure or type neccessary to give the full context to the process. 
     */
    template<typename T>
    class ContextedProcess
    {
        protected:
            T context;          /**< The context of the process algorithm */

        private:
            std::mutex mux;     /**< Mutext for thread safe getting and setting of the context */

        protected:
            /**
             * Get the context for this process
             * @return Context of type T
             */
            T getContext();

        public:
            /**
             * Sets the context for this process
             * @param context The context
             */
            void setContext(T context);

    };

    template<typename T>
    void ContextedProcess<T>::setContext(T context)
    {
        lock_guard<mutex> lk(mux);
        ContextedProcess<T>::context = context;            
    }

    template<typename T>
    T ContextedProcess<T>::getContext()
    {
        lock_guard<mutex> lk(mux);
        return ContextedProcess<T>::context;
    }
}

#endif