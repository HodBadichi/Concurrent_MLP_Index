#include "common.h"
#include "WorkloadInterface.h"

WorkloadUInt64::WorkloadUInt64()
        : numInitialValues(0), numOperations(0), initialValues(nullptr), operations(nullptr), results(nullptr),
          expectedResults(nullptr) {}

void WorkloadUInt64::AllocateMemory(uint64_t _numInitialValues, uint64_t _numOperations) {
    ReleaseAssert(initialValues == nullptr && operations == nullptr && expectedResults == nullptr);
    numInitialValues = _numInitialValues;
    numOperations = _numOperations;
    initialValues = new Range[numInitialValues];
    ReleaseAssert(initialValues != nullptr);
    operations = new WorkloadOperationUInt64[numOperations]();
    ReleaseAssert(operations != nullptr);
    results = new uint64_t[numOperations];
    ReleaseAssert(results != nullptr);
    memset(results, 0, sizeof(uint64_t) * numOperations);
    expectedResults = new uint64_t[numOperations];
    ReleaseAssert(expectedResults != nullptr);
    memset(expectedResults, 0, sizeof(uint64_t) * numOperations);
}

void WorkloadUInt64::FreeMemory() {
    numInitialValues = 0;
    numOperations = 0;
    if (initialValues) {
        delete[] initialValues;
        initialValues = nullptr;
    }
    if (operations) {
        delete[] operations;
        operations = nullptr;
    }
    if (results) {
        delete[] results;
        results = nullptr;
    }
    if (expectedResults) {
        delete[] expectedResults;
        expectedResults = nullptr;
    }
}

void WorkloadUInt64::PopulateExpectedResultsUsingStdSet() {
    printf("Populating expected results using std::set..\n");
    printf("Populating initial data set..\n");
    {
        rep(i, 0, numInitialValues - 1) {
            S.InsertRange(initialValues[i].first, initialValues[i].second);
        }
    }
    printf("Executing operations..\n");
    {
        rep(i, 0, numOperations - 1) {
            switch (operations[i].type) {
                case WorkloadOperationType::INSERT_RANGE: {
                    expectedResults[i] = S.InsertRange(operations[i].node.range.first,
                                                       operations[i].node.range.second).second;
                    break;
                }
                case WorkloadOperationType::EXIST: {
                    expectedResults[i] = S.count(operations[i].node.key);
                    break;
                }
                case WorkloadOperationType::LOWER_BOUND: {
                    expectedResults[i] = S.lower_bound(operations[i].node.key);
                    break;
                }
                default: {
                    ReleaseAssert(false);
                }
            }
        }
    }
    printf("Complete.");

}

void WorkloadUInt64::WorkloadToFile(std::string file_name) {

    std::ofstream outFile(file_name, std::ios::binary | std::ios::out | std::ios::trunc );

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    uint64_t total_operations = htole64(this->numOperations);

    outFile.write(reinterpret_cast<const char *>(&total_operations), sizeof(total_operations));
    // Iterate through the vector and write the operations to the file in little endian
    for (int i = 0; i < this->numOperations; i++) {
        uint64_t start;
        uint64_t end;
        uint64_t op_type;
        auto Operation = this->operations[i];
        if (Operation.type == INSERT_RANGE) {
            Range range = Operation.node.range;
            start = range.first;
            end = range.second;
            op_type = INSERT_RANGE;
            start = htole64(start);
            end = htole64(end);
            op_type = htole64(op_type);
        } else if (Operation.type == EXIST) {
            uint64_t key = Operation.node.key;
            op_type = EXIST;
            start = htole64(key);
            end = htole64(key);
            op_type = htole64(op_type);
        } else // LowerBound
        {
            uint64_t key = Operation.node.key;
            op_type = LOWER_BOUND;
            start = htole64(key);
            end = htole64(key);
            op_type = htole64(op_type);
        }


        outFile.write(reinterpret_cast<const char *>(&op_type), sizeof(op_type));
        outFile.write(reinterpret_cast<const char *>(&start), sizeof(start));
        outFile.write(reinterpret_cast<const char *>(&end), sizeof(end));
    }

    outFile.close();
}
