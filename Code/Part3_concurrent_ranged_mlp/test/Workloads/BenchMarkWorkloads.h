#pragma once

#include "common.h"
#include "WorkloadInterface.h"

namespace BenchMarkWorkloads
{

    WorkloadUInt64 GenInsertBenchMark(const std::string &dist_type);
    WorkloadUInt64 GenExistBenchMark(const std::string &dist_type);
    WorkloadUInt64 GenLowerBound(const std::string &dist_type);
    WorkloadUInt64 GenMix(const std::string &dist_type);

}
 
