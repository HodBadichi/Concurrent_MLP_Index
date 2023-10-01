import argparse
import csv
import os
import re

lBenchMarksNames = ["GenMix", "GenLowerBound", "GenInsert", "GenExist"]
nThreads = 28
nVersions = 5
lDistNames = ["LOW_32", "UNIFORM"]
lDataStructureNames = ["MapleTree", "MlpIndex"]


def ParseArguments():
    parser = argparse.ArgumentParser(description="Process a file.")

    parser.add_argument("--artifacts_dir", required=True)
    parser.add_argument("--plot", default=False, required=False)

    args = parser.parse_args()

    if not os.path.exists(args.artifacts_dir):
        raise FileNotFoundError(f"Failed to find artifacts dir !.{os.linesep} {args.artifacts_dir} does not exist")

    return args


def GetLogFile(sArtifactsDir, sBenchMarkName, sDataStruct, nThread, nVersion, sDistName):
    sLogFileName = f"log_{nThread}_{sDistName}_{sBenchMarkName}.txt"
    return os.path.join(sArtifactsDir, sDataStruct, f"v_{nVersion}", sLogFileName)


def GetLogFileData(sLogFilePath, sDataStructureName, sDistName):
    dLogFileData = {}

    with open(sLogFilePath, 'r') as file:
        sLogData = file.read()
        # Extract ns/op field
        ns_op_match = re.search(r'(\d+)ns/op', sLogData)
        ns_op = float(ns_op_match.group(1)) if ns_op_match else None

        mops_match = re.search(r'(\d+\.\d+)Mops/s', sLogData)
        mops = float(mops_match.group(1)) if mops_match else None

        # Extract ms field
        ms_match = re.search(r'ms=(\d+)', sLogData)
        ms = int(ms_match.group(1)) if ms_match else None

        ops_match = re.search(r'ops=(\d+)', sLogData)
        ops = int(ops_match.group(1)) if ops_match else None

    dLogFileData[f'ns/ops-{sDataStructureName}-{sDistName}'] = ns_op
    dLogFileData[f'mops/s-{sDataStructureName}-{sDistName}'] = mops
    dLogFileData[f'total_time-{sDataStructureName}-{sDistName}'] = ms
    dLogFileData['total-ops'] = ops

    return dLogFileData


def WriteData(sArtifactsDir: str, bPlot: bool):
    dBenchMarkPds = {}
    if bPlot:
        import pandas as pd
        import matplotlib.pyplot as plt

    for sBenchMarkName in lBenchMarksNames:
        # Create new csv file
        sCSVfileName = os.path.join(sArtifactsDir, f"{sBenchMarkName}.csv")
        bFirstWrite = True
        for nThread in range(1, nThreads):
            dThreadRecord = {'Thread': nThread}
            for sDistName in lDistNames:
                for sDataStructureName in lDataStructureNames:
                    dAverageLogFileData = {f'total-ops': 0,
                                           f'total_time-{sDataStructureName}-{sDistName}': 0,
                                           f'mops/s-{sDataStructureName}-{sDistName}': 0,
                                           f'ns/ops-{sDataStructureName}-{sDistName}': 0}
                    for nVersion in range(1, nVersions):
                        sLogFilePath = GetLogFile(sArtifactsDir, sBenchMarkName, sDataStructureName, nThread, nVersion,
                                                  sDistName)
                        dLogFileData = GetLogFileData(sLogFilePath, sDataStructureName, sDistName)

                        for key in dAverageLogFileData:
                            dAverageLogFileData[key] += dLogFileData[key]

                    for key in dAverageLogFileData:
                        dAverageLogFileData[key] = dAverageLogFileData[key] / nVersions

                    dThreadRecord.update(dAverageLogFileData)

            with open(sCSVfileName, 'a', newline='') as csvfile:
                writer = csv.writer(csvfile)
                if bFirstWrite:
                    writer.writerow(dThreadRecord.keys())
                    bFirstWrite = False

                writer.writerow(dThreadRecord.values())
        if bPlot:
            dBenchMarkPds[sBenchMarkName] = pd.read_csv(sCSVfileName)

    if bPlot:
        for sBenchMarkName in lBenchMarksNames:
            for sDistName in lDistNames:
                sRegExp = f".*mops/s.*{sDistName}.*|.*{sDistName}.*mops/s.*|total-ops|Thread"
                CurrentPd = dBenchMarkPds[sBenchMarkName].filter(regex=sRegExp)
                CurrentPd.columns = CurrentPd.columns.str.strip().str.replace('\n', '')

                # Plotting
                plt.figure(figsize=(6, 4.5))

                # Plot MapleTree
                plt.plot(CurrentPd["Thread"], CurrentPd[f"mops/s-MapleTree-{sDistName}"], label="MapleTree", marker='o')

                # Plot MapleIndex
                plt.plot(CurrentPd["Thread"], CurrentPd[f"mops/s-MlpIndex-{sDistName}"], label="MlpIndex",
                         marker='o')

                # Add title
                plt.title(
                    f"'{sBenchMarkName}', '{sDistName}', {int((int(float(CurrentPd['total-ops'][0]))) / 1000)}k ops")

                # Add legend
                plt.legend()

                # Add labels
                plt.xlabel("Threads", fontsize=14)
                plt.ylabel("Mops/s[Millions]", fontsize=14)

                # Show the plot
                sFileName = f"{sBenchMarkName}-{sDistName}.png"
                sSavePath = os.path.join(sArtifactsDir, sFileName)
                plt.savefig(sSavePath)


def main():
    args = ParseArguments()
    sArtifactsDir = args.artifacts_dir
    bPlot = args.plot
    WriteData(sArtifactsDir, bPlot)


if __name__ == "__main__":
    main()
