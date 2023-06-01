#include "Plant.hpp"

#ifndef TEST_PROC_H
#define TEST_PROC_H

namespace nectar::tests
{
    class TestProcesses : plant::Plant
    {
        public:
            static const unsigned int PROCESS_1 = 0x01;
            static const unsigned int PROCESS_2 = 0x02;
            static const unsigned int PROCESS_3 = 0x03;
            static const unsigned int PROCESS_4 = 0x04;
            static const unsigned int PROCESS_5 = 0x05;
            static const unsigned int PROCESS_6 = 0x06;
            static const unsigned int PROCESS_7 = 0x07;
            static const unsigned int PROCESS_8 = 0x08;
            static const unsigned int PROCESS_9 = 0x09;
            static const unsigned int PROCESS_10 = 0x0A;

        public:
            static void *testProcess_1(void *data);
            static void *testProcess_2(void *data);
            static void *testProcess_3(void *data);
            static void *testProcess_4(void *data);
            static void *testProcess_5(void *data);
            static void *testProcess_6(void *data);
            static void *testProcess_7(void *data);
            static void *testProcess_8(void *data);
            static void *testProcess_9(void *data);
            static void *testProcess_10(void *data);
    };
}

#endif