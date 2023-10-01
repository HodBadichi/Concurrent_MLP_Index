//
// Created by hod on 8/15/23.
//
#include "gtest/gtest.h"
#include "MlpSetUInt64.h"
#include "Workloads/WorkloadA.h"
#include "Workloads/WorkloadInterface.h"
#include <thread>


#define THREADS_NUM 5

template<bool enforcedDep>
void NO_INLINE MlpSetExecuteWorkload(WorkloadUInt64 &workload) {
    printf("MlpSet executing workload, enforced dependency = %d\n", (enforcedDep ? 1 : 0));
    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.Insert(workload.initialValues[i]);
        }
    }

#ifdef ENABLE_STATS
    ms.ReportStats();
    ms.ClearStats();
#endif

    printf("MlpSet executing workload..\n");
    {
        AutoTimer timer;
        if (enforcedDep) {
            uint64_t lastAnswer = 0;
            rep(i, 0, workload.numOperations - 1) {
                uint32_t x = workload.operations[i].type;
                x ^= (uint32_t) lastAnswer;
                WorkloadOperationType type = (WorkloadOperationType) x;
                uint64_t realKey = workload.operations[i].key ^ lastAnswer;
                uint64_t answer;
                switch (type) {
                    case WorkloadOperationType::INSERT: {
                        answer = ms.Insert(realKey);
                        break;
                    }
                    case WorkloadOperationType::EXIST: {
                        answer = ms.Exist(realKey);
                        break;
                    }
                    case WorkloadOperationType::LOWER_BOUND: {
                        bool found;
                        answer = ms.LowerBound(realKey, found);
                        break;
                    }
                }
                workload.results[i] = answer;
                lastAnswer = answer;
            }
        } else {
            rep(i, 0, workload.numOperations - 1) {
                uint64_t answer;
                switch (workload.operations[i].type) {
                    case WorkloadOperationType::INSERT: {
                        answer = ms.Insert(workload.operations[i].key);
                        break;
                    }
                    case WorkloadOperationType::EXIST: {
                        answer = ms.Exist(workload.operations[i].key);
                        break;
                    }
                    case WorkloadOperationType::LOWER_BOUND: {
                        bool found;
                        answer = ms.LowerBound(workload.operations[i].key, found);
                        break;
                    }
                }
                workload.results[i] = answer;
            }
        }
    }

#ifdef ENABLE_STATS
    ms.ReportStats();
#endif

    printf("MlpSet workload completed.\n");
}

template<bool enforcedDep>
void NO_INLINE MlpSetExecuteWorkloadThread(WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms, const int thread_num,
                                           const int start_range, const int end_range) {


    if (enforcedDep) {
        uint64_t lastAnswer = 0;
        rep(i, start_range, end_range - 1) {
            uint32_t x = workload->operations[i].type;
            x ^= (uint32_t) lastAnswer;
            WorkloadOperationType type = (WorkloadOperationType) x;
            uint64_t realKey = workload->operations[i].key ^ lastAnswer;
            uint64_t answer;
            switch (type) {
                case WorkloadOperationType::INSERT: {
                    answer = ms->Insert(realKey);
                    break;
                }
                case WorkloadOperationType::EXIST: {
                    answer = ms->Exist(realKey);
                    break;
                }
                case WorkloadOperationType::LOWER_BOUND: {
                    bool found;
                    answer = ms->LowerBound(realKey, found);
                    break;
                }
            }
            workload->results[i] = answer;
            lastAnswer = answer;
        }
    } else {
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
            workload->results[i] = answer;
        }
    }

    printf("Thread %d : MlpSet workload completed.\n", thread_num);
}

void processChunk(const int thread_num, const int chunk_size, WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms) {
    const int start_range = thread_num * chunk_size;
    const int end_range = (thread_num + 1) * chunk_size;
    MlpSetExecuteWorkloadThread<false>(workload, ms, thread_num, start_range, end_range);
}

void InsertThread(const int thread_num, const int chunk_size, WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms) {
    const int start_range = thread_num * chunk_size;
    const int end_range = (thread_num + 1) * chunk_size;

    for (int i = start_range; i < end_range; i++) {
        ms->Insert(workload->initialValues[i]);
    }
}

TEST(Insertion, Single_Insertion) {
    WorkloadUInt64 workload = WorkloadA::GenWorkload1();
    Auto(workload.FreeMemory());
    printf("Executing workload..\n");

    MlpSetExecuteWorkload<false>(workload);

    printf("Validating results..\n");
    uint64_t sum = 0;
    MlpSetExecuteWorkload<false>(workload);
    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

TEST(Insertion, Single_LowerBound) {
    WorkloadUInt64 workload = WorkloadA::GenWorkLoadEmailLB();

    Auto(workload.FreeMemory());
    printf("Executing workload..\n");
    MlpSetExecuteWorkload<false>(workload);
    printf("Validating results..\n");

    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }

}

