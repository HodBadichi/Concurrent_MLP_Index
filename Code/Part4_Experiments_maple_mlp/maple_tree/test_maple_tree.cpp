#include <csignal>
#include "../util.h"
#include "BenchMarkWorkloads.h"
#include "maple_tree.h"


typedef struct {
    struct maple_tree *m_tree;
    uint8_t *operations;
    uint64_t num_operations;
} mt_operation_ctx;

void *mt_work_thread(void *arg) {
    uint64_t i;
    uint8_t *buf_pos;
    auto ctx = (mt_operation_ctx *) arg;
    maple_tree *const maple_tree = ctx->m_tree;

    buf_pos = ctx->operations;
    for (i = 0; i < ctx->num_operations; i++) {
        auto operation = (ct_operation *) buf_pos;
        auto start_key = *(uint64_t *) (operation->start_key_buffer);
        auto end_key = *(uint64_t *) (operation->end_key_buffer);
        auto type = *(uint64_t *) (operation->type);
        switch (type) {
            case WorkloadOperationType::INSERT_RANGE:
                mtree_insert_range(maple_tree, start_key, end_key, (void *) (end_key), 0);
                break;
            case WorkloadOperationType::EXIST:
                mt_find(maple_tree, &start_key, start_key);
                break;
            case WorkloadOperationType::LOWER_BOUND:
                mt_find_after(maple_tree, &start_key, 0XFFFFFFFFFFFFFFFFULL);
                break;
            default:
                printf("Bad type...");
                exit(1);
        }


        buf_pos += sizeof(ct_operation);
        speculation_barrier();
    }

    return nullptr;
}

void preform_mt_work(dataset_t *dataset, int num_threads, maple_tree *_m_tree) {
    int i;
    struct timespec start_time;
    struct timespec end_time;
    mt_operation_ctx thread_contexts[num_threads];
    ct_operation *operations;


    operations = read_dataset(dataset);

    uint64_t workload_start = 0;
    uint64_t workload_end;
    for (i = 0; i < num_threads; i++) {
        mt_operation_ctx *ctx = &(thread_contexts[i]);
        workload_end = dataset->num_operations * (i + 1) / num_threads;
        ctx->operations = (uint8_t *) (&(operations[workload_start]));
        ctx->num_operations = workload_end - workload_start;
        ctx->m_tree = _m_tree;

        workload_start = workload_end;
    }

    printf("Preforming operations...\n");
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    run_multiple_threads(mt_work_thread, num_threads, thread_contexts, sizeof(mt_operation_ctx));
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    float time_took = time_diff(&end_time, &start_time);
    report_mt(time_took, dataset->num_operations, num_threads);
}

const flag_spec_t FLAGS[] = {
        {"--threads", 1},
        {nullptr,     0}
};

int main(int argc, char **argv) {
    dataset_t dataset;
    args_t *args = parse_args(FLAGS, argc, argv);
    int num_threads;

    num_threads = get_int_flag(args, "--threads", DEFAULT_NUM_THREADS);

    if (args->num_args !=3)
    {
        std::cout<<" Expected 3 arguments but got :'" << args->num_args <<"' instead";
        exit(1);
    }
    std::string workload_name = args->args[0];
    std::string dataset_path = args->args[1];
    std::string dist_name = args->args[2];

    std::cout<<"workload_name: " << workload_name << std::endl;
    std::cout<<"Dataset_path: " << dataset_path << std::endl;
    std::cout<<"distribuition_name: " << dist_name << std::endl;
    std::cout <<"\n\n\n"<<std::endl;
    WorkloadUInt64 workload;


    if (workload_name == "GenInsert")
        workload = BenchMarkWorkloads::GenInsertBenchMark(dist_name);
    else if (workload_name == "GenExist")
        workload = BenchMarkWorkloads::GenExistBenchMark(dist_name);
    else if(workload_name == "GenLowerBound")
        workload = BenchMarkWorkloads::GenLowerBound(dist_name);
    else if( workload_name == "GenMix")
        workload = BenchMarkWorkloads::GenMix(dist_name);
    else
    {
        printf("Bad workload name was given. Options: `GenInsert` , `GenExist`, `GenLowerBound`, `GenMix`");
        exit(1);
    }

    workload.WorkloadToFile(args->args[1]);

    DEFINE_RCU_MTREE(m_tree)


    printf("Inserting initial values before benchmark starts\n");
    for (int i = 0; i < workload.numInitialValues; i++) {
        mtree_insert_range(&m_tree, workload.initialValues[i].first, workload.initialValues[i].second,
                           (void *) (workload.initialValues[i].second), 0);
    }
    printf("Done inserting initial values\n");

    init_dataset(&dataset, args->args[1]);

    //preform-mt-work
    preform_mt_work(&dataset, num_threads, &m_tree);

}
