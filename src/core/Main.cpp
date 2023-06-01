#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Controller.hpp"
#include <pthread.h>
#include "Device.hpp"
#include "network/MQTT.hpp"
#include "mqtt/client.h"
#include <mutex>
#include <condition_variable>
#include "processes/DevProc.hpp"
#include "MappingBuilder.hpp"

using namespace std;
using namespace nectar::hardware;
using namespace nectar::core;
using namespace nectar::network;
using namespace nectar::processes;

/*********************************************************************************
 * TODO
 * 
 * -Implement reading the desctiption of the control process
 * -Mappingbuilder should be SystemBuilder and be able to build all the processess and modules
 * -Create an iterator for all active process returned by the Device
 * -Create a class for a Logger wrapper
 * -URL parser segmentation faults when there is not leading '/'
 * -Controller needs to understand the mapping of nodes so insert order doesnt matter 
 * 
 * *********************************************************************************/

int main()
{
    Device *machine = Device::getInstance();
    Controller controller;
    shared_ptr<condition_variable> cv = make_shared<condition_variable>();
    mutex lck; 
    MQTT clientConnection(cv);
    command args;
    double dRet;
    int iRet;
    std::vector<unsigned int> allActiveProcesses;           //intialise container for all active processes so that they can be shut down
    std::vector<unsigned int>::iterator activeProcessItr;   //iterator for the active processess
    Mapping const map = MappingBuilder::build();            //build the mapping of the device. See "MappingBuilder.cpp"
    URL const *urlBuffer;                                   //Holds a reference to a valid URL created by the MapBuilder
  
    /* 0. Initialise the Brew Modules and processes */
    //Modules
    std::vector<std::pair<std::string, std::string>> hltNodes = 
    { 
        make_pair("primary", "probe0"), 
        make_pair("primary", "load_cell0"), 
        make_pair("element", "switch0"), 
        make_pair("pump", "switch1") 
    };
    BrewModule *hlt = machine->newModule(map.machine("/device/hlt"), "HLT", hltNodes);

    std::vector<std::pair<std::string, std::string>> mashNodes = 
    { 
        make_pair("primary", "probe1"), 
        make_pair("primary", "load_cell1"), 
        make_pair("pump", "switch2") 
    };
    BrewModule *mash = machine->newModule(map.machine("/device/mash"), "MASH", mashNodes);

    std::vector<std::pair<std::string, std::string>> kettleNodes = 
    { 
        make_pair("primary", "probe2"), 
        make_pair("primary", "load_cell2"),
        make_pair("element", "switch3") 
    };
    BrewModule *kettle = machine->newModule(map.machine("/device/kettle"), "Kettle", kettleNodes);

    //Processes HLT
    CalibrateScale p0(map.machine("/process/calibrate_hlt"), "Calibrate the HLT's load cell"); 
        p0.setContext(hlt->moduleID);
    MeasureTemperature p1(map.machine("/process/measure_temp_hlt"), "Measure the HLT's temperature");   
        p1.setContext(hlt->moduleID);   
        p1.setPidParams(0, 0, 0, 0, 1000);
    MeasureScale p2(map.machine("/process/measure_weight_hlt"), "Measure the HLT's weight");    
        p2.setContext(hlt->moduleID);   
        p2.setPidParams(0, 0, 0, 0, 1000);
    SwitchElement p3(map.machine("/process/manual_element_hlt"), "Set the element power manually"); 
        p3.setContext(hlt->moduleID);
    SwitchPump p4(map.machine("/process/manual_pump_hlt"), "Set the pump power manually");  
        p4.setContext(hlt->moduleID);
    ControlTemperature p5(map.machine("/process/control_temp_hlt"), "Control the temperature of the HLT's contents");   
        p5.setContext(hlt->moduleID);   
        p5.setPidParams(30.5, 5, 2, 1, 4000);
    ControlWeight p6(map.machine("/process/control_weight_hlt"), "Control the water level of the HLT");
        p6.setContext(hlt->moduleID);
        p6.setPidParams(0, 1, 1, 1, 500);
    
    //Process MASH
    CalibrateScale p7(map.machine("/process/calibrate_mash"), ""); 
        p7.setContext(mash->moduleID);
    MeasureTemperature p8(map.machine("/process/measure_temp_mash"), "Measure the MASH temperature");   
        p8.setContext(mash->moduleID); 
        p8.setPidParams(0, 0, 0, 0, 1000);
    MeasureScale p9(map.machine("/process/measure_weight_mash"), "Measure the MASH weight"); 
        p9.setContext(mash->moduleID); 
        p9.setPidParams(0, 0, 0, 0, 1000);
    SwitchPump p10(map.machine("/process/manual_pump_mash"), "Set the pump power manually"); 
        p10.setContext(mash->moduleID);
    ControlWeight p11(map.machine("/process/control_weight_mash"), "Control the weight of the Mash Tun"); 
        p11.setContext(mash->moduleID); 
        p11.setPidParams(0, 1, 1, 1, 500);
    
    //Process KETTLE
    CalibrateScale p12(map.machine("/process/calibrate_kettle"), "");     
        p12.setContext(kettle->moduleID); 
    MeasureTemperature p13(map.machine("/process/measure_temp_kettle"), "Measure the kettle's temperature"); 
        p13.setContext(kettle->moduleID); 
        p13.setPidParams(0, 0, 0, 0, 1000);
    MeasureScale p14(map.machine("/process/measure_weight_kettle"), "Measure the Kettle's weight"); 
        p14.setContext(kettle->moduleID); 
        p14.setPidParams(0, 0, 0, 0, 1000);
    Boil p15(map.machine("/process/boil_kettle"), "Boil the kettle"); 
        p15.setContext(kettle->moduleID); 
        p15.setPidParams(100, 1, 1, 1, 10000);
    SwitchElement p16(map.machine("/process/manual_element_kettle"), "Manual switch the kettle's element");
        p16.setContext(kettle->moduleID);
    ControlWeight p17(map.machine("/process/control_weight_kettle"), "Control the weight of the Kettle");
        p17.setContext(kettle->moduleID);
        p17.setPidParams(0, 1, 1, 1, 500);

    /* 1. Connect to the broker to recieve commands */
    clientConnection.connect();

    /* Main Loop */
    args.action = Controller::CMD_NULL;            //ensure the first command is a null command to prevent undefind behavour
    do
    {       
        //     2. Check for pending commands
        unique_lock<mutex> lk(lck);                 //aquire a lock for the conditional wait
        while(!clientConnection.isCommand())
            cv->wait(lk);                           //wait for the next command to come through

        args = clientConnection.nextCommand();      //get the next command in the list 

        //Try and get an existing URL object mapped to the url string. If not found do nothing and continue.
        try
        {
            urlBuffer = &map.retrieve(args.url);
        }
        catch(const out_of_range &e)
        {
            std::cerr << "URL: " << args.url << " is not valid" << '\n';
            continue;
        }
        
        //Print out for debugging
        cout << "Arg[action] " << args.action  << '\n';
        cout << "Args[url] " << args.url << '\n';
        cout << "Arg[value] " << args.value  << '\n';

        //     3. Execute command 
        switch (args.action)
            {
                case Controller::CMD_NULL:
                    cout << "Command List:" << '\n';
                    cout << "exit   : q" << '\n';
                    cout << "start  : x=[value]" << '\n';
                    cout << "stop   : s" << '\n';
                    cout << "read   : r" << '\n';
                    cout << "write  : w=[value]" << '\n';
                    continue;

                case Controller::CMD_EXIT:
                    /* Stop all the running threads before exiting */
                    allActiveProcesses = machine->getAllControlProcesses();
                    
                    for(activeProcessItr = allActiveProcesses.begin(); activeProcessItr != allActiveProcesses.end(); activeProcessItr++)
                    {
                        controller.stop(*activeProcessItr);
                    }
                    goto exit;

                case Controller::CMD_START:
                    /* create a new thread to run the control process */
                    if(!urlBuffer->isRunnable())
                    {
                        continue;
                    }
                    if(controller.start(*urlBuffer, args.value) != 0)  //if not successfuly
                    {
                        cout << "failed to start process: " << *urlBuffer << endl;
                    }
                    continue;

                case Controller::CMD_STOP:
                    if(!urlBuffer->isRunnable())
                    {
                        continue;
                    }
                    if(controller.stop(*urlBuffer) != 0)
                    {
                        cout << "failed to stop process: " << *urlBuffer << '\n';
                    }
                    continue;

                case Controller::CMD_READ:
                    if(controller.read(*urlBuffer, map, dRet) == 0)
                    {
                        cout << "URL Value: " << dRet << endl;
                        goto transmit;
                    }
                    else
                    {
                        continue;
                    }

                case Controller::CMD_WRITE:
                    if(!urlBuffer->isWriteable())
                    {
                        continue;
                    }
                    if((iRet = controller.write(*urlBuffer->getParts().module, *urlBuffer->getParts().node, args.value)) != 0)
                    {
                        cout << "Parameter write failed: " << iRet << endl;
                    }
                    continue;

                default:
                    cout << "Invalid CMD " << endl;
                    continue;
            }
            
            //     4. Transmit command results
            transmit:
            clientConnection.transmit(*urlBuffer, to_string(dRet));
            continue;

            /* jump to the exit */
            exit:
            ;

        /* Loop again */
    } while (args.action != Controller::CMD_EXIT);

    /* Save settings */

    /* Change device state to off */ 
    clientConnection.disconnect();
    
    return 0;
}