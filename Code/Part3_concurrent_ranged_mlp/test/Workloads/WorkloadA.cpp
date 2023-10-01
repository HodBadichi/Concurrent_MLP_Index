#include "WorkloadA.h"
#include "common.h"
#include "WorkloadInterface.h"
#include "LightRangeGenerator.h"

namespace WorkloadA {
    vector<unsigned long> ranges({1, 256, 256*256*5, 256*256*256*43, 256ULL*256*256*256*3, 256ULL*256*256*256*256*256*3});
    vector<double> range_dist({0.6,0.2,0.1, 0.05, 0.025, 0.025});
    const int COMMON_N = 100000;
    const int COMMON_Q = 1000000;
    const int COMMON_SIZE = 8*(COMMON_N + COMMON_Q);
    WorkloadUInt64 GenWorkload16MLowerBound() {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        LightRandomGenerator LGR(NO_LARGE_RANGES, NO_LARGE_GAPS);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            workload.initialValues[i] = *LGR.Next();
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;
            if (rand() % 4 != 0) {
                Range val = workload.initialValues[rand() % N];
                if (val.second == val.first)
                    workload.operations[i].node.key = val.first;
                else
                    workload.operations[i].node.key = val.first + rand() % (val.second - val.first);
            } else {
                uint64_t key = 0;
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 64 + 32;
                }
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 5 + 48;
                }
                workload.operations[i].node.key = key;
            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkload16MExists() {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        LightRandomGenerator LGR(NO_LARGE_RANGES, NO_LARGE_GAPS);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);

        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            Range *R = LGR.Next();
            workload.initialValues[i] = *R;
            assert(workload.initialValues[i].second >= workload.initialValues[i].first);
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::EXIST;
            if (rand() % 4 != 0) {
                Range val = workload.initialValues[rand() % N];
                workload.operations[i].node.key = val.first + rand() % (val.second - val.first);

            } else {
                uint64_t key = 0;
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 64 + 32;
                }
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 5 + 48;
                }
                workload.operations[i].node.key = key;
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
        workload.initialValues[0].first = 0XAABBCCDDAABBCCDD;
        workload.initialValues[0].second = 0XAABBCCDDAABBCCDD;

        workload.operations[0].node.key = 0XAABBCCDDAABBCCDD;
        workload.operations[0].type = WorkloadOperationType::EXIST;


        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkLoadEmail() {
        const int N = 3;
        const int Q = 4;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        workload.initialValues[0].first = 0X123456789ABCDEF0;
        workload.initialValues[0].second = 0X123456789ABCDEF0;

        workload.initialValues[1].first = 0x1234AAAAAAAAAAAA;
        workload.initialValues[1].second = 0x1234AAAAAAAAAAAA;

        workload.initialValues[2].first = 0x12BBBBBBBBBBBBBB;
        workload.initialValues[2].second = 0x12BBBBBBBBBBBBBB;

        workload.operations[0].node.key = 0XAABBCCDDAABBCCDD;
        workload.operations[0].type = WorkloadOperationType::EXIST;
        workload.operations[1].node.key = 0x1234AAAAAAAAAAAA;
        workload.operations[1].type = WorkloadOperationType::EXIST;
        workload.operations[2].node.key = 0x12BBBBBBBBBBBBBB;
        workload.operations[2].type = WorkloadOperationType::EXIST;

        workload.operations[3].node.key = 2050;
        workload.operations[3].type = WorkloadOperationType::LOWER_BOUND;

        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenWorkLoadEmailLB() {
        const int N = 3;
        const int Q = 3;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        workload.initialValues[0].first = 0X123456789ABCDEF0;
        workload.initialValues[0].second = 0X123456789ABCDEF0;

        workload.initialValues[1].first = 0x1234AAAAAAAAAAAA;
        workload.initialValues[1].second = 0x1234AAAAAAAAAAAA;

        workload.initialValues[2].first = 0x12BBBBBBBBBBBBBB;
        workload.initialValues[2].first = 0x12BBBBBBBBBBBBBB;

        workload.operations[0].node.key = 0X123456789ABCDEF0;
        workload.operations[0].type = WorkloadOperationType::LOWER_BOUND;
        workload.operations[1].node.key = 0x1234AAAAAAAAAA00;
        workload.operations[1].type = WorkloadOperationType::LOWER_BOUND;
        workload.operations[2].node.key = 0x5600000000000001;
        workload.operations[2].type = WorkloadOperationType::LOWER_BOUND;


        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }


    WorkloadUInt64 GenWorkload16MInsertLowerBoundParllel() {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        LightRandomGenerator LGR(NO_LARGE_RANGES, NO_LARGE_GAPS);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            workload.initialValues[i] = *LGR.Next();
        }
        rep(i, 0, Q - 1) {

            if (rand() % 5 != 0) {
                workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;

                if (rand() % 4 == 0) {
                    workload.operations[i].node.key = workload.initialValues[rand() % N].first;
                } else {
#define MASK_HIGH_PART  0x0000ffff
#define MASK_LOW_PART   0xffffffff

                    uint64_t low_part = rand() & MASK_LOW_PART;
                    uint64_t high_part = rand() & MASK_HIGH_PART;
                    uint64_t key = ((uint64_t) high_part << 32) | low_part;
                    workload.operations[i].node.key = key;
                }

            } else {
                workload.operations[i].type = WorkloadOperationType::INSERT_RANGE;
                Range val = *LGR.Next();
                workload.operations[i].node.range.first = val.first;
                workload.operations[i].node.range.second = val.second;

            }


        }


        workload.PopulateExpectedResultsUsingStdSet();

        return workload;
    }

    WorkloadUInt64 GenWorkload16MInsertExistsParallel() {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        LightRandomGenerator LGR(NO_LARGE_RANGES, NO_LARGE_GAPS);
	LGR.SetRsbSeed(41);
	LGR.SetGbrSeed(82);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            Range *R = LGR.Next();
            workload.initialValues[i] = *R;
        }
        rep(i, 0, Q - 1) {

            if (rand() % 5 != 0) {
                workload.operations[i].type = WorkloadOperationType::EXIST;
                if (rand() % 4 == 0) {
                    workload.operations[i].node.key = workload.initialValues[rand() % N].first;
                } else {
#define MASK_HIGH_PART  0x0000ffff
#define MASK_LOW_PART   0xffffffff

                    uint64_t low_part = rand() & MASK_LOW_PART;
                    uint64_t high_part = rand() & MASK_HIGH_PART;
                    uint64_t key = ((uint64_t) high_part << 32) | low_part;
                    workload.operations[i].node.key = key;
                }

            } else {
                workload.operations[i].type = WorkloadOperationType::INSERT_RANGE;
                Range *R = LGR.Next();
                workload.operations[i].node.range.first = R->first;
                workload.operations[i].node.range.second = R->second;
            }


        }


        workload.PopulateExpectedResultsUsingStdSet();

        return workload;
    }

}    // namespace WorkloadA

