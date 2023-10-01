#pragma once

#include "common.h"
#include "WorkloadInterface.h"

namespace BenchMarkWorkloads {


    WorkloadUInt64 GenWorkload2M_EXISTS();

    WorkloadUInt64 GenWorkload2M_INSERT();

    WorkloadUInt64 GenWorkload2M_LOWERBOUND();
}
 
