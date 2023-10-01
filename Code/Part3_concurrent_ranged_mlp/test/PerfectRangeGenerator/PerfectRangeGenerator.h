//
// Created by hodbadihi on 9/18/23.
//

#ifndef CONCURRENT_MLPDS_PERFECTRANGEGENERATOR_H
#define CONCURRENT_MLPDS_PERFECTRANGEGENERATOR_H


#include <utility>
#include "../RangeSet/RangeSet.h"
#include <vector>
#include <cstdint>


class PerfectRangeGenerator {
public:

    int samples;
    std::vector<unsigned long> range_sizes;
    std::vector<std::pair<unsigned long, unsigned long>> ranges_shuffled_pool;


    PerfectRangeGenerator(int _samples, std::vector<unsigned long> &_range_sizes, std::vector<double> &_distrbuitions);

    void Initialize();

    std::pair<uint64_t, uint64_t> GetNextRange();

private:
    RangeSet range_validator;

    bool initialized;

    std::vector<double> range_distrbuitions;

    void AddRange(Range &_range);

    int index;

    std::pair<unsigned long, unsigned long> FindRandomGap(unsigned long  gap_size);


};


#endif //CONCURRENT_MLPDS_PERFECTRANGEGENERATOR_H
