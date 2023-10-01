//
// Created by hodbadihi on 9/22/23.
//

#include "RangeSet.h"
#include <cassert>

std::pair<bool, bool> RangeSet::InsertRange(unsigned long start_range, unsigned long end_range) {
    if (this->count(start_range) != 0)
        assert(this->count(start_range) == 0);
    assert(this->count(end_range) == 0);
    auto item = Range({start_range, end_range});
    bool result = this->S.insert(item).second;
    return std::pair<bool, bool>({result, result});
}


unsigned long RangeSet::count(unsigned long key) {

    std::pair<unsigned long, unsigned long> dummy_range(key, key);
    auto it = S.lower_bound(dummy_range);

    if (S.empty()) {
        return 0;
    }

    if (it != S.end() && it->first == key)
        return 1;
    else if (it == S.begin())
        return 0;
    else {
        it--;
        if (it->first <= key && it->second >= key)
            return 1;
        else
            return 0;
    }

}

unsigned long RangeSet::lower_bound(unsigned long key) {
    std::pair<unsigned long, unsigned long> dummy_range(key, key);
    auto it = S.lower_bound(dummy_range);

    if (S.empty()) {
        return 0xffffffffffffffffULL;
    }

    if (it->first == key)
        return key;
    else if (it == S.begin()) {
        return it->first;
    } else {
        auto old_it = it;
        it--;
        if (it->second < key && old_it == S.end()) {
            return 0xffffffffffffffffULL;
        } else if (it->first <= key && it->second >= key) {
            return key;
        } else {
            it++;
            return it->first;
        }
    }
}

Range RangeSet::cover_by(unsigned long key) {
    std::pair<unsigned long, unsigned long> dummy_range(key, key);
    auto it = S.lower_bound(dummy_range);

    if (S.empty()) {
        return Range({1,0});
    }

    if (it != S.end() && it->first == key)
        return *it;
    else if (it == S.begin())
        return Range({1,0});
    else {
        it--;
        if (it->first <= key && it->second >= key)
            return *it;
        else
            return Range({1,0});
    }
}
