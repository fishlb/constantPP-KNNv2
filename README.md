# constantPP-KNN

This repository contains the full source code for the paper:

> **Constant-Round Privacy-Preserving KNN Classification Based on Function Secret Sharing**

This README provides:
- **Build Instructions:** Steps to set up dependencies and compile the project.

- **Experiment Execution:** An interactive menu to run our scheme's benchmarks for Table V, Figure 3, and Figure 4.

- **Data Extraction:** Python scripts to summarize raw results into formatted tables.

---

## 1. Prerequisites

This project extends the underlying Function Secret Sharing (FSS) implementation ([frankw2/libfss](https://github.com/frankw2/libfss)). To ensure successful compilation and reproducibility, your system must meet the following requirements.

**Minimum Requirements:**
- **Build Tool**: CMake >= 3.5
- **Compiler**: GCC 13.x
- **Interpreter**: Python >= 3.6 (Required for running Python scripts)
- **Libraries**:
  - GNU GMP (Tested with >= 6.1.2)
  - OpenSSL (`libssl-dev`)
  - OpenMP (Enabled for parallel execution)
  - EMP-toolkit (`emp-tool` used for accurate timing)

**Tested Benchmark Environment:**
The experiments in our paper were conducted on:
- **OS**: Ubuntu 18.04 LTS
- **Build Tool**: CMake 3.28.3
- **Compiler**: GCC 13.1.0 
- **Interpreter**: Python 3.8.0

---

## 2. Environment Setup

This project is developed and tested on Ubuntu. The core cryptographic protocols are implemented in C++, while Python scripts are used for extracting and formatting the experimental results. Please follow the steps below to configure your environment to ensure reproducibility.

### 2.1. Development Libraries & Compiler (GCC 13)

First, install the essential build tools and the libraries required for the project:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libgmp-dev libgmpxx4ldbl libssl-dev
```

To ensure full compatibility with modern C++ features and avoid compilation errors, **we strongly recommend using GCC 13. Install and configure it as your system's default compiler**:

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-13 g++-13
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

# Verify the installation (should output 13.x.x)
gcc --version
g++ --version
```

### 2.2. Benchmarking Utilities (emp-tool)

Our implementation relies on `emp-tool` for accurate execution timing. Clone and build the repository as follows:

```bash
# Clone the emp-tool repository
sudo apt install git
git clone https://github.com/emp-toolkit/emp-tool.git
cd emp-tool

# Build and install
mkdir build && cd build
cmake ..
make
sudo make install
```

### 2.3. Python Environment (Python >= 3.6)
The result extraction scripts (e.g., fig3_offline_cost_summary.py) require Python 3.6 or higher.

```bash
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt update
sudo apt install python3.8

#Set Python 3.8.x to a higher priority in the system
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.8 2

# Verify the installation (should output 3.8.x)
python --version 

# Install required packages (numpy and pandas)
sudo apt update
sudo apt install -y python3.8-distutils python3.8-dev
wget https://bootstrap.pypa.io/pip/3.8/get-pip.py -O get-pip-3.8.py
python get-pip-3.8.py
python -m pip install pandas numpy

```

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
mkdir build && cd build
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
| 2 | Table V – Ours |
| 3 | Fig. 3a – Offline (Sequential) |
| 4 | Fig. 3a – Offline (Parallel) |
| 5 | Fig. 3b – Offline (Sequential) |
| 6 | Fig. 3b – Offline (Parallel) |
| 7 | Fig. 4a – Latency (Sequential) |
| 8 | Fig. 4a – Latency (Parallel) |
| 9 | Fig. 4b – Latency (Sequential) |
| 10 | Fig. 4b – Latency (Parallel) |

Experiments can be run individually by selecting the respective number.

---

## 5. Reproducing Table V, Figures 3 and 4

For each experiment you run, the program will:
- Print the accuracy or timing results directly to standard output (stdout).
- Save the results into the `evalResult/` directory (and the respective `comparison schemes/` directories for comparison schemes).

Each plot and table from the paper has the corresponding raw `.txt` file generated in these folders.

> **Important Note on Comparison Schemes:** To reproduce the latency results for the comparison schemes (Scheme [11], Scheme [12], and Scheme [13] shown in Figure 4), you must explicitly navigate into their respective folders under the `comparison schemes/` directory and execute them individually. Please follow the specific execution instructions provided in each scheme's local `README.md`.


### 5.1. Summarizing the Results
To easily verify the data against the paper, we provide Python scripts that automatically parse the results across all directories and generate formatted summary tables.

You can extract and summarize the results using either the automated bash script or by running the Python scripts individually.

### Option A: Automated Summary (Recommended)
Run the provided bash script to extract and format all results: (**Require Python >= 3.6**)

```bash
chmod +x run_results_summary.sh
./run_results_summary.sh
```

### Option B: Manual Summary
Alternatively, you can generate the summaries individually based on what you want to verify: (**Require Python >= 3.6**)

```bash
# To reproduce Table V (Accuracy Comparison)
python tableV_accuracy_summary.py
(or: python3 tableV_accuracy_summary.py)

# To reproduce Figure 3 (Offline Cost Comparison)
python fig3_offline_cost_summary.py
(or: python3 fig3_offline_cost_summary.py)

# To reproduce Figure 4 (Query Latency Comparison)
python fig4_latency_result_summary.py
(or: python3 fig4_latency_result_summary.py)
```

### 5.2. Final Output
The summary scripts will generate the following formatted `.txt` files in the project's root directory:
- `TableV_Accuracy_Summary.txt`
- `Fig3_OfflineCost_Summary.txt`
- `Fig4_LatencyResult_Summary.txt`
