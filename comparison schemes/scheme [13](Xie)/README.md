# Scheme [13] - Evaluation Source Code

This repository contains the implementation of Scheme [13], provided for performance comparison as described in our paper.

---

## 1. Requirements

To run the code, ensure your environment meets the following dependency:
- **Java**: OpenJDK 11 (Tested with version 11.0.19)

---

## 2. How to Run

The core evaluation logic is implemented in Java. You can easily run the scheme by executing the main entry point, `App.java`, through a Java IDE or the command line.

### Option A: Via Java IDE 
1. Open the **root directory of the current scheme** (e.g., `scheme [13](Xie)`) in your IDE (like VS Code).
2. Locate the `src/App.java` file.
3. Simply click the **Run** button provided by your IDE.

### Option B: Via Command Line
Open your terminal, navigate to the **root directory of the current scheme**, and execute the following commands:

```bash
# 1. Compile the source code 
javac -sourcepath src -d bin src/App.java

# 2. Run the main program
java -cp bin App
```
---

## 3. Output & Results

The results of the experiments, including dataset information and query latency metrics, will be automatically generated and saved in the output directory:

- **Output Directory**: `./evalResult/`

**Generated Result Files**:
- `Fig.4a_latency_result_scheme[13].txt`: Contains latency results varying by dataset size (`n`).
- `Fig.4b_latency_result_scheme[13].txt`: Contains latency results varying by the number of neighbors (`k`).
