# Run tests.
# modify data.
# Create charts.
import argparse
import csv
import os
import re
import subprocess
import logging

Logger = None

CPU_THREADS_BOUND = 14


def SetLogger(sLogFilePath):
    global Logger
    # Create a logger
    Logger = logging.getLogger('BenchMarksFlowLogger')
    Logger.setLevel(logging.DEBUG)

    # Create a formatter
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    # Create a file handler to log to a file
    file_handler = logging.FileHandler(sLogFilePath)
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(formatter)

    # Create a stream handler to log to stdout
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.INFO)
    stream_handler.setFormatter(formatter)

    # Add the handlers to the logger
    Logger.addHandler(file_handler)
    Logger.addHandler(stream_handler)


class CBenchMarkData():
    def __init__(self, sBenchMarkName, nThreads):
        self.sName = sBenchMarkName
        self.lRuns = []
        self.nThreads = nThreads;
        self.nRuns = 0

    def AddRun(self, nOPsML, nRunTime):
        dRunInfo = {}
        dRunInfo['nOpsML'] = float(nOPsML)
        dRunInfo['nRunTime'] = float(nRunTime)
        self.lRuns.append(dRunInfo)
        self.nRuns += 1

    @property
    def AverageOpsML(self):
        nSum = 0
        for dRunInfo in self.lRuns:
            nSum += dRunInfo['nOpsML']

        nAverage = nSum / self.nRuns

        return nAverage

    @property
    def AverageRunTime(self):
        nSum = 0
        for dRunInfo in self.lRuns:
            nSum += dRunInfo['nRunTime']

        nAverage = nSum / self.nRuns

        return nAverage


def ParseArguments():
    parser = argparse.ArgumentParser(description="Process a file.")

    parser.add_argument("--file_path",
                        required=True,
                        help="Path to the benchmarks executable file")

    parser.add_argument("--artifacts_dir", required=True)

    parser.add_argument("--thread_num",
                        type=int,
                        default=1,
                        help="Number of threads to run the benchmarks with will execute from 1 to |thread_num|")

    args = parser.parse_args()

    if not os.path.exists(args.file_path):
        raise FileNotFoundError(f"Failed to find executable !.{os.linesep} {args.file_path} does not exist")

    return args


def ExtractBenchMarksData(sStdOut, nThreads):
    test_name_pattern = r'Running test: (\w+)'
    second_elapsed_pattern = r'AutoTimer: ([\d.]+) second elapsed'
    ops_ml_pattern = r'MlpSet ops/ml: (\d+\.\d+)M'

    # Find all matches for each pattern
    lTestNames = re.findall(test_name_pattern, sStdOut)
    lRunTimes = re.findall(second_elapsed_pattern, sStdOut)
    lOpsMLS = re.findall(ops_ml_pattern, sStdOut)

    if not (len(lTestNames) == len(lRunTimes) == len(lOpsMLS)):
        print("Parsing Google Test result went wrong !")
        exit(1)

    lExtractedData = list(zip(lTestNames, lRunTimes, lOpsMLS))

    dExistBenchMarks = {}
    lExistBenchMarks = []
    for ExtractedData in lExtractedData:
        BenchMarkName = ExtractedData[0]
        BenchMarkRunTime = ExtractedData[1]
        BenchMarkMLOPS = ExtractedData[2]

        if BenchMarkName not in lExistBenchMarks:
            NewBenchMarkData = CBenchMarkData(BenchMarkName, nThreads)
            NewBenchMarkData.AddRun(BenchMarkMLOPS, BenchMarkRunTime)
            dExistBenchMarks[f'{BenchMarkName}'] = NewBenchMarkData
            lExistBenchMarks.append(BenchMarkName)
        else:
            dExistBenchMarks[f'{BenchMarkName}'].AddRun(BenchMarkMLOPS, BenchMarkRunTime)

    return dExistBenchMarks


def main():
    global Logger
    args = ParseArguments()
    sBenchMarksExePath = args.file_path
    nThreadsNum = args.thread_num
    sArtifactsDir = args.artifacts_dir
    sLogFile = os.path.join(sArtifactsDir, 'log.txt')
    SetLogger(sLogFile)

    # Run the benchmarks
    lExperimentsData = []

    for thread_num in range(1, nThreadsNum + 1):
        Logger.info(f"Running benchmarks with {thread_num} threads...")
        try:
            if int(thread_num) > CPU_THREADS_BOUND:
                sNumaCommand = "numactl --cpunodebind=0,1 --interleave=0,1"
            else:
                sNumaCommand = "numactl --cpunodebind=0 --membind=0"

            sCommand = f"{sNumaCommand} {sBenchMarksExePath} --gtest_repeat=5 -t {thread_num}"
            Logger.info(f"Executing: '{sCommand}'")
            BenchmarksProcess = subprocess.run(sCommand.split(), check=True,
                                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            Logger.info("Finished execution successfully..." + os.linesep)

            sStdOut = BenchmarksProcess.stdout.decode('utf-8')
            sStdErr = BenchmarksProcess.stderr.decode('utf-8')
            Logger.info("@@@stderr begin@@@")
            Logger.info(sStdErr)
            Logger.info("@@@stderr end@@@")
            Logger.debug(sStdOut)

            dExperimentBlocks = ExtractBenchMarksData(sStdOut, thread_num)
            lExperimentsData.append(dExperimentBlocks)

        except BaseException as e:
            Logger.error(f"An Error occurred !. reason : {e}")
            raise

    lBenchMarksNames = [sBenchMarkName for sBenchMarkName in lExperimentsData[0]]

    dBenchMarksDFS = {}
    for sBenchMarkName in lBenchMarksNames:
        dBenchMarksDFS[sBenchMarkName] = {}

        dBenchMarksDFS[sBenchMarkName]['Threads'] = []
        dBenchMarksDFS[sBenchMarkName]['RunTime'] = []
        dBenchMarksDFS[sBenchMarkName]['OPSML'] = []
        dBenchMarksDFS[sBenchMarkName]['RUNS'] = []

        for dExperimentBlocks in lExperimentsData:
            BenchMarkData: CBenchMarkData = dExperimentBlocks[sBenchMarkName]
            dBenchMarksDFS[sBenchMarkName]['Threads'].append(BenchMarkData.nThreads)
            dBenchMarksDFS[sBenchMarkName]['RunTime'].append(BenchMarkData.AverageRunTime)
            dBenchMarksDFS[sBenchMarkName]['OPSML'].append(BenchMarkData.AverageOpsML)
            dBenchMarksDFS[sBenchMarkName]['RUNS'].append(BenchMarkData.nRuns)

    dataframes = {}
    for sBenchMarkName, data in dBenchMarksDFS.items():
        Logger.debug(f"Saving benchmark {sBenchMarkName}...")
        dataframes[sBenchMarkName] = data

        # Save the data as a CSV file within the directory
        sCSVfileName = os.path.join(sArtifactsDir, f"{sBenchMarkName}.csv")
        column_headers = list(data.keys())
        rows = zip(*data.values())

        with open(sCSVfileName, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)

            # Write the column headers as the first row
            writer.writerow(column_headers)

            # Write the rows
            writer.writerows(rows)


if __name__ == "__main__":
    main()
