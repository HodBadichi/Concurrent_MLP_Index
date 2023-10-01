//
// Created by hodbadihi on 9/18/23.
//
#include "PerfectRangeGenerator.h"
#include <random>
#include <iostream>
#include <algorithm>
#include <cassert>

PerfectRangeGenerator::PerfectRangeGenerator(int _samples, std::vector<unsigned long> &_range_sizes,
                                             std::vector<double> &_distrbuitions) {
    for (auto &it: _range_sizes) {
        if (it % 256 && it != 1) {
            std::cerr << " PerfectRangeGenerator ranges must be divisible by 256 or size 1" << std::endl;
            exit(1);
        }
        this->range_sizes.push_back(it);
    }

    double sum = 0;
    for (auto &it: _distrbuitions)
        sum += it;

    const double epsilon = 1e-9; // Define a small tolerance

    if (std::abs(sum - 1.0) > epsilon) {
        std::cerr << "distributions vector must sum up to 1. result: " << sum << std::endl;
        exit(1);
    }

    this->samples = _samples;

    this->initialized = false;

    this->range_distrbuitions = _distrbuitions;
    this->index = 0;
}

std::pair<unsigned long, unsigned long> PerfectRangeGenerator::FindRandomGap(unsigned long gap_size) {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max() - gap_size - 1);
    std::pair<unsigned long, unsigned long> gap_pair;

    int cnt = 1000;
    while (cnt > 0) {
        cnt--;
        unsigned long random_number = dist(rng);
        random_number &= 0xFFFFFFFFFFFFFF00ULL;

        if (this->range_validator.count(random_number) > 0) {
            continue;
        }

        unsigned long lower_bound_result = this->range_validator.lower_bound(random_number);
        std::pair<unsigned long, unsigned long> lowerbound_pair;

        if (lower_bound_result == 0xffffffffffffffffULL) {
            lowerbound_pair.first = std::numeric_limits<uint32_t>::max();
            lowerbound_pair.second = std::numeric_limits<uint32_t>::max();
        }

        if (lower_bound_result < (gap_size + random_number - 1)) {
            continue;
        } else {
            gap_pair.first = random_number;
            gap_pair.second = random_number + gap_size - 1;
            break;
        }

    }
    if (cnt == 0) {
        std::cerr << "Error: too many retries in `PerfectRangeGenerator`" << std::endl;
        exit(1);
    }

    assert(this->range_validator.count(gap_pair.first) == 0);
    assert(this->range_validator.count(gap_pair.second) == 0);
    assert(gap_pair.first <= gap_pair.second);
    return gap_pair;


}

void PerfectRangeGenerator::AddRange(Range &_range) {
    this->range_validator.InsertRange(_range.first, _range.second);
}

void PerfectRangeGenerator::Initialize() {

    for (int i = 0; i < this->samples; i++) {
        std::mt19937 rng(42);
        std::discrete_distribution<int> dist(this->range_distrbuitions.begin(), this->range_distrbuitions.end());

        int pick_random_size_index = dist(rng);

        unsigned long gap_size = this->range_sizes[pick_random_size_index];

        std::pair<unsigned long, unsigned long> new_range = this->FindRandomGap(gap_size);
        this->AddRange(new_range);

    }

    // Copy elements from set to vector

    for (std::pair<unsigned long, unsigned long> elem: this->range_validator.S) {
        this->ranges_shuffled_pool.push_back(elem);
    }

    // Shuffle the vector
    //    std::random_shuffle(ranges_shuffled_pool.begin(), ranges_shuffled_pool.end());


    this->initialized = true;
}


std::pair<uint64_t, uint64_t> PerfectRangeGenerator::GetNextRange() {

    if (!this->initialized) {
        std::cerr << "Exception: PerfectRangeGenerator can not call 'GetNextRange' before initialization" << std::endl;
        exit(1);
    }
    return this->ranges_shuffled_pool[this->index++];
}

