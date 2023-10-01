import argparse
import csv
import os
import re


class CBenchMarkData:
    def __init__(self, sBenchMarkName, nThreads):
        self.sName = sBenchMarkName
        self.lRuns = []
        self.nThreads = nThreads
        self.nRuns = 0

    def AddRun(self, nOPsML, nRunTime):
        dRunInfo = {'nOpsML': float(nOPsML), 'nRunTime': float(nRunTime)}
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

    parser.add_argument("--artifacts_dir", required=True)

    parser.add_argument("--threads_num",
                        type=int,
                        default=1,
                        help="Number of threads to run the benchmarks with will execute from 1 to |thread_num|")

    parser.add_argument("--plot", type=bool, default=False, help="Plot the results")
    args = parser.parse_args()

    if not os.path.exists(args.artifacts_dir):
        raise FileNotFoundError(f"Failed to find artifacts dir !.{os.linesep} {args.artifacts_dir} does not exist")

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
        print(f"Parsing Google Test result went wrong for {nThreads} Threads..")
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


def ExecuteConcurrentFlow(sArtifactsDir: str, nThreadsNum: int):
    # Run the benchmarks
    lExperimentsData = []

    for thread_num in range(1, nThreadsNum + 1):
        sFilePath = os.path.join(sArtifactsDir, f'log_{thread_num}_threads.txt')
        with open(sFilePath, 'r') as ExperimentLogFile:
            sStdOut = ExperimentLogFile.read()

        dExperimentBlocks = ExtractBenchMarksData(sStdOut, thread_num)
        lExperimentsData.append(dExperimentBlocks)

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
        sCSVfileName = os.path.join(sArtifactsDir, f"{sBenchMarkName}.csv")

        print(f"Saving Concurrent run benchmark '{sBenchMarkName}' at '{sCSVfileName}'")
        dataframes[sBenchMarkName] = data

        column_headers = list(data.keys())
        rows = zip(*data.values())

        with open(sCSVfileName, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)

            # Write the column headers as the first row
            writer.writerow(column_headers)

            # Write the rows
            writer.writerows(rows)


def ExecuteOriginFlow(sArtifactsDir: str):
    # Run the benchmarks
    lExperimentsData = []

    sFilePath = os.path.join(sArtifactsDir, f'log_1_thread.txt')
    with open(sFilePath, 'r') as ExperimentLogFile:
        sStdOut = ExperimentLogFile.read()

    dExperimentBlocks = ExtractBenchMarksData(sStdOut, 1)
    lExperimentsData.append(dExperimentBlocks)

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
        sCSVfileName = os.path.join(sArtifactsDir, f"{sBenchMarkName}.csv")

        print(f"Saving `Origin` run benchmark '{sBenchMarkName}' at '{sCSVfileName}'")
        dataframes[sBenchMarkName] = data

        column_headers = list(data.keys())
        rows = zip(*data.values())

        with open(sCSVfileName, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)

            # Write the column headers as the first row
            writer.writerow(column_headers)

            # Write the rows
            writer.writerows(rows)


# Define a function for plotting
def scatter_comparison(ax, df_concurrent, df_origin, x_column, y_column,y_column_title, title):
    ax.scatter(df_concurrent[x_column], df_concurrent[y_column], label="Concurrent", marker='o')
    ax.scatter(df_origin[x_column], df_origin[y_column], label="Origin", marker='x')

    ax.set_xlabel(x_column)
    ax.set_ylabel(y_column_title)
    ax.set_title(title)
    ax.legend()


def PlotResults(sOriginArtifactsPath, sConcurrentArtifactsPath):
    import pandas as pd
    import matplotlib.pyplot as plt

    sGraphsDirectory = os.path.abspath(os.path.join(sOriginArtifactsPath,os.pardir))

    ConcurrentExistDF = pd.read_csv(os.path.join(sConcurrentArtifactsPath, 'WorkLoad_2M_EXIST.csv'))
    ConcurrentInsertDF = pd.read_csv(os.path.join(sConcurrentArtifactsPath, 'WorkLoad_2M_INSERT.csv'))
    ConcurrentLowerBoundDF = pd.read_csv(os.path.join(sConcurrentArtifactsPath, 'WorkLoad_2M_LOWERBOUND.csv'))

    OriginExistDF = pd.read_csv(os.path.join(sOriginArtifactsPath, 'WorkLoad_2M_EXIST.csv'))
    OriginInsertDF = pd.read_csv(os.path.join(sOriginArtifactsPath, 'WorkLoad_2M_INSERT.csv'))
    OriginLowerBoundDF = pd.read_csv(os.path.join(sOriginArtifactsPath, 'WorkLoad_2M_LOWERBOUND.csv'))

    # Plot for Runtime
    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    fig.subplots_adjust(wspace=0.4)

    for ax in axes:
        ax.set_xlabel("Threads", fontsize=14)
        ax.set_ylabel("Run Time [milliseconds]", fontsize=14)

    # Graph 4: Insert DF, RunTime
    scatter_comparison(axes[0], ConcurrentInsertDF, OriginInsertDF, "Threads", "RunTime", "Run Time [Milliseconds]", "`Insert` (2M OPS)")

    # Graph 5: LowerBound DF, RunTime
    scatter_comparison(axes[1], ConcurrentLowerBoundDF, OriginLowerBoundDF, "Threads", "RunTime", "Run Time [Milliseconds]", "`LowerBound` (2M OPS)")

    # Graph 6: Exist DF, RunTime
    scatter_comparison(axes[2], ConcurrentExistDF, OriginExistDF, "Threads", "RunTime", "Run Time [Milliseconds]", "`Exist` (2M OPS)")

    # Adjust layout
    plt.tight_layout()

    # Display the plot
    plt.savefig(os.path.join(sGraphsDirectory, 'RuntimeResult.jpg'))
    plt.show()

    # Plot for OPS/ML
    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    fig.subplots_adjust(wspace=0.4)
    for ax in axes:
        ax.set_xlabel("Threads", fontsize=14)
        ax.set_ylabel("ops/ms [Millions]", fontsize=14)

    # Graph 1: Exist DF, OPSML
    scatter_comparison(axes[0], ConcurrentExistDF, OriginExistDF, "Threads", "OPSML", "ops/ms [Millions]", "`Exist` (2M OPS)")

    # Graph 2: LowerBound DF, OPSML
    scatter_comparison(axes[1], ConcurrentLowerBoundDF, OriginLowerBoundDF, "Threads", "OPSML", "ops/ms [Millions]", "`LowerBound` (2M OPS)")

    # Graph 3: Insert DF, OPSML
    scatter_comparison(axes[2], ConcurrentInsertDF, OriginInsertDF, "Threads", "OPSML", "ops/ms [Millions]", "`Insert` (2M OPS)")

    # Adjust layout
    plt.tight_layout()

    # Display the plot
    plt.savefig(os.path.join(sGraphsDirectory, 'OPSMLResult.jpg'))
    plt.show()




def main():
    args = ParseArguments()
    nThreadsNum = args.threads_num
    sArtifactsDir = args.artifacts_dir
    bPlot = args.plot

    ExecuteConcurrentFlow(os.path.join(sArtifactsDir, 'Concurrent'), nThreadsNum)
    ExecuteOriginFlow(os.path.join(sArtifactsDir, 'Origin'))
    if bPlot:
        PlotResults(os.path.join(sArtifactsDir, 'Origin'), os.path.join(sArtifactsDir, 'Concurrent'))


if __name__ == "__main__":
    main()
