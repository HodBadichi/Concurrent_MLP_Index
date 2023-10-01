#ifndef _INCLUDE_UTIL_H_
#define _INCLUDE_UTIL_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "WorkloadInterface.h"


#define DEFAULT_NUM_THREADS 1
#define MAX_ARGS 64

#define YCSB_NUM_OP_TYPES 6


#define MAX_ZIPF_RANGES 10000


typedef struct {
    uint8_t* start_key_buffer;
    uint8_t* end_key_buffer;
    uint8_t* type;
} ct_operation;


typedef struct {
    struct timespec start_time;
} stopwatch_t;

typedef struct {
    // The weight of all operations up to and including this one
    double weight_cumsum;

    uint64_t start;
    uint64_t size;
} zipf_range;

typedef struct {
    zipf_range zipf_ranges[MAX_ZIPF_RANGES];
    uint64_t num_zipf_ranges;
    double total_weight;
    double skew;
    uint64_t max;
    int type;
} rand_distribution;


typedef struct {
    float op_type_probs[YCSB_NUM_OP_TYPES];
    uint64_t num_ops;
    int distribution;
} ycsb_workload_spec;

typedef struct {
    uint8_t* ptr;
    uint64_t size;
    uint64_t pos;
} dynamic_buffer_t;

typedef struct dataset_t_struct {
    uint64_t num_operations;
    void (*read_operation)(struct dataset_t_struct* dataset, ct_operation* operation_buffer);
    void (*close)(struct dataset_t_struct* dataset);
    void* context;
} dataset_t;

typedef struct {
    const char* name;
    int has_value;
} flag_spec_t;

typedef struct {
    const char* name;
    char* value;
} flag_t;

typedef struct {
    int num_args;
    int num_flags;
    flag_t flags[MAX_ARGS];
    char* args[MAX_ARGS];
} args_t;

// Makes the CPU wait until all preceding instructions have completed
// before it starts to execute following instructions.
// Used to make sure that calls to the benchmarked index operation
// (e.g. insert) in consecutive loop iterations aren't overlapped by
// the CPU.
static inline void speculation_barrier() {
    uint32_t unused;
    __builtin_ia32_rdtscp(&unused);
    __builtin_ia32_lfence();
}

int init_dataset(dataset_t* dataset, const char* name);
ct_operation* read_dataset(dataset_t* dataset);
void stopwatch_start(stopwatch_t* stopwatch);
float stopwatch_value(stopwatch_t* stopwatch);
float time_diff(struct timespec* end, struct timespec* start);
args_t* parse_args(const flag_spec_t* flags, int argc, char** argv);
int get_int_flag(args_t* args, const char* name, unsigned int def_value);
uint64_t get_uint64_flag(args_t* args, const char* name, uint64_t def_value);
void rand_zipf_init(rand_distribution* dist, uint64_t max, double skew);
int run_multiple_threads(void* (*thread_func)(void*), int num_threads, void* thread_contexts, int context_size);
void report_mt(float duration, uint64_t num_ops, int num_threads);
void report(float duration, uint64_t num_ops);
#endif
