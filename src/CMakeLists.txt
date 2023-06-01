set(EXE "${CMAKE_PROJECT_NAME}")

add_executable("${EXE}")

#add the source files for the main project here
target_sources("${EXE}" PRIVATE core/Main.cpp)
target_sources("${EXE}" PRIVATE core/SysHandler.cpp)
target_sources("${EXE}" PRIVATE core/Controller.cpp)
target_sources("${EXE}" PRIVATE core/Device.cpp)
target_sources("${EXE}" PRIVATE core/Process.cpp)
target_sources("${EXE}" PRIVATE core/PIDProcess.cpp)
target_sources("${EXE}" PRIVATE core/Mapping.cpp)
target_sources("${EXE}" PRIVATE core/MappingBuilder.cpp)
target_sources("${EXE}" PRIVATE core/URL.cpp)
target_sources("${EXE}" PRIVATE hardware/TemperatureNode.cpp)
target_sources("${EXE}" PRIVATE hardware/LoadCellNode.cpp)
target_sources("${EXE}" PRIVATE hardware/SwitchNode.cpp)
target_sources("${EXE}" PRIVATE hardware/BrewModule.cpp)
target_sources("${EXE}" PRIVATE network/MQTT.cpp)
target_sources("${EXE}" PRIVATE topography/URLFactory.cpp)
target_sources("${EXE}" PRIVATE topography/MapNode.cpp)
target_sources("${EXE}" PRIVATE topography/MapTree.cpp)
target_sources("${EXE}" PRIVATE processes/DevProc.cpp)


#link the nectariio library
target_link_libraries("${EXE}" nectariio)
target_link_libraries("${EXE}" pid)

#Point to the MQTT libraries   -- Make this nicer when you can
target_link_libraries("${EXE}" 
    /home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/lib/libpaho-mqttpp3.so
    /home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/lib/libpaho-mqtt3a.so
    /home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/lib/libpaho-mqtt3as.so
    /home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/lib/libpaho-mqtt3c.so
    /home/tom/onedrive_normco/projects/01_nectar/code/nectar-user/bin/extern/usr/local/lib/libpaho-mqtt3cs.so
    /usr/lib/arm-linux-gnueabihf/libcrypto.so
    /usr/lib/arm-linux-gnueabihf/libssl.so)

#Need to set the PThreads library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries("${EXE}" Threads::Threads)

#set the desination of the binary when make install is called
install(TARGETS "${EXE}" DESTINATION bin)