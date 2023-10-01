//
// Created by hod on 8/15/23.
//
#include "gtest/gtest.h"
#include "MlpSetUInt64.h"
#include "Workloads/WorkloadA.h"
#include "Workloads/WorkloadInterface.h"
#include "LightRangeGenerator.h"
#include <thread>

#define THREADS_NUM 1

void NO_INLINE MlpSetExecuteWorkload(WorkloadUInt64 &workload) {
    MlpSetUInt64::MlpSet ms;
    ms.Init(workload.numInitialValues + 1000);

    printf("MlpSet populating initial values..\n");
    {
        rep(i, 0, workload.numInitialValues - 1) {
            ms.InsertRange(workload.initialValues[i].first, workload.initialValues[i].second);
        }
    }
    printf("MlpSet executing workload..\n");
    {
        {
            rep(i, 0, workload.numOperations - 1) {
                uint64_t answer;
                switch (workload.operations[i].type) {
                    case WorkloadOperationType::INSERT_RANGE: {
                        ms.InsertRange(workload.operations[i].node.range.first,
                                       workload.operations[i].node.range.second);
                        break;
                    }
                    case WorkloadOperationType::EXIST: {
                        answer = ms.Exist(workload.operations[i].node.key);
                        break;
                    }
                    case WorkloadOperationType::LOWER_BOUND: {
                        bool found;
                        answer = ms.LowerBound(workload.operations[i].node.key, found);
                        break;
                    }
                }
                workload.results[i] = answer;
            }
        }
    }

    printf("MlpSet workload completed.\n");
}

void NO_INLINE MlpSetExecuteWorkloadThread(WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms, const int thread_num, const int num_of_threads) {
    {
        rep(i, 0, workload->numOperations - 1) {
            if (i % num_of_threads != thread_num)
                continue;

            uint64_t answer;
            switch (workload->operations[i].type) {
                case WorkloadOperationType::INSERT_RANGE: {
                    ms->InsertRange(workload->operations[i].node.range.first,
                                    workload->operations[i].node.range.second);
                    break;
                }
                case WorkloadOperationType::EXIST: {
                    answer = ms->Exist(workload->operations[i].node.key);
                    break;
                }
                case WorkloadOperationType::LOWER_BOUND: {
                    bool found;
                    answer = ms->LowerBound(workload->operations[i].node.key, found);
                    break;
                }
            }
            workload->results[i] = answer;
        }
    }
    printf("Thread %d : MlpSet workload completed.\n", thread_num);
}

void processChunk(const int thread_num, WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms, const int num_threads) {
    MlpSetExecuteWorkloadThread(workload, ms, thread_num, num_threads);
}

void InsertThread(const int thread_num, WorkloadUInt64 *workload, MlpSetUInt64::MlpSet *ms, const int num_threads) {
    for (int i = 0; i < workload->numInitialValues; i++) {
        if (i % num_threads != thread_num)
            continue;
        ms->InsertRange(workload->initialValues[i].first, workload->initialValues[i].second);
    }
}


