cmake_minimum_required(VERSION 3.19)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 4.19.92)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(tools /home/tom/onedrive_normco/projects/01_nectar/code/toolchain/compiler/bin)
set(CMAKE_C_COMPILER ${tools}/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/arm-linux-gnueabihf-g++)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

project(
    nectar_board 
    VERSION 0.1.0
    DESCRIPTION "user space program for nectar system"
    LANGUAGES CXX
)

#Project include directory
include_directories(include)

#MQTT include directory
include_directories(bin/extern/usr/local/include)

add_subdirectory(lib)
add_subdirectory(src)

#add testing
#enable_testing()
#add_subdirectory(tests)