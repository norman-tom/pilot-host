set(CMAKE_INSTALL_RPATH "/home/debian/session")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(TEST "nectar_test")

add_executable("${TEST}")
add_test(nectar_test "${TEST}" WORKING_DIRECTORY tests/bin)

target_include_directories("${TEST}" PRIVATE ./include)

target_sources("${TEST}" PRIVATE Test.cpp)
target_sources("${TEST}" PRIVATE TestProcesses.cpp)
target_sources("${TEST}" PRIVATE ../src/core/Device.cpp)
target_sources("${TEST}" PRIVATE ../src/core/SysHandler.cpp)
target_sources("${TEST}" PRIVATE ../src/core/Controller.cpp)
target_sources("${TEST}" PRIVATE ../src/devices/BrewModule.cpp)
target_sources("${TEST}" PRIVATE ../src/devices/TemperatureNode.cpp)
target_sources("${TEST}" PRIVATE ../src/devices/LoadCellNode.cpp)
target_sources("${TEST}" PRIVATE ../src/devices/SwitchNode.cpp)
target_sources("${TEST}" PRIVATE ../src/plant/Plant.cpp)

target_link_libraries("${TEST}" nectariio)
target_link_libraries("${TEST}" pid)

#Need to set the PThreads library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries("${TEST}" Threads::Threads)

#install targets
install(TARGETS "${TEST}" DESTINATION tests/bin)