TEST(Insertion, Single_Insertion) {
    WorkloadUInt64 workload = WorkloadA::GenWorkload1();
    Auto(workload.FreeMemory());
    printf("Executing workload..\n");

    MlpSetExecuteWorkload(workload);

    printf("Validating results..\n");
    uint64_t sum = 0;
    MlpSetExecuteWorkload(workload);
    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

TEST(Insertion, Single_LowerBound) {
    WorkloadUInt64 workload = WorkloadA::GenWorkLoadEmailLB();

    Auto(workload.FreeMemory());
    printf("Executing workload..\n");
    MlpSetExecuteWorkload(workload);
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

    MlpSetExecuteWorkload(workload);
    uint64_t sum = 0;

    rep(i, 0, workload.numOperations - 1) {
        assert(workload.results[i] == workload.expectedResults[i]);
        sum += workload.results[i];
    }
}

TEST(MlpSetUInt64, WorkloadA_16M_parllel_exist) {
    std::vector<std::thread> threads;
    const int num_of_threads = THREADS_NUM;

    printf("Generating workload WorkloadA 16M..\n");
    WorkloadUInt64 workload = WorkloadA::GenWorkload16MExists();
    Auto(workload.FreeMemory());

    printf("\n\nExecuting workload..\n");
    MlpSetUInt64::MlpSet ms;
    ms.Init(8*(workload.numInitialValues + workload.numOperations));

    printf("\n\nMlpSet populating initial values..\n");
    {
        rep(i, 0, workload.numInitialValues - 1) {
            ms.InsertRange(workload.initialValues[i].first, workload.initialValues[i].second);
        }
    }


    printf("MlpSet running ops..\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, &workload, &ms, num_of_threads);
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

TEST(MlpSetUInt64, WorkloadA_16M_parallel_Insert) {
    std::vector<std::thread> threads1;
    std::vector<std::thread> threads2;

    const int num_of_threads = THREADS_NUM;

    WorkloadUInt64 workload = WorkloadA::GenWorkload16MExists();
    Auto(workload.FreeMemory());
    printf("Executing workload..\n");

    MlpSetUInt64::MlpSet ms;
    ms.Init(8*(workload.numInitialValues + workload.numOperations));
    printf("\n\nMlpSet populating initial values..\n");
    {
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads1.emplace_back(InsertThread, thread_num, &workload, &ms, num_of_threads);
        }


        for (auto &thread: threads1) {
            thread.join();
        }


    }

    printf("\n\nMlpSet running ops..\n");
    {
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads2.emplace_back(processChunk, thread_num, &workload, &ms, num_of_threads);
        }

        for (auto &thread: threads2) {
            thread.join();
        }
    }


    printf("\n\nValidating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        ASSERT_EQ(workload.results[i], workload.expectedResults[i]) << "i: " << i << "\nKey: 0x" << std::hex << workload.operations[i].node.key;
        sum += workload.results[i];
    }
}

TEST(MlpSetUInt64, WorkloadA_16M_parllel_LowerBound) {
    std::vector<std::thread> threads;
    const int num_of_threads = THREADS_NUM;

    WorkloadUInt64 workload = WorkloadA::GenWorkload16MLowerBound();
    Auto(workload.FreeMemory());

    printf("Executing workload..\n");
    MlpSetUInt64::MlpSet ms;
    ms.Init(8*(workload.numInitialValues + workload.numOperations));

    printf("MlpSet populating initial values..\n");
    {
        rep(i, 0, workload.numInitialValues - 1) {
            ms.InsertRange(workload.initialValues[i].first, workload.initialValues[i].second);
        }
    }


    printf("MlpSet running ops..\n");
    {
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, &workload, &ms, num_of_threads);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }

    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        ASSERT_EQ(workload.results[i], workload.expectedResults[i]) <<"i :" << i << "\nKey value: 0x" << std::hex << workload.operations[i].node.key << std::endl;
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
    ms.Init(8*(workload.numInitialValues + workload.numOperations));

    printf("MlpSet populating initial values..\n");
    {
        rep(i, 0, workload.numInitialValues - 1) {
            ms.InsertRange(workload.initialValues[i].first, workload.initialValues[i].second);
        }
    }

    printf("Running `Insert & Exists` Test flow\n");
    {
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, &workload, &ms, num_of_threads);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }


    printf("Validating results..\n");
    uint64_t sum = 0;
    rep(i, 0, workload.numOperations - 1) {
        if (workload.operations[i].type != WorkloadOperationType::INSERT_RANGE) {
            bool found;
            assert(ms.LowerBound(workload.operations[i].node.key, found) ==
                   workload.S.lower_bound(workload.operations[i].node.key));
        } else {
            bool found;
            uint64_t key_res = ms.LowerBound(workload.operations[i].node.range.first, found);
            assert(found);
            assert(key_res == workload.operations[i].node.range.first);
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
    ms.Init(8*(workload.numInitialValues + workload.numOperations));

    printf("\n\nMlpSet populating initial values..\n");
    {
        AutoTimer timer;
        rep(i, 0, workload.numInitialValues - 1) {
            ms.InsertRange(workload.initialValues[i].first, workload.initialValues[i].second);
        }
    }

    printf("\n\nRunning `Insert & Exists` Test flow\n");
    {
        AutoTimer timer;
        for (int thread_num = 0; thread_num < num_of_threads; thread_num++) {
            threads.emplace_back(processChunk, thread_num, &workload, &ms, num_of_threads);
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }


    printf("\n\nValidating results..\n");
    rep(i, 0, workload.numOperations - 1) {
        if (workload.operations[i].type != WorkloadOperationType::INSERT_RANGE)
            continue;
        assert(ms.Exist(workload.operations[i].node.range.first));
    }
}



TEST(EdgeCase, Case1) {
    uint64_t l = 0x2da3016800;
    uint64_t h = 0x2da40167ff;
    MlpSetUInt64::MlpSet ms;
    ms.Init(1000);

    ms.InsertRange(l,h);
    //ms.PrintMlpSet();
    ms.Exist(0x2da4000100);
}


TEST(LightRanges, SmallGaps_NoShuffle_NoTopLevels) {
    MlpSetUInt64::MlpSet ms;

    LightRandomGenerator LRG(NO_LARGE_RANGES, MINIMAL_GAPS);
    
    std::vector<pair64*>* ranges = LRG.DumpToVector(10, false);

    for (const auto &elem: *ranges)
	    printf("%lx, %lx\n", elem->first, elem->second);

    printf("Test Size: %ld\n", ranges->size());

    ms.Init(8 * ranges->size() + 1000);

    printf("MlpSet populating initial values..\n");
    {
        AutoTimer timer;
        for (const auto &pair: *ranges) {
            ms.InsertRange(pair->first, pair->second);
        }
    }

    printf("Validating results..\n");
    {
        AutoTimer timer;
        for (const auto &pair: *ranges) {
            ASSERT_EQ(ms.Exist(pair->first), true);
            ASSERT_EQ(ms.Exist(pair->second), true);

            if (!ms.Exist(pair->first + (rand() % (pair->second - pair->first)))) {
                printf("(0x%lx, 0x%lx)\n",pair->first, pair->second);
                ASSERT_EQ(true,false);
            }
        }
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
