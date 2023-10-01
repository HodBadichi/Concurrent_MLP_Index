
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_DIR="$(realpath "$SCRIPT_DIR/../../..")"
RESULTS_DIR="$PROJECT_DIR/Code/Part4_Experiments_maple_mlp/Results"
TimeStamp=$(date +"%Y-%m-%d_%H-%M-%S")
DirectoryName="$RESULTS_DIR/BenchmarkResult_${TimeStamp}"
MAX_TRIES=5

function Build() {
  echo "Building Project..."
  eval "${PROJECT_DIR}/Code/Utils/buildscript.sh" | tee ${DirectoryName}/log.txt
  if [ $? -ne 0 ]; then
      exit
  fi
}


function DeleteArtifacts()
{
    echo "Going to delete artifacts..."
    cd ..
    rm -fr build
}

function RunExperiment()
{
  echo "Going to run experiments..."

  MAPLE_EXE_PATH="${PROJECT_DIR}/build/Code/Part4_Experiments_maple_mlp/maple_tree/MapleTreeBenchMark"
  MLP_EXE_PATH="${PROJECT_DIR}/build/Code/Part4_Experiments_maple_mlp/mlp_index/MlpRangeBenchMark"

 for VERSION in {1..5}; do
     mkdir -p "$DirectoryName/MapleTree/v_$VERSION" >> ${DirectoryName}/log.txt
     mkdir -p "$DirectoryName/MlpIndex/v_$VERSION" >> ${DirectoryName}/log.txt
     for THREAD_NUM in {1..28}; do
         OPTIONAL_ARGS="numactl --cpunodebind=0,1 --interleave=0,1"
         [ $THREAD_NUM -le 14 ] && OPTIONAL_ARGS="numactl --cpunodebind=0 --membind=0"

         for DIST_NAME in "LOW_32" "UNIFORM"; do
             for OP_NAME in "GenInsert" "GenExist" "GenLowerBound" "GenMix"; do

                 echo "Running Version: $VERSION Threads: $THREAD_NUM, dist_name: $DIST_NAME, Workload: $OP_NAME" | tee -a "$DirectoryName/log.txt"
                 COMMON_ARGS_COMMAND="$OP_NAME $DirectoryName/dataset.bin $DIST_NAME --threads $THREAD_NUM"
                 COMMON_LOG_NAME="log_${THREAD_NUM}_${DIST_NAME}_${OP_NAME}.txt"

                 EXEC_COMMAND="$OPTIONAL_ARGS $MAPLE_EXE_PATH $COMMON_ARGS_COMMAND > $DirectoryName/MapleTree/v_${VERSION}/$COMMON_LOG_NAME 2>&1"

                 echo "Going to run: $EXEC_COMMAND" >> "$DirectoryName/log.txt"

                 for TRY in $(seq "$MAX_TRIES"); do
                     eval "$EXEC_COMMAND"
                     if [ $? -eq 0 ]; then
                         break
                     fi

                     echo "Error: Execution failed. Attempt $TRY/$MAX_TRIES" | tee -a ${DirectoryName}/log.txt
                     echo "Trying to run recovery..." | tee -a ${DirectoryName}/log.txt
                 done

                 if [ $? -ne 0 ]; then
                     echo "Recovery failed $MAX_TRIES times. Exiting..."
                     exit 1
                 else
                     echo "Execution succeeded." | tee -a ${DirectoryName}/log.txt
                 fi

                 EXEC_COMMAND="$OPTIONAL_ARGS $MLP_EXE_PATH $COMMON_ARGS_COMMAND > $DirectoryName/MlpIndex/v_${VERSION}/$COMMON_LOG_NAME"
                 echo "Going to run: $EXEC_COMMAND" >> "$DirectoryName/log.txt"

                 for TRY in $(seq $MAX_TRIES); do
                     eval "$EXEC_COMMAND"
                     if [ $? -eq 0 ]; then
                         break
                     fi

                     echo "Error: Execution failed. Attempt $TRY/$MAX_TRIES" | tee -a ${DirectoryName}/log.txt
                     echo "Trying to run recovery..." | tee -a ${DirectoryName}/log.txt
                 done

                 if [ $? -ne 0 ]; then
                     echo "Recovery failed $MAX_TRIES times. Exiting..."
                     exit 1
                 else
                     echo "Execution succeeded."
                 fi

             done
         done
     done
 done
 rm $DirectoryName/dataset.bin

}

function AnalyzeExperiment
{
  echo "Going to analyze experiment..."
  echo "Artifacts directory: ${DirectoryName}"

  EXEC_COMMAND="python3 ${SCRIPT_DIR}/BenchMarksFlow.py --artifacts_dir ${DirectoryName}>> $DirectoryName/log.txt "
  echo "Going to run : $EXEC_COMMAND"
  eval $EXEC_COMMAND
  if [ $? -ne 0 ]; then
      echo "Error: AnalyzeExperiment execution failed. Please check $DirectoryName/log.txt for details."
      exit 1
  fi

    echo "Done analyzing experiments..."
    echo "Please see results in $DirectoryName"

}

mkdir -p $RESULTS_DIR
mkdir -p $DirectoryName

mkdir -p $DirectoryName/MapleTree
mkdir -p $DirectoryName/MlpIndex

if ! Build; then
    echo "Failed to build"
    exit 1
fi

RunExperiment
AnalyzeExperiment