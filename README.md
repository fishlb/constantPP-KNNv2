# constantPP-KNN

This repository contains the full source code for the paper:

> **Constant-Round Privacy-Preserving KNN Classification Based on Function Secret Sharing**

This README provides:
- Complete build instructions
- Exact steps to reproduce Table V, Figure 3, and Figure 4
- Access to raw experimental results used in the plots

---

## 1. Environment Configuration

The environment requirements for this project are highly flexible. The primary compatibility boundary is defined by the underlying Function Secret Sharing (FSS) core implementation ([frankw2/libfss](https://github.com/frankw2/libfss)). 

**Minimum Requirements:**
- **Build Tool**: CMake >= 3.5
- **Compiler**: Any standard C/C++ compiler compatible with `libfss` (e.g., GCC).
- **Libraries**:
  - GNU GMP (Tested with >= 6.1.2)
  - OpenSSL (`libssl-dev`)
  - OpenMP (Enabled for parallel execution)
  - EMP-toolkit (`emp-tool` for timing)
**Tested Benchmark Environment:**
The experiments in our paper were conducted on:
- **OS**: Ubuntu 18.04 LTS
- **Compiler**: GCC 13.1.0 
- **Build Tool**: CMake 3.28.3

---

## 2. Install Dependencies

You can install the required dependencies using the following commands:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libgmp-dev libgmpxx4ldbl libssl-dev

# Clone the emp-tool repository
git clone https://github.com/emp-toolkit/emp-tool.git
cd emp-tool
# Build and install
mkdir build && cd build
cmake ..
make
sudo make install
```
---

## 3. Build Instructions

We provide two ways to build and run the project: using an automated script (Recommended) or compiling manually via CMake.

### Option A: Automated Script (Recommended)
The easiest way to build the project and launch the interactive testing menu is to use the provided bash script:

```bash
git clone https://github.com/fishlb/constantPP-KNN.git
cd constantPP-KNN
chmod +x run_ours.sh
./run_ours.sh
```

### Option B: Manual Build via CMake
Alternatively, you can manually build the project step-by-step:

```bash
git clone https://github.com/fishlb/constantPP-KNN.git
cd constantPP-KNN
mkdir build
cd build
cmake ..
make
./main
```

---

## 4. Running Experiments

After launching the executable via `./run_ours.sh` or `./main`, an interactive console will appear.

Each menu option corresponds directly to the evaluation results presented in the paper:

| Menu Option | Corresponds To |
|-------------|----------------|
| 1 | Table V – Plaintext baseline |
| 2 | Table V – Ours (Sequential) |
| 3 | Table V – Ours (Parallel) |
| 4 | Fig. 3a – Offline (Sequential) |
| 5 | Fig. 3a – Offline (Parallel) |
| 6 | Fig. 3b – Offline (Sequential) |
| 7 | Fig. 3b – Offline (Parallel) |
| 8 | Fig. 4a – Latency (Sequential) |
| 9 | Fig. 4a – Latency (Parallel) |
| 10 | Fig. 4b – Latency (Sequential) |
| 11 | Fig. 4b – Latency (Parallel) |

Experiments can be run individually by selecting the respective number.

---

## 5. Reproducing Table V, Figures 3 and 4

For each experiment you run, the program will:
- Print the accuracy and timing results directly to standard output (stdout).
- Save the results into the `evalResult/` directory (and the respective `comparison schemes/` directories for comparison schemes).

Each plot and table from the paper has the corresponding raw `.txt` file generated in these folders.

> **Important Note on Comparison Schemes:** To reproduce the latency results for the comparison schemes (Scheme [11], Scheme [12], and Scheme [13] shown in Figure 4), you must explicitly navigate into their respective folders under the `comparison schemes/` directory and execute them individually. Please follow the specific execution instructions provided in each scheme's local `README.md`.


### Summarizing the Results
To easily verify the data against the paper, we provide Python scripts that automatically parse the results across all directories and generate formatted summary tables.

You can extract and summarize the results using either the automated bash script or by running the Python scripts individually.

**Option A: Automated Summary (Recommended)**
Run the provided bash script to extract and format all results:

```bash
chmod +x run_results_summary.sh
./run_results_summary.sh
```

**Option B: Manual Summary**
Alternatively, you can generate the summaries individually based on what you want to verify:

```bash
# To reproduce Table V (Accuracy Comparison)
python tableV_accuracy_summary.py

# To reproduce Figure 3 (Offline Cost Comparison)
python fig3_offline_cost_summary.py

# To reproduce Figure 4 (Query Latency Comparison)
python fig4_latency_result_summary.py
```

### Final Output
After running the summary scripts, the formatted tables will be printed to the console and saved in the root directory as:
- `TableV_Accuracy_Summary.txt`
- `Fig3_OfflineCost_Summary.txt`
- `Fig4_LatencyResult_Summary.txt`
