# nectar-user
Part of the FUBru Project. Nectar-User is a linux user space application for that is used fo settings, controlling and publishing data for the FUBru system. This sits on the embeded system and handles information comming in from the client application and hardware drivers. 

## Cross Compiling MQTT C++ library
1. Followed instruction on Eclipse MQTT build page:

*https://github.com/eclipse/paho.mqtt.cpp*

### Building the C library

2. Copied the libssl and libcrypto shared libraries from the target machine to the host machine directory "/usr/lib/arm-linux-gnueabihf/"

3. Copy the *openssl* header files from the target device to the cross compiler's folder on the host machine

*/home/tom/onedrive_normco/projects/01_nectar/code/toolchain/compiler/arm-linux-gnueabihf/include*

4. Had to change the src/CMakeLists.txt to take the shared libraries from the *arm-linux-gnueabihf* folder. This was achieved by adding *link_directories(/usr/lib/arm-linux-gnueabihf)* after the *IF (PAHO_WITH_SSL)* and hard coding the libraries names in the *TARGET_LINK_LIBRARIES* command rather than using the inbuilt CMake OpenSSL::SSL and OpenSSL::Crypto

5. Used the following build configiuration:

*cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=OFF -DPAHO_BUILD_SHARED=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON / 
-DCMAKE_INSTALL_PREFIX='/home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/' / 
-DCMAKE_C_COMPILER='/home/tom/onedrive_normco/projects/01_nectar/code/toolchain/compiler/bin/arm-linux-gnueabihf-gcc' / 
-DOPENSSL_ROOT_DIR='usr/lib/arm-linux-gnueabihf'* 

6. build with *cmake --build build/ --target install*

### Building CPP Library

7. Update the CMakeList.txt file to understand where the Paho.Mqtt.C shared libraries and headers where. These are as per the above DCMAKE_INSTALL_PREFIX argument

8. Updated the CMakeList.txt files so that the CPP headers and libraries would install in my project directory and not in the default location on the host machine

9. Use the following arguments

*cmake -Bbuild -H. -DPAHO_BUILD_STATIC=OFF -DPAHO_BUILD_SHARED=ON -DPAHO_BUILD_TESTS=OFF 
-DCMAKE_PREFIX_PATH='/home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/' 
-DCMAKE_CXX_COMPILER='/home/tom/onedrive_normco/projects/01_nectar/code/toolchain/compiler/bin/arm-linux-gnueabihf-g++'*

10. build with 

*sudo cmake --build build/ --target install*

### Install the MQTT Libraries

When the libraries above have been compliled; they will need to be installed, along with the OpenSSL (preinstalled) to the embeded device.

Copy:
-libpaho-mqtt3a.*
-libpaho-mqtt3as.*
-libpaho-mqtt3c.*
-libpaho-mqtt3cs.*
-libpaho-mqttpp3.*

library files to the location:

*/usr/local/lib/* one the embeded device

Also make sure the OpenSSL (libssl) library and Crypto (libcrypto) library are installed in */usr/lib/arm-linux-gneuabihf/*

Header files where also copied to */usr/local/include/*

RPATH of the program is set to */usr/local/lib* to find the shared libraries