TEST(Insertion, Email) {
    printf("Generating workload WorkloadA 1 element..\n");
    WorkloadUInt64 workload = WorkloadA::GenWorkLoadEmail();
    Auto(workload.FreeMemory());
    printf("Executing workload..\n");

    MlpSetExecuteWorkload<false>(workload);
    uint64_t sum = 0;

    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

TEST(MlpSetUInt64, WorkloadA_16M_parllel_exist) {
    std::vector<std::thread> threads;
    const int num_of_threads = THREADS_NUM;

    printf("Generating workload WorkloadA 16M NO-ENFORCE dep..\n");
    WorkloadUInt64 workload = WorkloadA::GenWorkload16MExists();
    Auto(workload.FreeMemory());
    const int chunk_size = workload.numOperations / num_of_threads;

    printf("Executing workload..\n");
    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.Insert(workload.initialValues[i]);
        }
    }


    printf("MlpSet running ops..\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }


        for (auto &thread: threads) {
            thread.join();
        }
    }


    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        //assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }

}

TEST(MlpSetUInt64, WorkloadA_16M_parallel_Insert) {
    std::vector<std::thread> threads1;
    std::vector<std::thread> threads2;

    const int num_of_threads = THREADS_NUM;

    WorkloadUInt64 workload = WorkloadA::GenWorkload16MExists();
    Auto(workload.FreeMemory());
    const int initial_values_chunk_size = workload.numInitialValues / num_of_threads;
    const int ops_chunk_size = workload.numOperations / num_of_threads;
    printf("Executing workload..\n");

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads1.emplace_back(InsertThread, thread_num, initial_values_chunk_size, &workload, &ms);
        }


        for (auto &thread: threads1) {
            thread.join();
        }


    }

    printf("MlpSet running ops..\n");
    {
        AutoTimer timer;

        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads2.emplace_back(processChunk, thread_num, ops_chunk_size, &workload, &ms);
        }

        for (auto &thread: threads2) {
            thread.join();
        }
    }


    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {

        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

    TEST(MlpSetUInt64, WorkloadA_16M_parllel_LowerBound) {
        std::vector<std::thread> threads;
        const int num_of_threads = THREADS_NUM;

        WorkloadUInt64 workload = WorkloadA::GenWorkload16MLowerBound();
    Auto(workload.FreeMemory());
    const int chunk_size = workload.numOperations / num_of_threads;

    printf("Executing workload..\n");
    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.Insert(workload.initialValues[i]);
        }
    }


    printf("MlpSet running ops..\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }

    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

TEST(MlpSetUInt64, WorkloadA_16M_parllel_LowerBoundInsert) {
    std::vector<std::thread> threads;
    const int num_of_threads = THREADS_NUM;
    WorkloadUInt64 workload = WorkloadA::GenWorkload16MInsertLowerBoundParllel();
    Auto(workload.FreeMemory());

    const int chunk_size = workload.numOperations / num_of_threads;

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.Insert(workload.initialValues[i]);
        }
    }

    printf("Running `Insert & Exists` Test flow\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }


    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        if (workload.operations[i].type != WorkloadOperationType::INSERT) {
            bool found;
            assert(ms.LowerBound(workload.operations[i].key, found) ==
                   *workload.S.lower_bound(workload.operations[i].key));
        } else {
            bool found;
            uint64_t key_res = ms.LowerBound(workload.operations[i].key, found);
            assert(found);
            assert(key_res == workload.operations[i].key);
        }

    }
}

TEST(MlpSetUInt64, WorkloadA_16m_parllel_ExistsInsert) {
    std::vector<std::thread> threads;
    const int num_of_threads = THREADS_NUM;
    WorkloadUInt64 workload = WorkloadA::GenWorkload16MInsertExistsParallel();
    Auto(workload.FreeMemory());

    const int chunk_size = workload.numOperations / num_of_threads;

    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.Insert(workload.initialValues[i]);
        }
    }

    printf("Running `Insert & Exists` Test flow\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, chunk_size, &workload, &ms);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }


    printf("Validating results..\n");
    rep(i, 0, workload.numOperations - 1) {
        if (workload.operations[i].type != WorkloadOperationType::INSERT)
            continue;
        assert(ms.Exist(workload.operations[i].key));
    }
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Attach the custom test listener
    ::testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new TestNamePrinter);

    return RUN_ALL_TESTS();
}
