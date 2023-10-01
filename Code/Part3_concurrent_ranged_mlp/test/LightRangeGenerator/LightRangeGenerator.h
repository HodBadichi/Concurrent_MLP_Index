//
// Created by eaad on 9/26/23.
//

#ifndef CONCURRENT_MLPDS_LIGHTRANGEGENERATOR_H
#define CONCURRENT_MLPDS_LIGHTRANGEGENERATOR_H
#include <vector>
#include <cstdint>
#include <random>
#include <cassert>
#include <algorithm>

typedef std::pair<uint64_t, uint64_t> pair64;

typedef enum ranges_sizes_behaviour {
    ONLY_SINGLETONS,
    UNIFORM_RANGES,
    NO_SINGLETONS_DECREASING,
    LARGE_RANGES,
    NO_LARGE_RANGES,
    ALMOST_UNIFORM_RANGES,
    LOW_UNIFORM_RANGES,
    RANGES_SIZES_BEHAVIOUR_LAST_ENUM,

} ranges_sizes_behaviour_t;

typedef enum gap_between_ranges_behaviour {
    MINIMAL_GAPS,
    LARGE_ONLY,
    UNIFORM_GAPS,
    NO_LARGE_GAPS,
    GAP_BETWEEN_RANGES_BEHAVIOUR_LAST_ENUM
} gap_between_ranges_behaviour_t;

typedef enum overlap_policy {
    DO_NOTHING,
    ASSERT,
    RETRY_N_TIMES_THEN_NULLPTR,
    RETRY_N_TIMES_THEN_ASSERT,
    OVERLAP_POLICY_LAST_ENUM
} overlap_policy_t;

class LightRandomGenerator {
public:
    uint64_t next_low_bound;
    ranges_sizes_behaviour_t rsb;
    gap_between_ranges_behaviour_t gbr;
    overlap_policy_t policy;
    uint32_t allowed_retries;
    uint32_t hardcoded_retries = 2000;
    bool overlap_flag;

    LightRandomGenerator(ranges_sizes_behaviour_t rsb, gap_between_ranges_behaviour_t gbr);
    LightRandomGenerator() : LightRandomGenerator(UNIFORM_RANGES, UNIFORM_GAPS) {};
    void SetOverlapPolicy(overlap_policy_t policy);
    bool EnforcePolicy(uint32_t *retry, uint64_t low, uint64_t high, bool* ret_nullptr);
    void SetRsbSeed(uint32_t r);
    void SetGbrSeed(uint32_t r);

    std::vector<pair64*> * DumpToVector(uint32_t max_elements, bool shuffle = true);

    pair64 * Next();

    uint64_t SampleGbrValue();
    uint64_t SampleRsbValue();


//private:
    uint64_t mul_256[8] = {0, 0x100, 0x10000, 0x1000000, 0x100000000,
                           0x10000000000, 0x1000000000000, 0x100000000000000};

    const double rsb_probs[RANGES_SIZES_BEHAVIOUR_LAST_ENUM][8] = {
            [ONLY_SINGLETONS] =
                    {1, 0, 0, 0, 0, 0, 0, 0},
            [UNIFORM_RANGES] =
                    {.25, .25, .25, .25, .25, .25, .25 ,.25},
            [NO_SINGLETONS_DECREASING] =
                    {0, .35, .25, .15, .10, .5, .5 ,.5},
            [LARGE_RANGES] =
                    {0, 0, 0, 0, 0, 0, .1, .9},
            [NO_LARGE_RANGES] =
                    {0, .25 ,.25 ,.25 ,.25 ,0 ,0 ,0},
            [ALMOST_UNIFORM_RANGES] =
                    {.1, .225, .325, .2, .15, .0, 0 ,0},
            [LOW_UNIFORM_RANGES] =
                    {.15, .25, .35, .25, .0, 0, 0 ,0},
    };

    const double gbr_probs[GAP_BETWEEN_RANGES_BEHAVIOUR_LAST_ENUM][8] = {
            [MINIMAL_GAPS] =
                    {0, 1, 0, 0, 0, 0, 0, 0},
            [LARGE_ONLY] =
                    {0, 0, 0, 0, 0, 0, 0, 1},
            [UNIFORM_GAPS] =
                    {0, 14.285, 14.285, 14.285, 14.285, 14.285, 14.285, 14.285},
            [NO_LARGE_GAPS] =
                    {0.25, .25, .25, .25, 0, 0, 0, 0},
    };

    std::random_device rsb_rd;
    std::mt19937 rsb_gen;
    std::discrete_distribution<uint8_t> rsb_distribution;

    std::random_device gbr_rd;
    std::mt19937 gbr_gen;
    std::discrete_distribution<uint8_t> gbr_distribution;

};


#endif //CONCURRENT_MLPDS_LIGHTRANGEGENERATOR_H
