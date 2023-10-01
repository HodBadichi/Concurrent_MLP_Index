//
// Created by hod on 8/15/23.
//
#include "gtest/gtest.h"
#include "MlpSetUInt64.h"


TEST(InsertRange, Basic_Case) {
    const int N = 1000;
    vector<int> positive_values = {0x0, 0x1, 0xefe, 0xdff, 0x100, 0xeff};
    vector<int> negative_values = {0xf00, 0xffff, 0xfff};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0, 0x0eff);
    for (auto &it: positive_values) {
        ASSERT_EQ(ms.Exist(it), true);
    }


    for (auto &it: negative_values)
        ASSERT_EQ(!(ms.Exist(it)), true);

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

    ms.InsertRange(0xf00, 0xf00);
    ms.InsertRange(0x08ffff, 0x08ffff);


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
        ASSERT_EQ(ms.Exist(it), true) << "key value: 0x" << std::hex << it << std::endl;
}

TEST(InsertRange, Extended_Range) {
    const int N = 1000;
    vector<uint64_t> positive_values = {0x0, 0x0cffffff, 0x0dffffff, 0x0e000000, 0x0ebfffff, 0x0edfffff};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0, 0x0edfffff);
    for (auto &it: positive_values)
        ASSERT_EQ(ms.Exist(it), true) << "key value: 0x" << std::hex << it << std::endl;
}

TEST(InsertRange, Extended_Range_both_ways) {
    const int N = 1000;
    vector<uint64_t> positive_values = {0x0edcba00, 0x0fffffff, 0x0f000000};
    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x0edcba00, 0x0fffffff);
    for (auto &it: positive_values)
        ASSERT_EQ(ms.Exist(it), true) << "key value: 0x" << std::hex << it << std::endl;
}

TEST(InsertRange, Lower_Bound_One_Range) {
    const int N = 1000;
    vector<uint64_t> queries = {0x0, 0x100, 0x200,
                                0xaaaa, 0xbbbb, 0xcccc,
                                0xffff, 0x10000, 0x10001};
    vector<uint64_t> expected = {0x1000, 0x1000, 0x1000,
                                 0xaaaa, 0xbbbb, 0xcccc,
                                 0xffff, 0xffffffffffffffff, 0xffffffffffffffff};

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x1000, 0xffff);
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

TEST(InsertRange, Lower_Bound_One_Range_One_Very_Far_Insert) {
    const int N = 1000;
    vector<uint64_t> queries = {0x0, 0x100, 0x200,
                                0xaaaaaa, 0xbbbbbb, 0xcccccc,
                                0xffffff, 0x10000000, 0x10000001,
    };
    vector<uint64_t> expected = {0x1f0000, 0x1f0000, 0x1f0000,
                                 0xaaaaaa, 0xbbbbbb, 0xcccccc,
                                 0xffffff, 0xffffffffff, 0xffffffffff,
    };

    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);
    ms.InsertRange(0x1f0000, 0xffffff);
    ms.InsertRange(0xffffffffff, 0xffffffffff);

    bool found;
    uint64_t k;
    for (uint16_t i = 0; i < queries.size(); i++) {
        k = ms.LowerBound(queries[i], found);
        if (k != expected[i]) {
            printf("query: 0x%lx answer: 0x%lx expected: 0x%lx\n", queries[i], k, expected[i]);
            ASSERT_EQ(false, true);
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
    ms.InsertRange(0x100, 0xffff);
    ms.InsertRange(0x100000, 0xffffff);

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

TEST(InsertRange, Singelton) {
    const int N = 1000;


    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);

    ms.InsertRange(0x53, 0x53);
    ms.InsertRange(0x54, 0x54);
    ms.InsertRange(0x100, 0x100);

    ASSERT_EQ(ms.Exist(0x52), false);
    ASSERT_EQ(ms.LowerBound(0x52).Resolve(), 0x53);
    ASSERT_EQ(ms.Exist(0x53), true);
    ASSERT_EQ(ms.LowerBound(0x53).Resolve(), 0x53);
    ASSERT_EQ(ms.Exist(0x54), true);
    ASSERT_EQ(ms.LowerBound(0x54).Resolve(), 0x54);
    ASSERT_EQ(ms.LowerBound(0x55).Resolve(), 0x100);
    ASSERT_EQ(ms.Exist(0x100), true);
    ASSERT_EQ(ms.LowerBound(0x100).Resolve(), 0x100);

}

TEST(InsertRange, SingeltonAndRange) {
    const int N = 1000;


    MlpSetUInt64::MlpSet ms;
    ms.Init(N + 1000);

    ms.InsertRange(0xff, 0xff);
    ms.InsertRange(0x100, 0x1ff);

    ASSERT_EQ(ms.Exist(0xff), true);
    ASSERT_EQ(ms.LowerBound(0xff).Resolve(), 0xff);
    ASSERT_EQ(ms.Exist(0x100), true);
    ASSERT_EQ(ms.LowerBound(0x100).Resolve(), 0x100);
    ASSERT_EQ(ms.Exist(0x154), true);
    ASSERT_EQ(ms.LowerBound(0x154).Resolve(), 0x154);
    ASSERT_EQ(ms.Exist(0xfe), false);
    ASSERT_EQ(ms.LowerBound(0xfe).Resolve(), 0xff);

}

TEST(InsertRange, EdgeCase1) {
    MlpSetUInt64::MlpSet ms;
    ms.Init( 1000);

    ms.InsertRange(0xe7165100, 0xe71651ff);
    ms.InsertRange(0xe73c8600, 0xe73c86ff);
    ms.InsertRange(0xe73ca200, 0xe73ca2ff);
    ASSERT_EQ(ms.Exist(0xe73c8600), true);

}

TEST(InsertRange, EdgeCase2) {
    MlpSetUInt64::MlpSet ms;
    ms.Init( 1000);

    ms.InsertRange(0x9ad2460ef13afe00,0x9ad2460ef13b02ff);
    ASSERT_EQ(ms.Exist(0x9ad2460ef13b01fb), true);

}

TEST(InsertRange, FromTopToBottom) {
    MlpSetUInt64::MlpSet ms;
    ms.Init( 1000);

    ms.InsertRange(0x0000000000000000,0x00000100ffffffff);
    ASSERT_EQ(ms.Exist(0x0000000000000000), true);
    ASSERT_EQ(ms.Exist(0x0000000000100000), true);
    ASSERT_EQ(ms.Exist(0x000000ffffffffff), true);
    ASSERT_EQ(ms.Exist(0x0000010000000000), true);
    ASSERT_EQ(ms.Exist(0x0000010000010000), true);
    ASSERT_EQ(ms.Exist(0x00000100ffffffff), true);

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
