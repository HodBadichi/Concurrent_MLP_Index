#pragma once

#include "common.h"
#include "../RangeSet/RangeSet.h"
#include <fstream>
enum WorkloadOperationType : uint8_t {
    INSERT_RANGE,
    EXIST,
    LOWER_BOUND,
};

class WorkloadOperationUInt64Node {
public:
    uint64_t key;
    Range range;

    WorkloadOperationUInt64Node() {
        this->key = 0;
        range.first = 0;
        range.second = 0;
    }

};

struct WorkloadOperationUInt64 {
    WorkloadOperationType type;
    WorkloadOperationUInt64Node node;

};

struct WorkloadUInt64 {
    uint64_t numInitialValues;
    uint64_t numOperations;
    Range *initialValues;
    WorkloadOperationUInt64 *operations;
    uint64_t *results;
    uint64_t *expectedResults;
    RangeSet S;

    WorkloadUInt64();

    void AllocateMemory(uint64_t _numInitialValues, uint64_t _numOperations);

    void FreeMemory();

    void PopulateExpectedResultsUsingStdSet();

    void WorkloadToFile(std::string  sFileName);

};

