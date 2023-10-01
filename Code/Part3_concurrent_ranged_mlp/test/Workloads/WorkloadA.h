#pragma once

#include "common.h"
#include "WorkloadInterface.h"

namespace WorkloadA {
    WorkloadUInt64 GenWorkload16MLowerBound();
    WorkloadUInt64 GenWorkload16MExists();

    WorkloadUInt64 GenWorkload1();

    WorkloadUInt64 GenWorkLoadEmail();
    WorkloadUInt64 GenWorkLoadEmailLB();
    WorkloadUInt64 GenWorkload16MInsertExistsParallel();
    WorkloadUInt64 GenWorkload16MInsertLowerBoundParllel();
}    // WorkloadA
 
