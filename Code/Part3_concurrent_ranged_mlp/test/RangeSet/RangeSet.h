//
// Created by hodbadihi on 9/22/23.
//

#ifndef CONCURRENT_MLPDS_RANGESET_H
#define CONCURRENT_MLPDS_RANGESET_H

#include <utility>
#include <set>

struct CustomComparator {
    bool operator()(const std::pair<unsigned long, unsigned long> &a,
                    const std::pair<unsigned long, unsigned long> &b) const {
        return a.first < b.first;
    }
};

typedef std::pair<unsigned long, unsigned long> Range;

class RangeSet {
public:
    std::pair<bool, bool> InsertRange(unsigned long start_range, unsigned long end_range);

    unsigned long count(unsigned long key);

    unsigned long lower_bound(unsigned long key);

    std::set<std::pair<unsigned long, unsigned long>, CustomComparator> S;
    // debug purpose only

    Range cover_by(unsigned long key);

};


#endif //CONCURRENT_MLPDS_RANGESET_H
