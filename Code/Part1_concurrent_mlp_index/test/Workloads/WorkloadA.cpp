#include "WorkloadA.h"
#include "common.h"
#include "WorkloadInterface.h"

namespace WorkloadA {
    WorkloadUInt64 GenWorkload16MLowerBound()
    {
        const int N = 1600000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t key = 0;
            rep(k, 0, 1) {
                key = key * 256 + rand() % 64 + 32;
            }
            rep(k, 2, 7) {
                key = key * 256 + rand() % 5 + 48;
            }
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;
            if (rand() % 4 != 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
                uint64_t key = 0;
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 64 + 32;
                }
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 5 + 48;
                }
                workload.operations[i].key = key;
            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }
    WorkloadUInt64 GenWorkload16MExists() {
        const int N = 1600000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t key = 0;
            rep(k, 0, 1) {
                key = key * 256 + rand() % 64 + 32;
            }
            rep(k, 2, 7) {
                key = key * 256 + rand() % 5 + 48;
            }
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::EXIST;
            if (rand() % 4 != 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
                uint64_t key = 0;
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 64 + 32;
                }
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 5 + 48;
                }
                workload.operations[i].key = key;
            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkload1() {
        const int N = 1;
        const int Q = 1;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        workload.initialValues[0] = 0XAABBCCDDAABBCCDD;

        workload.operations[0].key = 0XAABBCCDDAABBCCDD;
        workload.operations[0].type = WorkloadOperationType::EXIST;


        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkLoadEmail() {
        const int N = 3;
        const int Q = 3;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        workload.initialValues[0] = 0X123456789ABCDEF0;
        workload.initialValues[1] = 0x1234AAAAAAAAAAAA;
        workload.initialValues[2] = 0x12BBBBBBBBBBBBBB;

        workload.operations[0].key = 0XAABBCCDDAABBCCDD;
        workload.operations[0].type = WorkloadOperationType::EXIST;
        workload.operations[1].key = 0x1234AAAAAAAAAAAA;
        workload.operations[1].type = WorkloadOperationType::EXIST;
        workload.operations[2].key = 0x12BBBBBBBBBBBBBB;
        workload.operations[2].type = WorkloadOperationType::EXIST;


        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkLoadEmailLB() {
        const int N = 3;
        const int Q = 3;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        workload.initialValues[0] = 0X123456789ABCDEF0;
        workload.initialValues[1] = 0x1234AAAAAAAAAAAA;
        workload.initialValues[2] = 0x12BBBBBBBBBBBBBB;

        workload.operations[0].key = 0X123456789ABCDEF0;
        workload.operations[0].type = WorkloadOperationType::LOWER_BOUND;
        workload.operations[1].key = 0x1234AAAAAAAAAA00;
        workload.operations[1].type = WorkloadOperationType::LOWER_BOUND;
        workload.operations[2].key = 0x5600000000000001;
        workload.operations[2].type = WorkloadOperationType::LOWER_BOUND;


        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkload16MInsertExistsParallel() {
        const int N = 1600000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t low_part = rand();
            uint64_t high_part = rand();
            uint64_t key = ((uint64_t)high_part << 32) | low_part;
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {

            if( rand() % 5 !=0)
            {
                workload.operations[i].type = WorkloadOperationType::EXIST;
            }
            else
            {
                workload.operations[i].type = WorkloadOperationType::INSERT;
            }


            if (rand() % 4 == 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
#define MASK_HIGH_PART  0x0000ffff
#define MASK_LOW_PART   0xffffffff

                //uint64_t key = ((uint64_t)rand() << 32) | rand();
                uint64_t low_part = rand() & MASK_LOW_PART;
                uint64_t high_part = rand() & MASK_HIGH_PART;
                uint64_t key = ((uint64_t)high_part << 32) | low_part;
             /*   uint64_t key = 0;
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 64 + 32;
                }
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 5 + 48;
                }*/
                workload.operations[i].key = key;
            }
        }


        workload.PopulateExpectedResultsUsingStdSet();

        return workload;
    }


    WorkloadUInt64 GenWorkload16MInsertLowerBoundParllel() {
        const int N = 1600000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t low_part = rand();
            uint64_t high_part = rand();
            uint64_t key = ((uint64_t)high_part << 32) | low_part;
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {

            if( rand() % 5 !=0)
            {
                workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;
            }
            else
            {
                workload.operations[i].type = WorkloadOperationType::INSERT;
            }


            if (rand() % 4 == 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
#define MASK_HIGH_PART  0x0000ffff
#define MASK_LOW_PART   0xffffffff

                uint64_t low_part = rand() & MASK_LOW_PART;
                uint64_t high_part = rand() & MASK_HIGH_PART;
                uint64_t key = ((uint64_t)high_part << 32) | low_part;
                workload.operations[i].key = key;
            }
        }


        workload.PopulateExpectedResultsUsingStdSet();

        return workload;
    }
}    // namespace WorkloadA

