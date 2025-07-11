#pragma once

#include "common.h"

enum WorkloadOperationType : uint32_t
{
	INSERT,
	EXIST,
	LOWER_BOUND
};

struct WorkloadOperationUInt64
{
	WorkloadOperationType type;
	uint64_t key;
};

struct WorkloadUInt64
{
	uint64_t numInitialValues;
	uint64_t numOperations;
	uint64_t* initialValues;
	WorkloadOperationUInt64* operations;
	uint64_t* results;
	uint64_t* expectedResults;
    set<uint64_t> S;

    WorkloadUInt64();
	
	void AllocateMemory(uint64_t _numInitialValues, uint64_t _numOperations);
	
	void FreeMemory();
	
	void PopulateExpectedResultsUsingStdSet();
	
	// Encrypt the next query's content with the previous query's expected result
	// This fully prevents any possible CPU out-of-order execution across queries
	//
	void EnforceDependency();
	
};

