//
// Created by hod on 8/15/23.
//
#include "gtest/gtest.h"
#include "MlpSetUInt64.h"
#include "../../Part1_concurrent_mlp_index/test/Workloads/WorkloadA.h"
#include "../../Part1_concurrent_mlp_index/test/Workloads/WorkloadInterface.h"
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

TEST(InsertRange, Basic_Case) {
    const int N = 1000;
    vector<int> positive_values = {0x0, 0x1, 0xefe, 0xdff, 0x100, 0xeff};
    vector<int> negative_values = {0xf00, 0xffff, 0xfff};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0, 0x0eff);

    for (auto &it: positive_values)
        assert(ms.Exist(it));

    for (auto &it: negative_values)
        assert(!(ms.Exist(it)));

}

TEST(InsertRange, Advanced_Case) {
    const int N = 1000;
    vector<int> positive_values1 = {0x0, 0x1, 0xefe, 0xdff, 0x100, 0xeff};
    vector<int> negative_values1 = {0x110000, 0x09ffff};

    vector<int> positive_values2 = {0xa0000, 0x0c1234};
    vector<int> negative_values2 = {0x110000, 0x09ffff};
    MlpSetUInt64::MlpSet ms;

    ms.Init(N + 1000);
    ms.InsertRange(0x0a0000, 0x10ffff);
    ms.InsertRange(0, 0x0eff);

    ms.Insert(0xf00);
    ms.Insert(0x08ffff);

    for (auto &it: positive_values1)
        assert(ms.Exist(it));

    for (auto &it: negative_values1)
        assert(!(ms.Exist(it)));

    for (auto &it: positive_values2)
        assert(ms.Exist(it));

    for (auto &it: negative_values2)
        assert(!(ms.Exist(it)));

    assert(ms.Exist(0xf00));
    assert(ms.Exist(0x08ffff));

}

TEST(InsertRange, Extended_Range_Top) {
    const int N = 1000;
    vector<uint64_t> positive_values = {0x0, 0x0cffffffffffffff, 0x0dffffffffffffff, 0x0e00000000000000,
                                        0x0ebfffffffffffff, 0x0edfffffffffffff};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0, 0x0fffffffffffffff);
    for (auto &it: positive_values)
        assert(ms.Exist(it));
}

TEST(InsertRange, Extended_Range) {
    const int N = 1000;
    vector<uint64_t> positive_values = {0x0, 0x0cffffff, 0x0dffffff, 0x0e000000, 0x0ebfffff, 0x0edfffff};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0, 0x0edfffff);

    for (auto &it: positive_values)
        assert(ms.Exist(it));
}

TEST(InsertRange, Lower_Bound_One_Range) {
    const int N = 1000;
    vector<uint64_t> queries = {0x0, 0x100, 0x200,
                                0xaaaa, 0xbbbb, 0xcccc,
                                0xffff, 0x10000, 0x10001};
    vector<uint64_t> expected = {0x1024, 0x1024, 0x1024,
                                 0xaaaa, 0xbbbb, 0xcccc,
                                 0xffff, 0xffffffffffffffff, 0xffffffffffffffff};

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x1024, 0xffff);
    bool found;
    uint64_t k;
    for (uint16_t i = 0; i < queries.size(); i++) {
        k = ms.LowerBound(queries[i], found);
        if (k != expected[i]) {
            printf("query: 0x%lx answer: 0x%lx expected: 0x%lx\n", queries[i], k, expected[i]);
            assert(false);
        }
    }
}

TEST(InsertRange, Lower_Bound_One_Range_One_Very_Far_Insert) {
    const int N = 1000;
    vector<uint64_t> queries = {0x0, 0x100, 0x200,
                                0xaaaa, 0xbbbb, 0xcccc,
                                0xffff, 0x10000, 0x10001,
    };
    vector<uint64_t> expected = {0x1fff, 0x1fff, 0x1fff,
                                 0xaaaa, 0xbbbb, 0xcccc,
                                 0xffff, 0xffffffffff, 0xffffffffff,
    };

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x1fff, 0xffff);
    ms.Insert(0xffffffffff);

    bool found;
    uint64_t k;
    for (uint16_t i = 0; i < queries.size(); i++) {
        k = ms.LowerBound(queries[i], found);
        if (k != expected[i]) {
            printf("query: 0x%lx answer: 0x%lx expected: 0x%lx\n", queries[i], k, expected[i]);
            assert(false);
        }
    }
}

TEST(InsertRange, Lower_Bound_Multi_Perfect_Ranges) {
    const int N = 1000;
    vector<uint64_t> queries = /*{0x0, 0x5, 0x6,*/{
            0x102, 0x105, 0x106,
            0x1001, 0x1005, 0x1006};
    vector<uint64_t> expected = /*{0x0, 0x5, 0x6,*/{
            0x1000, 0x1000, 0x1000,
            0x1001, 0x1005, 0x1006};

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x0, 0xff);

    ms.InsertRange(0x1000, 0xffff);

    ms.InsertRange(0x100000, 0x1fffff);
    bool found;
    uint64_t k;
    for (uint16_t i = 0; i < queries.size(); i++) {
        k = ms.LowerBound(queries[i], found);
        if (k != expected[i]) {
            printf("query: 0x%lx answer: 0x%lx expected: 0x%lx\n", queries[i], k, expected[i]);
            ASSERT_EQ(k, expected[i]);
        }
    }
}

TEST(InsertRange, Lower_Bound_Adv) {
    const int N = 1000;
    vector<uint64_t> queries = {0x0, 0x5, 0x6,
                                0x1001, 0x1005, 0x1006};

    vector<uint64_t> expected = {0x0, 0x5, 0x6,
                                 0x1001, 0x1005, 0x1006};

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x0, 0xff);
    ms.InsertRange(0x1ff, 0xffff);
    ms.InsertRange(0x1fffff, 0xffffff);

    bool found;
    uint64_t k;
    for (uint16_t i = 0; i < queries.size(); i++) {
        k = ms.LowerBound(queries[i], found);
        if (k != expected[i]) {
            printf("query: 0x%lx answer: 0x%lx expected: 0x%lx\n", queries[i], k, expected[i]);
            ASSERT_EQ(true, false);
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
