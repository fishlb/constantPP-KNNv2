# SecKNN (Scheme [12]) - Evaluation Source Code

This repository contains the implementation of **SecKNN** (Scheme [12]), provided for performance comparison as described in our paper.

---

## 1. Requirements

To run the code, ensure your environment meets the following dependency:
- **Python**: 3.x (Tested with version 3.8.0)

---

## 2. How to Run

The core evaluation logic is implemented in Python. To reproduce the experimental results for varying dataset sizes (`n`) and neighbor values (`k`), you can run the scheme using either a Python IDE or the command line.

### Option A: Via Python IDE
1. Open the **root directory of the current scheme** in your IDE (like VS Code).
2. Locate the main entry file: `secKNN.py`.
3. Simply click the **Run** button provided by your IDE.

### Option B: Via Command Line
Open your terminal, navigate to the **root directory of the current scheme**, and execute the following command:

```bash
python secKNN.py
```

---

## 3. Output & Results

The results of the experiments, including dataset information and query latency metrics, will be automatically generated and saved in the output directory:

- **Output Directory**: `evalResult/`

**Generated Result Files**:
- `Fig.4a_latency_result_scheme[12].txt`: Contains latency results varying by dataset size (`n`).
- `Fig.4b_latency_result_scheme[12].txt`: Contains latency results varying by the number of neighbors (`k`).
