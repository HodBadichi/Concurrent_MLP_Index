
# Concurrent MLP Range datastructure
### Abstract

_"In this paper, we introduce an optimistic concurrent and
memory-efficient hash trie structure designed to support the
storage of ranges, leveraging the principles of memory level
parallelism (MLP) and derived from the MLP Index [6]. To
evaluate its performance, we conduct a comparative bench-
mark against Linux’s Maple tree [4], a widely-used data struc-
ture. Our findings indicate that our trie implementation per-
forms on par with the Maple tree on lookup operations, but
lacks on insertions. However we assume that this discrepancy
in insertions is primarily due to the fact that our structure is
not yet fully optimized"_

![image](https://github.com/user-attachments/assets/101ec07e-55e0-4219-a60f-02a1ced92586)



The paper pdf can be found at [this link][proj_pdf]

[proj_pdf]: https://www.overleaf.com/read/qwhwpczxqvrc


## Building and Running Experiments

To replicate the experiments, we provide simple bash-python automation scripts. The experiments were successfully conducted on `mass@rack-mad-10r.cs.tau.ac.il`.
For simplicity the provided automations are suited only for `mass@rack-mad-10r.cs.tau.ac.il` and are not configurable, as it relays on the number of cores at each NUMA node (14 in our case).
The configured compilation flags can be seen in the main `CMakeLists.txt` file : "-O3" "-march=native" "-mavx2" "-DNDEBUG" "pthreads"

### Concurrent MLP Index vs BaseLine Mlp Index Experiments

To run the experiments, execute the following command:
`./concurrent_mlpds/Code/Part1_concurrent_mlp_index/src/Automation/part1Exp.sh`
The results will be saved under `/concurrent_mlpds/Code/Part1_concurrent_mlp_index/Results` in designated CSV files under the folder
`"BenchMarkResults_<time_stamp>"`

To plot the results in a Python supporting environment with `pandas` and `matplotlib` packages, use the following command:

`python 3 concurrent_mlpds/Code/Part1_concurrent_mlp_index/src/Automation/BenchMarksFlow --artifacts_dir [dir_name] --threads_num 28 --plot True`

The figures will also be saved in the designated folder.


### Concurrent MLP index vs Maple Tree Experiments

To run the experiments, execute the following command:
`./concurrent_mlpds/Code/Part4_Experiments_maple_mlp/Automation/part4Exp.sh`
The results will be saved under `/concurrent_mlpds/Code/Part4_Experiments_maple_mlp/Results` in designated CSV files under the folder
`"BenchMarkResults_<time_stamp>"`

To plot the results in a Python supporting environment with `pandas` and `matplotlib` packages, use the following command:

`python 3 concurrent_mlpds/Code/Part4_Experiments_maple_mlp/Automation/BenchMarksFlow --artifacts_dir [dir_name] --plot True`

The figures will also be saved in the designated folder.
