#define _GNU_SOURCE  // For <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include "util.h"



uint64_t rand_state;

float time_diff(struct timespec *end, struct timespec *start) {
    float time_diff = end->tv_sec - start->tv_sec;
    time_diff += (1.0 / 1000000000) * (end->tv_nsec - start->tv_nsec);
    return time_diff;
}

void stopwatch_start(stopwatch_t *stopwatch) {
    clock_gettime(CLOCK_MONOTONIC, &(stopwatch->start_time));
}

float stopwatch_value(stopwatch_t *stopwatch) {
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    return time_diff(&end_time, &(stopwatch->start_time));
}

void file_dataset_read_operation(dataset_t *dataset, ct_operation *operation_buf) {
    int items_read;
    FILE *dataset_file = (FILE *) (dataset->context);

    items_read = fread(operation_buf->type, 8, 1, dataset_file);
    if (items_read != 1) {
        printf("Error reading type field\n");
        return;
    }

    items_read = fread(operation_buf->start_key_buffer, 8, 1, dataset_file);
    if (items_read != 1) {
        printf("Error reading start range key\n");
        return;
    }

    items_read = fread(operation_buf->end_key_buffer, 8, 1, dataset_file);
    if (items_read != 1) {
        printf("Error reading start range key\n");
        return;
    }
}

void file_dataset_close(dataset_t *dataset) {
    fclose((FILE *) (dataset->context));
}

// TODO: Support random datasets with random key lengths
// TODO: Support random datasets that guarantee unique keys
int init_dataset(dataset_t *dataset, const char *name) {
    int items_read;
    FILE *dataset_file;

    dataset_file = fopen(name, "rb");
    if (!dataset_file)
        return 0;

    items_read = fread(&(dataset->num_operations), sizeof(dataset->num_operations), 1, dataset_file);
    if (items_read != 1)
        goto close_and_fail;

    dataset->read_operation = file_dataset_read_operation;
    dataset->close = file_dataset_close;
    dataset->context = dataset_file;
    return 1;

    close_and_fail:
    fclose(dataset_file);
    return 0;
}

ct_operation *read_dataset(dataset_t *dataset) {
    uint64_t i;
    uint8_t *buf;
    uint8_t *buf_pos;
    ct_operation *operations;

    buf = (uint8_t *) malloc((dataset->num_operations * sizeof(ct_operation)) + 1000);
    operations = (ct_operation *) malloc(dataset->num_operations * sizeof(ct_operation));

    buf_pos = buf;
    for (i = 0; i < dataset->num_operations; i++) {
        ct_operation *operation = &(operations[i]);
        operation->type = buf_pos;
        operation->start_key_buffer = buf_pos + sizeof(operation->type);
        operation->end_key_buffer = buf_pos + sizeof(operation->start_key_buffer) + sizeof(operation->type);

        dataset->read_operation(dataset, operation);

        buf_pos += sizeof(ct_operation);
    }

    return operations;
}


int get_int_flag(args_t *args, const char *name, unsigned int def_value) {
    int i;
    for (i = 0; i < args->num_flags; i++) {
        if (!strcmp(args->flags[i].name, name))
            return atoi(args->flags[i].value);
    }
    return def_value;
}

uint64_t get_uint64_flag(args_t *args, const char *name, uint64_t def_value) {
    int i;
    for (i = 0; i < args->num_flags; i++) {
        if (!strcmp(args->flags[i].name, name))
            return strtoull(args->flags[i].value, NULL, 0);
    }
    return def_value;
}

args_t *parse_args(const flag_spec_t *flags, int argc, char **argv) {
    args_t *args;
    int i;
    const flag_spec_t *flag;
    if (argc > MAX_ARGS) {
        return NULL;
    }

    args = (args_t *) malloc(sizeof(args_t));
    args->num_flags = 0;
    args->num_args = 0;
    for (i = 1; i < argc; i++) {
        flag = flags;
        while (flag->name != NULL) {
            if (strcmp(flag->name, argv[i])) {
                flag++;
                continue;
            }

            // Found a known flag
            args->flags[args->num_flags].name = argv[i];
            if (flag->has_value) {
                i++;
                if (i == argc)
                    return NULL;
                args->flags[args->num_flags].value = argv[i];
            }
            args->num_flags++;
            break;
        }
        if (flag->name == NULL) {
            // argv[i] is not a known flag, so it is an argument
            args->args[args->num_args++] = argv[i];
        }
    }
    return args;
}

typedef struct {
    void *(*thread_func)(void *);

    void *arg;
    int cpu;
} run_with_affinity_ctx;

void *run_with_affinity(void *arg) {
    run_with_affinity_ctx *ctx = (run_with_affinity_ctx *) arg;
    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);
    CPU_SET(ctx->cpu, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set);
    return ctx->thread_func(ctx->arg);
}

int run_multiple_threads(void *(*thread_func)(void *), int num_threads, void *thread_contexts, int context_size) {
    uint64_t i;
    int result;
    int cpu = 0;
    run_with_affinity_ctx wrapper_contexts[num_threads];
    pthread_t threads[num_threads];
    cpu_set_t mask;

    sched_getaffinity(0, sizeof(cpu_set_t), &mask);

    for (i = 0; i < num_threads; i++) {
        run_with_affinity_ctx *wrapper_ctx = &(wrapper_contexts[i]);

        // Find next allowed CPU
        while (!CPU_ISSET(cpu, &mask)) {
            cpu++;
            if (cpu == CPU_SETSIZE) {
                printf("Not enough CPUs for all threads\n");
                exit(1);
            }
        }

        wrapper_ctx->thread_func = thread_func;
        wrapper_ctx->arg = (char *) thread_contexts + context_size * i;
        wrapper_ctx->cpu = cpu;
        result = pthread_create(&(threads[i]), NULL, run_with_affinity, wrapper_ctx);
        if (result != 0) {
            printf("Thread creation error\n");
            return 0;
        }

        // Run the next thread on another CPU
        cpu++;
    }

    for (i = 0; i < num_threads; i++) {
        result = pthread_join(threads[i], NULL);
        if (result != 0) {
            printf("Thread join error\n");
            return 0;
        }
    }
    return 1;
}

void report_mt(float duration, uint64_t num_ops, int num_threads) {
    printf("Took %.2fs for %lu ops in %d threads (%.0fns/op, %.2fMops/s)\n",
           duration, num_ops, num_threads,
           (duration / num_ops * num_threads) * 1.0e9,
           (num_ops / duration ) / 1.0e6);

    printf("RESULT: ops=%lu threads=%d ms=%d\n", num_ops, num_threads, (int) (duration * 1000));
}

void report(float duration, uint64_t num_ops) {
    printf("Took %.2fs for %lu ops (%.0fns/op)\n", duration, num_ops, duration / num_ops * 1.0e9);
    printf("RESULT: ops=%lu ms=%d\n", num_ops, (int) (duration * 1000));
}