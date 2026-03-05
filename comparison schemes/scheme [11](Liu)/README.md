# Scheme [11] Evaluation

This directory contains the evaluation program for Scheme [11], provided for performance comparison as described in our paper. 

> **Important Note on Implementation:**
> Since the original source code for Scheme [11] is not publicly available, we have developed a faithful simulation program to evaluate its computational latency. 
> 
> Specifically, relying on the theoretical complexity analysis presented in their paper, we rigorously deduced the exact number of the most computationally expensive cryptographic operations (such as homomorphic encryptions and decryptions) required by their protocol. This program executes these specific operations the exact required number of times to measure the latency. 
> 
> This approach effectively provides a highly accurate **lower bound** for their execution time, as it deliberately ignores other lightweight overheads. This ensures our comparative evaluation is both rigorous and strictly fair.

---

## 1. Requirements

To run the code, ensure your environment meets the following dependency:
- **Java**: OpenJDK 11 (Tested with version 11.0.19)

---

## 2. How to Run

The core evaluation logic is implemented in Java. You can easily run the scheme by executing the main entry point, `App.java`, through a Java IDE or the command line.

### Option A: Via Java IDE
1. Open the **root directory of the current scheme** (e.g., `scheme [11](Liu)`) in your IDE (like VS Code).
2. Locate the main entry file at `src/test/java/de/henku/jpaillier/App.java`.
3. Simply click the **Run** button provided by your IDE.

### Option B: Via Command Line
Open your terminal, navigate to the **root directory of the current scheme** (e.g., `scheme [11](Liu)`), and execute the following commands:

```bash
# 1. Compile the source code 
javac -sourcepath "src/main/java:src/test/java" -d bin src/test/java/de/henku/jpaillier/App.java

# 2. Run the main program
java -cp bin de.henku.jpaillier.App
```
---

## 3. Output & Results

The results of the experiments, including dataset information and query latency metrics, will be automatically generated and saved in the output directory:

- **Output Directory**: `evalResult/`

**Generated Result Files**:
- `Fig.4a_latency_result_scheme[11].txt`: Contains latency results varying by dataset size (`n`).
- `Fig.4b_latency_result_scheme[11].txt`: Contains latency results varying by the number of neighbors (`k`).
