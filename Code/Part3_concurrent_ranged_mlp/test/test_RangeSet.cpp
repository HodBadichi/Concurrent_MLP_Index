#include "RangeSet/RangeSet.h"
#include <cassert>

int main() {
    RangeSet rangeSet;

    assert(rangeSet.count(0) == 0);
    assert(rangeSet.lower_bound(0) == 0xffffffffffffffffULL);

    // Insert ranges
    Range range1(1, 100);
    Range range2(150, 150);
    Range range3(200, 300);
    rangeSet.InsertRange(range1.first, range1.second);

    assert(rangeSet.count(0) == 0);
    assert(rangeSet.count(1) == 1);
    assert(rangeSet.count(50) == 1);
    assert(rangeSet.count(100) == 1);
    assert(rangeSet.count(101) == 0);

    assert(rangeSet.lower_bound(0) == 1);
    assert(rangeSet.lower_bound(1) == 1);
    assert(rangeSet.lower_bound(50) == 50);
    assert(rangeSet.lower_bound(100) == 100);
    assert(rangeSet.lower_bound(101) == 0xffffffffffffffffULL);


    rangeSet.InsertRange(range2.first, range2.second);
    rangeSet.InsertRange(range3.first, range3.second);

    // Test count function
    assert(rangeSet.count(1) == 1);
    assert(rangeSet.count(100) == 1);
    assert(rangeSet.count(50) == 1);
    assert(rangeSet.count(150) == 1);
    assert(rangeSet.count(200) == 1);
    assert(rangeSet.count(250) == 1);
    assert(rangeSet.count(300) == 1);
    assert(rangeSet.count(6) == 1);

    assert(rangeSet.count(0) == 0);
    assert(rangeSet.count(101) == 0);
    assert(rangeSet.count(149) == 0);
    assert(rangeSet.count(151) == 0);
    assert(rangeSet.count(301) == 0);
    assert(rangeSet.count(199) == 0);



    // Test lower_bound function
    assert(rangeSet.lower_bound(0) == 1);
    assert(rangeSet.lower_bound(1) == 1);
    assert(rangeSet.lower_bound(4) == 4);
    assert(rangeSet.lower_bound(77) == 77);
    assert(rangeSet.lower_bound(100) == 100);
    assert(rangeSet.lower_bound(101) == 150);
    assert(rangeSet.lower_bound(150) == 150);
    assert(rangeSet.lower_bound(151) == 200);
    assert(rangeSet.lower_bound(200) == 200);
    assert(rangeSet.lower_bound(243) == 243);
    assert(rangeSet.lower_bound(300) == 300);
    assert(rangeSet.lower_bound(301) == 0xffffffffffffffffULL);
}

// Add more tests as needed

