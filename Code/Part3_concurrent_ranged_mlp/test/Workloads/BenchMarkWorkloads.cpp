#include "common.h"
#include "WorkloadInterface.h"
#include "LightRangeGenerator.h"

namespace BenchMarkWorkloads {
    const int COMMON_N = 10000;
    const int COMMON_Q = 1000000;
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max() - 1);

    WorkloadUInt64 GenInsertBenchMark(const std::string &dist_type) {
        const int N = 1;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        gap_between_ranges_behaviour_t gbr;
        ranges_sizes_behaviour_t rsb;
        if (dist_type == "LOW_32") {
            rsb = LOW_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;

        } else if (dist_type == "UNIFORM") {
            rsb = ALMOST_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;
        } else {
            printf("Bad dist type\n");
            exit(1);
        }

        LightRandomGenerator LGR(rsb, gbr);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);
	workload.AllocateMemory(N, Q);

        Range temp = *(LGR.Next());
        workload.initialValues[0].first = temp.first;
        workload.initialValues[1].second = temp.second;

	std::vector<pair64*> ranges = *LGR.DumpToVector(Q); //Shuffled vector of ranges

        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::INSERT_RANGE;
            pair<uint64_t, uint64_t> *R = ranges[i];

            workload.operations[i].node.range = *R;
            assert(workload.operations[i].node.range.second >= workload.operations[i].node.range.first);
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenExistBenchMark(const std::string &dist_type) {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        gap_between_ranges_behaviour_t gbr;
        ranges_sizes_behaviour_t rsb;
        if (dist_type == "LOW_32") {
            rsb = LOW_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;

        } else if (dist_type == "UNIFORM") {
            rsb = ALMOST_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;
        } else {
            printf("Bad dist type\n");
            exit(1);
        }

        LightRandomGenerator LGR(rsb, gbr);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);

        workload.AllocateMemory(N, Q);

	std::vector<pair64*> ranges = *LGR.DumpToVector(N); //Shuffled vector of ranges
        rep(i, 0, N - 1) {
            Range *R = ranges[i];
            workload.initialValues[i] = *R;
            assert(workload.initialValues[i].second >= workload.initialValues[i].first);
        }

        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::EXIST;
            if (rand() % 4 != 0) {
                Range val = workload.initialValues[rand() % N];
                if (val.second == val.first)
                    workload.operations[i].node.key = val.first;
                else
                    workload.operations[i].node.key = val.first + rand() % (val.second - val.first);
            } else {
                uint64_t key = 0;
                if (dist_type == "LOW_32") {
                    rep(k, 0, 3) {
                        key = key * 256 + rand() % 5 + 48;
                    }
                    rep(k, 4, 7) {
                        key = key * 256 + rand() % 64 + 32;
                    }
                } else if (dist_type == "UNIFORM") {
                    key = dist(rng);
                }

                workload.operations[i].node.key = key;
            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenLowerBound(const std::string &dist_type) {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        gap_between_ranges_behaviour_t gbr;
        ranges_sizes_behaviour_t rsb;
        if (dist_type == "LOW_32") {
            rsb = LOW_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;

        } else if (dist_type == "UNIFORM") {
            rsb = ALMOST_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;
        } else {
            printf("Bad dist type\n");
            exit(1);
        }

        LightRandomGenerator LGR(rsb, gbr);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);

        workload.AllocateMemory(N, Q);

	std::vector<pair64*> ranges = *LGR.DumpToVector(N); //Shuffled vector of ranges
        rep(i, 0, N - 1) {
            Range *R = ranges[i];
            workload.initialValues[i] = *R;
            assert(workload.initialValues[i].second >= workload.initialValues[i].first);
        }
        rep(i, 0, Q - 1) {
            workload.operations[i].type = WorkloadOperationType::LOWER_BOUND;
            if (rand() % 4 != 0) {
                Range val = workload.initialValues[rand() % N];
                if (val.second == val.first) {
                    workload.operations[i].node.key = val.first;
                } else {
                    workload.operations[i].node.key = val.first + rand() % (val.second - val.first);
                }


            } else {
                uint64_t key = 0;
                if (dist_type == "LOW_32") {
                    rep(k, 0, 3) {
                        key = key * 256 + rand() % 5 + 48;
                    }
                    rep(k, 4, 7) {
                        key = key * 256 + rand() % 64 + 32;
                    }
                } else if (dist_type == "UNIFORM") {
                    key = dist(rng);
                }
                workload.operations[i].node.key = key;

            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }

    WorkloadUInt64 GenMix(const std::string &dist_type) {
        const int N = COMMON_N;
        const int Q = COMMON_Q;
        WorkloadUInt64 workload;
        gap_between_ranges_behaviour_t gbr;
        ranges_sizes_behaviour_t rsb;
        if (dist_type == "LOW_32") {
            rsb = LOW_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;

        } else if (dist_type == "UNIFORM") {
            rsb = ALMOST_UNIFORM_RANGES;
            gbr = NO_LARGE_GAPS;
        } else {
            printf("Bad dist type\n");
            exit(1);
        }

        LightRandomGenerator LGR(rsb, gbr);
        LGR.SetOverlapPolicy(RETRY_N_TIMES_THEN_ASSERT);

        workload.AllocateMemory(N, Q);

	std::vector<pair64*> ranges = *LGR.DumpToVector(N); //Shuffled vector of ranges
        rep(i, 0, N - 1) {
            Range *R = ranges[i];
            workload.initialValues[i] = *R;
            assert(workload.initialValues[i].second >= workload.initialValues[i].first);
        }

	ranges = *LGR.DumpToVector(Q);
        rep(i, 0, Q - 1) {
            if (rand() % 4 != 0) {
                //get key - exists or not lets favor pos keys in 80% and 20 %
                if (rand() % 5 != 0) {
                    Range val = workload.initialValues[rand() % N];
                    if (val.second == val.first) {
                        workload.operations[i].node.key = val.first;
                    } else {
                        workload.operations[i].node.key = val.first + rand() % (val.second - val.first);
                    }


                } else {
                    uint64_t key = 0;
                    if (dist_type == "LOW_32") {
                        rep(k, 0, 3) {
                            key = key * 256 + rand() % 5 + 48;
                        }
                        rep(k, 4, 7) {
                            key = key * 256 + rand() % 64 + 32;
                        }
                    } else if (dist_type == "UNIFORM") {
                        key = dist(rng);
                    }
                    workload.operations[i].node.key = key;

                }

                if (rand() % 2)
                    workload.operations[i].type = EXIST;
                else
                    workload.operations[i].type = LOWER_BOUND;

            } else {
                workload.operations[i].type = WorkloadOperationType::INSERT_RANGE;
                Range *R = ranges[i];
                workload.operations[i].node.range = *R;
            }
        }
        workload.PopulateExpectedResultsUsingStdSet();
        return workload;
    }
}    // namespace BenchMarkWorkloads
