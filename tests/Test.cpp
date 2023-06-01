#include "Device.hpp"
#include "Controller.hpp"
#include "TestProcesses.hpp"
#include "Processes.hpp"
#include <unistd.h>
#include <algorithm>

using namespace core;
using namespace devices;
using namespace nectar;
using namespace core;

int main()
{
    Device *machine = Device::getInstance();
    Controller controller;
    std::vector<std::string> nodes = { "probe0", "probe1" };
    BrewModule *bm = machine->newModule(BrewModule::MOD_HLT, "Test Module", nodes);

    /* create the processes */
    control_p *p1 = machine->newControlProcess(tests::TestProcesses::PROCESS_1, "Test process 1", tests::TestProcesses::testProcess_1);
    control_p *p2 = machine->newControlProcess(tests::TestProcesses::PROCESS_2, "Test process 2", tests::TestProcesses::testProcess_2);
    control_p *p3 = machine->newControlProcess(tests::TestProcesses::PROCESS_3, "Test process 3", tests::TestProcesses::testProcess_3);
    control_p *p4 = machine->newControlProcess(tests::TestProcesses::PROCESS_4, "Test process 4", tests::TestProcesses::testProcess_4);
    control_p *p5 = machine->newControlProcess(tests::TestProcesses::PROCESS_5, "Test process 5", tests::TestProcesses::testProcess_5);
    control_p *p6 = machine->newControlProcess(tests::TestProcesses::PROCESS_6, "Test process 6", tests::TestProcesses::testProcess_6);
    control_p *p7 = machine->newControlProcess(tests::TestProcesses::PROCESS_7, "Test process 7", tests::TestProcesses::testProcess_7);
    control_p *p8 = machine->newControlProcess(tests::TestProcesses::PROCESS_8, "Test process 8", tests::TestProcesses::testProcess_8);
    control_p *p9 = machine->newControlProcess(tests::TestProcesses::PROCESS_9, "Test process 9", tests::TestProcesses::testProcess_9);
    control_p *p10 = machine->newControlProcess(tests::TestProcesses::PROCESS_10, "Test process 10", tests::TestProcesses::testProcess_10);

    /* initialise the settings */
    p1->pidParam.ts = 500;
    p1->pMux = PTHREAD_MUTEX_INITIALIZER;
    p1->pCon = PTHREAD_COND_INITIALIZER;

    p2->pidParam.ts = 500;
    p2->pMux = PTHREAD_MUTEX_INITIALIZER;
    p2->pCon = PTHREAD_COND_INITIALIZER;

    p3->pidParam.ts = 500;
    p3->pMux = PTHREAD_MUTEX_INITIALIZER;
    p3->pCon = PTHREAD_COND_INITIALIZER;

    p4->pidParam.ts = 500;
    p4->pMux = PTHREAD_MUTEX_INITIALIZER;
    p4->pCon = PTHREAD_COND_INITIALIZER;

    p5->pidParam.ts = 500;
    p5->pMux = PTHREAD_MUTEX_INITIALIZER;
    p5->pCon = PTHREAD_COND_INITIALIZER;

    p6->pidParam.ts = 500;
    p6->pMux = PTHREAD_MUTEX_INITIALIZER;
    p6->pCon = PTHREAD_COND_INITIALIZER;

    p7->pidParam.ts = 500;
    p7->pMux = PTHREAD_MUTEX_INITIALIZER;
    p7->pCon = PTHREAD_COND_INITIALIZER;

    p8->pidParam.ts = 500;
    p8->pidParam.setPoint = 1;
    p8->pMux = PTHREAD_MUTEX_INITIALIZER;
    p8->pCon = PTHREAD_COND_INITIALIZER;

    p9->pidParam.ts = 500;
    p9->pidParam.setPoint = 1;
    p9->pMux = PTHREAD_MUTEX_INITIALIZER;
    p9->pCon = PTHREAD_COND_INITIALIZER;

    p10->pidParam.ts = 500;
    p10->pidParam.setPoint = 1;
    p10->pMux = PTHREAD_MUTEX_INITIALIZER;
    p10->pCon = PTHREAD_COND_INITIALIZER;
    
    cout << "___Running Ownership Test___" << endl;

    controller.start(tests::TestProcesses::PROCESS_1, NULL);
    controller.start(tests::TestProcesses::PROCESS_2, NULL);
    sleep(1);
    controller.start(tests::TestProcesses::PROCESS_3, NULL);  
    controller.start(tests::TestProcesses::PROCESS_4, NULL);
    sleep(1);
    controller.stop(tests::TestProcesses::PROCESS_3);
    sleep(1);
    controller.start(tests::TestProcesses::PROCESS_2, NULL);
    sleep(1);
    controller.stop(tests::TestProcesses::PROCESS_1);
    controller.stop(tests::TestProcesses::PROCESS_2);
    controller.stop(tests::TestProcesses::PROCESS_3);
    controller.stop(tests::TestProcesses::PROCESS_4);
    sleep(2);

    cout << "___Running Read Concurrency Test___" << endl;
    sleep(2);
    controller.start(tests::TestProcesses::PROCESS_5, NULL);
    controller.start(tests::TestProcesses::PROCESS_6, NULL);
    controller.start(tests::TestProcesses::PROCESS_7, NULL);
    sleep(5);
    controller.stop(tests::TestProcesses::PROCESS_5);
    controller.stop(tests::TestProcesses::PROCESS_6);
    controller.stop(tests::TestProcesses::PROCESS_7);
    sleep(2);

    cout << "___Running Write Concurrency Test___" << endl;
    sleep(2);
    controller.start(tests::TestProcesses::PROCESS_8, NULL);
    controller.start(tests::TestProcesses::PROCESS_9, NULL);
    controller.start(tests::TestProcesses::PROCESS_10, NULL);
    sleep(5);
    controller.stop(tests::TestProcesses::PROCESS_8);
    controller.stop(tests::TestProcesses::PROCESS_9);
    controller.stop(tests::TestProcesses::PROCESS_10);

    return 0;
}