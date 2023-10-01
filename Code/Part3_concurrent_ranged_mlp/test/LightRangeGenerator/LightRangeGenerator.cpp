//
// Created by eaad on 9/26/23.
//

#include "LightRangeGenerator.h"

#include <fstream>
#include <iostream>

LightRandomGenerator::LightRandomGenerator(ranges_sizes_behaviour_t rsb, gap_between_ranges_behaviour_t gbr) :
        rsb_rd(), rsb_gen(rsb_rd()), rsb_distribution((const double *) (rsb_probs[rsb]), rsb_probs[rsb] + 8),
        gbr_rd(), gbr_gen(gbr_rd()), gbr_distribution((const double *) (gbr_probs[gbr]), gbr_probs[gbr] + 8) {
    this->next_low_bound = 0;
    this->policy = DO_NOTHING;
    this->allowed_retries = 100;
    this->overlap_flag = false;
    this->rsb = rsb;
    this->gbr = gbr;
}

uint64_t LightRandomGenerator::SampleRsbValue() {
    return mul_256[rsb_distribution(this->rsb_gen)];
}

uint64_t LightRandomGenerator::SampleGbrValue() {
    return mul_256[gbr_distribution(this->gbr_gen)];
}

void LightRandomGenerator::SetOverlapPolicy(overlap_policy_t policy) {
    this->policy = policy;
}

bool LightRandomGenerator::EnforcePolicy(uint32_t *retry_cnt, uint64_t low, uint64_t high, bool *ret_nullptr) {
    *ret_nullptr = false;
    bool retry = false;
    if (high < low) {
        this->overlap_flag = true;
        switch (policy) {
            case DO_NOTHING:
                break;
            case ASSERT:
                assert(high >= low);
                break;
            case RETRY_N_TIMES_THEN_NULLPTR:
            case RETRY_N_TIMES_THEN_ASSERT:
                if (*retry_cnt >= this->allowed_retries) {
                    *ret_nullptr = true;
                    if (policy == RETRY_N_TIMES_THEN_ASSERT)
                        assert(*retry_cnt < this->allowed_retries);
                } else retry = true;
                break;
            default:
                assert(false);
                break;
        }
    }
    return retry;
}

pair64 *LightRandomGenerator::Next() {

    pair64 *result = new pair64();
    uint32_t retry = 0;
    result->first = this->next_low_bound;
    bool ret_nullptr;
    bool retry_flag;

    while (true) {
        assert(retry < this->hardcoded_retries);
        retry++;

        uint64_t range_size = this->SampleRsbValue();
        result->second = result->first + range_size;
        retry_flag = EnforcePolicy(&retry, result->first, result->second, &ret_nullptr);
        if (ret_nullptr) {
            free(result);
            return nullptr;
        }
        if (retry_flag)
            continue;

        uint64_t gap = this->SampleGbrValue();
        if (range_size == 0 && gap == 0)
            gap = 0x100;

        uint64_t proposed_next_lower_bound = result->second + gap;

        retry_flag = EnforcePolicy(&retry, result->second, proposed_next_lower_bound, &ret_nullptr);
        if (ret_nullptr) {
            free(result);
            return nullptr;
        }
        if (retry_flag)
            continue;

        this->next_low_bound = result->second + gap;
        if (range_size)
            result->second--;
        return result;
    }
}

void LightRandomGenerator::SetRsbSeed(uint32_t r) {
    rsb_gen.seed(r);
}

void LightRandomGenerator::SetGbrSeed(uint32_t r) {
    gbr_gen.seed(r);
}

std::vector<pair64 *> *LightRandomGenerator::DumpToVector(uint32_t max_elements, bool shuffle) {
    std::vector<pair64 *> *result = new std::vector<pair64 *>();
    this->SetOverlapPolicy(RETRY_N_TIMES_THEN_NULLPTR);
    uint32_t cnt = 0;
    while (cnt < max_elements) {
        pair64 *curr = this->Next();
        if (curr == nullptr)
            break;
        result->push_back(curr);
        cnt++;
    }
    if (shuffle) {
	    std:: random_device rd;
	    std::mt19937 rng(rd());
	    std::shuffle(result->begin(), result->end(), rng);
    }

    return result;
}



