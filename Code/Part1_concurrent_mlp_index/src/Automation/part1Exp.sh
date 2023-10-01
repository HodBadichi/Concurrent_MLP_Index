#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_DIR="$(realpath "$SCRIPT_DIR/../../../..")"
RESULTS_DIR="$PROJECT_DIR/Code/Part1_concurrent_mlp_index/Results"
TimeStamp=$(date +"%Y-%m-%d_%H-%M-%S")
DirectoryName="$RESULTS_DIR/BenchmarkResult_${TimeStamp}"

CONCURRENT_EXE_PATH="${PROJECT_DIR}/build/Code/Part1_concurrent_mlp_index/test/ConcurrentMlpSet"
ORIGIN_EXE_PATH="${PROJECT_DIR}/build/Code/Part1_concurrent_mlp_index/test/OriginMlp"
AUTOMATION_SCRIPT="${SCRIPT_DIR}/BenchmarksFlow.py"

function Build() {
  echo "Building Project..."
  eval "${PROJECT_DIR}/Code/Utils/buildscript.sh" | tee ${DirectoryName}/log.txt
}

function RunExperiment() {
    echo "Going to run experiments..."


    ExecCommand="numactl --cpunodebind=0 --membind=0 $ORIGIN_EXE_PATH --gtest_repeat=5 -t 1"
    echo "Running benchmarks with Origin Mlp Index single threaded..."
    eval "$ExecCommand >> $DirectoryName/Origin/log_1_thread.txt"


    for thread_num in {1..14}
    do
        ExecCommand="numactl --cpunodebind=0 --membind=0 $CONCURRENT_EXE_PATH --gtest_repeat=5 -t $thread_num"
        echo "Running benchmarks with Concurrent Mlp Index with $thread_num Threads"
        eval "$ExecCommand >> $DirectoryName/Concurrent/log_${thread_num}_threads.txt"

    done

    for thread_num in {15..28}
    do
        ExecCommand="numactl --cpunodebind=0,1 --interleave=0,1 $CONCURRENT_EXE_PATH --gtest_repeat=5 -t $thread_num"
        echo "Running benchmarks with Concurrent Mlp Index with $thread_num Threads"
        eval "$ExecCommand >> $DirectoryName/Concurrent/log_${thread_num}_threads.txt"
    done

    echo "Completed running experiments..."
}

function AnalyzeExepriments() {
    echo "Going to analyze experiments..."
    ExecCommand="python3 $AUTOMATION_SCRIPT --artifacts_dir $DirectoryName --threads_num 28"
    echo "Going to run $ExecCommand "
    eval "$ExecCommand"
    echo "Completed run of $ExecCommand"

}

mkdir -p "$DirectoryName"
mkdir -p "$DirectoryName/Origin"
mkdir -p "$DirectoryName/Concurrent"

if ! Build; then
    echo "Failed to build"
    exit 1
fi

RunExperiment
AnalyzeExepriments
