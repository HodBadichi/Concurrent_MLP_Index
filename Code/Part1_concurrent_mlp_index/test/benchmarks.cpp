//
// Created by hod on 8/15/23.
//
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "MlpSetUInt64.h"
#include "Workloads/BenchMarkWorkloads.h"
#include "Workloads/WorkloadInterface.h"
#include <thread>


int THREADS_NUM = 10;

void NO_INLINE MlpSetExecuteWorkloadThread(WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms, const int start_range,
                                           const int end_range) {

    rep(i, start_range, end_range - 1) {
        uint64_t answer;
        switch (workload->operations[i].type) {
            case WorkloadOperationType::INSERT: {
                answer = ms->Insert(workload->operations[i].key);
                break;
            }
            case WorkloadOperationType::EXIST: {
                answer = ms->Exist(workload->operations[i].key);
                break;
            }
            case WorkloadOperationType::LOWER_BOUND: {
                bool found;
                answer = ms->LowerBound(workload->operations[i].key, found);
                break;
            }
        }
    }
}

void processChunk(const int thread_num, const int chunk_size, WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms) {
    const int start_range = thread_num * chunk_size;
    const int end_range = (thread_num + 1) * chunk_size;
    MlpSetExecuteWorkloadThread(workload, ms, start_range, end_range);
}

/*
 * `WorkLoad_2M_EXIST` Benchmark:
 *                                  - The DS is initialized with 80M elements before running any operation.
 *                                  - The Benchmarks preforms 2M `Exist` queries.
 *                                  - Key distribution is structured such that the initial 6 out of 8 bytes exhibit limited variability,
 *                                    while the final 2 bytes possess a broader range of possibilities.
 *                                  - 75% out of the `Exist` queries are positive the rest are sampled from the Key distribution above.
 */
TEST(MlpSetUInt64, WorkLoad_2M_EXIST) {
    double result;
    std::vector<std::thread> threads;
    std::cout << "Running 'WorkLoad_2M_EXIST' with " << THREADS_NUM << " Threads" << std::endl;
    const int num_of_threads = THREADS_NUM;

    WorkloadUInt64 workload = BenchMarkWorkloads::GenWorkload2M_EXISTS();
    Auto(workload.FreeMemory());
    const int chunk_size = workload.numOperations / num_of_threads;

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + + workload.numOperations+1000);


    rep(i, 0, workload.numInitialValues - 1) {
        ms.Insert(workload.initialValues[i]);
    }

    printf("MlpSet running ops..\n");
    {
        AutoTimer timer(&result);
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }
    std::cout << "MlpSet ops/ml: " << (workload.numOperations / result) / 1e6 << "M" << std::endl;

}

/*
 * `Workload_2M_INSERT` Benchmark:
 *                                  - The DS is initialized with 10M elements before running any operation.
 *                                  - The Benchmarks preforms 2M `Insert` queries.
 *                                  - Key distribution is structured such that the initial 6 out of 8 bytes exhibit limited variability,
 *                                    while the final 2 bytes possess a broader range of possibilities.
 *                                  - 25% out of the `Insert` queries are false and the rest are sampled from the Key distribution above.
 */
TEST(MlpSetUInt64, WorkLoad_2M_INSERT) {
    std::vector<std::thread> threads2;
    double result;
    const int num_of_threads = THREADS_NUM;
    std::cout << "Running 'WorkLoad_2M_INSERT' with " << THREADS_NUM << " Threads" << std::endl;
    WorkloadUInt64 workload = BenchMarkWorkloads::GenWorkload2M_INSERT();
    Auto(workload.FreeMemory());
    const int ops_chunk_size = workload.numOperations / num_of_threads;

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + workload.numOperations+1000);


//    rep(i, 0, workload.numInitialValues - 1) {
//        ms.Insert(workload.initialValues[i]);
//    }

    printf("MlpSet running ops..\n");
    {
        AutoTimer timer(&result);

        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads2.emplace_back(processChunk, thread_num, ops_chunk_size, &workload, &ms);
        }

        for (auto &thread: threads2) {
            thread.join();
        }
    }
    std::cout << "MlpSet ops/ml: " << (workload.numOperations / result) / 1e6 << "M" << std::endl;
}

/*
 * `WorkLoad_2M_LowerBound` Benchmark:
 *                                  - The DS is initialized with 80M elements before running any operation.
 *                                  - The Benchmarks preforms 2M `LowerBound` queries.
 *                                  - Key distribution is structured such that the initial 6 out of 8 bytes exhibit limited variability,
 *                                    while the final 2 bytes possess a broader range of possibilities.
 *                                  - 25% out of the `LowerBound` queries are existing keys the rest are sampled from the Key distribution above.
 */
TEST(MlpSetUInt64, WorkLoad_2M_LOWERBOUND) {
    double result;
    std::vector<std::thread> threads;
    std::cout << "Running 'WorkLoad_2M_LOWERBOUND' with " << THREADS_NUM << " Threads" << std::endl;
    const int num_of_threads = THREADS_NUM;

    WorkloadUInt64 workload = BenchMarkWorkloads::GenWorkload2M_LOWERBOUND();
    Auto(workload.FreeMemory());
    const int chunk_size = workload.numOperations / num_of_threads;

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + workload.numOperations + 1000);


    rep(i, 0, workload.numInitialValues - 1) {
        ms.Insert(workload.initialValues[i]);
    }

    printf("MlpSet running ops..\n");
    {
        AutoTimer timer(&result);
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }
    std::cout << "MlpSet ops/ml: " << (workload.numOperations / result) / 1e6 << "M" << std::endl;
}

class Color {
public:
    static const std::string Reset;
    static const std::string Red;
    static const std::string Green;
};

const std::string Color::Reset = "\033[0m";
const std::string Color::Red = "\033[31m";
const std::string Color::Green = "\033[32m";

class TestNamePrinter : public testing::EmptyTestEventListener {
    void OnTestStart(const testing::TestInfo &test_info) override {
        std::cout << Color::Green << "[==========] "
                  << "Running test: " << test_info.name() << Color::Reset << std::endl;
        std::cout << "\n" << std::endl;
    }


    void OnTestEnd(const testing::TestInfo &test_info) override {
        if (test_info.result()->Passed()) {
            std::cout << Color::Green << "[ PASSED   ]" << "\n" << std::endl;
        } else {
            std::cout << Color::Red << "[ FAILED   ] " << "\n" << std::endl;
        }
        std::cout << test_info.name() << Color::Reset << std::endl;
    }
};

// Function to parse custom command-line arguments
void ParseCustomArguments(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            THREADS_NUM = atoi(argv[i + 1]);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse custom arguments

    ParseCustomArguments(argc, argv);

    // Remove the custom arguments from argv for Google Test
    int new_argc = 0;
    for (int i = 0; i < argc; ++i) {
        if (strncmp(argv[i], "-t", 2) != 0) {
            argv[new_argc] = argv[i];
            ++new_argc;
        }
    }
    printf("Setting InitGoogleTest ...");
    // Initialize Google Test
    ::testing::InitGoogleTest(&new_argc, argv);
    printf("Done Setting InitGoogleTest ...");

    // Attach the custom test listener
    ::testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new TestNamePrinter);

    return RUN_ALL_TESTS();
}
