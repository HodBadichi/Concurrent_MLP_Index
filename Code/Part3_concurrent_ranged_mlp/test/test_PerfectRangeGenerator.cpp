//
// Created by hodbadihi on 9/18/23.
//
#include <iostream>
#include "PerfectRangeGenerator/PerfectRangeGenerator.h"
#include <vector>
#include <cassert>
#include <algorithm>

bool ValidateIntervals(std::vector<std::pair<unsigned long, unsigned long>> &intervals) {
    // Sort the intervals based on their start values
    std::sort(intervals.begin(), intervals.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    unsigned long previousEnd = 0;

    for (const auto &interval: intervals) {
        if ((((interval.second - interval.first) + 1) % 256) && (interval.second != interval.first)) {
            std::cerr << "One of the intervals is not divisible by 256 or of size 1 (" << interval.first << ","
                      << interval.second
                      << ")" << std::endl;
            std::cerr << "Result : " << (interval.second - interval.first) + 1 << std::endl;
            return false; // One of the intervals is not divisible by 256
        }

        if (interval.first < previousEnd) {
            std::cerr << "Intervals overlap " << std::endl;

            return false; // Intervals overlap
        }

        previousEnd = interval.second;
    }

    return true; // All intervals are valid
}

int main() {
    std::vector<int> ranges({1, 256, 512});
    std::vector<double> distrbuitions({0.5, 0.25, 0.25});
    PerfectRangeGenerator x = PerfectRangeGenerator(10, ranges, distrbuitions);
    x.Initialize();
    assert(ValidateIntervals(x.ranges_shuffled_pool));
}