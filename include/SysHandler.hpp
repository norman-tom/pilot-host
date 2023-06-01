#ifndef SYS_HANDLER_H
#define SYS_HANDLER_H

#include <vector>
#include <string>
#include <mutex>

#define TEMPERATURE_PATH "/sys/class/nectar_temperature/"
#define LOAD_PATH "/sys/class/nectar_weight/"
#define SWITCH_PATH "/sys/class/nectar_switching/"

using namespace std;

namespace nectar::core
{
    struct sysNode {
        int fd;
        string attribute;
        string name;
    };

    class SysHandler
    {
        private:
            int sysNodeCount = 0;
            std::mutex mutex_;

        protected:
            /**
             * Gets the latest sensor value or attribute from sysfs
             * @param nodeIndex the index of the value/attribute the in the node
             * @param base The number base of the value/attribute in sysfs (hex or ten) used for string to long convertion.
             * @return The value of the value or attribute.
             */
            long updateNode(int nodeIndex, int base);

        public:
            string basePath;
            vector<struct sysNode> sysNodes;
            SysHandler *parent;
            
            SysHandler(void);
            SysHandler(string basePath);
            SysHandler(string basePath, SysHandler *parent);

            /* Copy constructors and Move constructors explicitly used because we dont want to copy the mutex */
            SysHandler(const SysHandler &obj);
            SysHandler& operator=(const SysHandler&& obj)
            {
                basePath = obj.basePath;
                parent = obj.parent;    
                return *this;
            };

            /**
             * @brief Adds a new attribute to the parent object. Attributes are the Sysfs attributes derived from the hardware's driver.
             * @param attribute The attribute that is to be added to the object. This needs to match the attribute naming in the driver's class.
             * @param name Give the attribute a user defined optional name. Default is to use the attribute as the name.
             * @return The index of the attribute within the object. This index is used to call the attribute after it has been added.
             */
            int addSysNode(string attribute);
            int addSysNode(string attribute, string name);
            
            void removeSysNode(int index);
            int openSysNode(int index, int permissions);
            int readSysNode(int index, char *buf, size_t nbytes);
            int writeSysNode(int index, const char *buf, size_t nbytes);
            int closeSysNode(int index);
    };
}
#endif