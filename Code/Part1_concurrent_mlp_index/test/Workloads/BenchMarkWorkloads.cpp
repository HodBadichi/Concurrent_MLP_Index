#include "BenchMarkWorkloads.h"
#include "common.h"
#include "WorkloadInterface.h"

namespace BenchMarkWorkloads {

    WorkloadUInt64 GenWorkload2M_EXISTS() {
        const int N = 200000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t key = 0;
            rep(k, 2, 7) {
                key = key * 256 + rand() % 6 + 48;
            }
            rep(k, 0, 1) {
                key = key * 256 + rand() % 96 + 32;
            }
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::EXIST;
            if (rand() % 4 != 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
                uint64_t key = 0;
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 6 + 48;
                }
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 96 + 32;
                }
                workload.operations[i].key = key;
            }
        }
        return workload;
    }

    WorkloadUInt64 GenWorkload2M_INSERT() {
        const int N = 200000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t key = 0;
            rep(k, 2, 7) {
                key = key * 256 + rand() % 6 + 48;
            }
            rep(k, 0, 1) {
                key = key * 256 + rand() % 96 + 32;
            }
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::INSERT;
            if (rand() % 4 == 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
                uint64_t key = 0;
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 6 + 48;
                }
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 96 + 32;
                }
                workload.operations[i].key = key;
            }
        }
        return workload;
    }

    WorkloadUInt64 GenWorkload2M_LOWERBOUND() {
        const int N = 200000;
        const int Q = 2000000;
        WorkloadUInt64 workload;
        workload.AllocateMemory(N, Q);
        rep(i, 0, N - 1) {
            uint64_t key = 0;
            rep(k, 2, 7) {
                key = key * 256 + rand() % 6 + 48;
            }
            rep(k, 0, 1) {
                key = key * 256 + rand() % 96 + 32;
            }
            workload.initialValues[i] = key;
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;
            if (rand() % 4 == 0) {
                workload.operations[i].key = workload.initialValues[rand() % N];
            } else {
                uint64_t key = 0;
                rep(k, 2, 7) {
                    key = key * 256 + rand() % 6 + 48;
                }
                rep(k, 0, 1) {
                    key = key * 256 + rand() % 96 + 32;
                }
                workload.operations[i].key = key;
            }
        }
        return workload;
    }
}    // namespace BenchMarkWorkloads

