import os
import re
import pandas as pd

def parse_fig4_results():
    """
    Parse Fig.4 Latency results line-by-line. 
    Maintains 'k' as a persistent state to ensure all entries in a block are captured.
    """
    directories = [
        "./evalResult",
        "./comparison schemes/scheme [11](Liu)/evalResult",
        "./comparison schemes/scheme [12](Li)/evalResult",
        "./comparison schemes/scheme [13](Xie)/evalResult"
    ]

    # Case-insensitive patterns for maximum compatibility
    k_pattern = re.compile(r"k\s*=\s*(\d+)", re.IGNORECASE)
    n_pattern = re.compile(r"#Sample\s*\(n\)\s*:\s*(\d+)", re.IGNORECASE)
    # Flexible latency pattern to catch different labeling styles
    latency_pattern = re.compile(r"Latency.*?=\s*([0-9.]+)", re.IGNORECASE)

    data_4a = []
    data_4b = []

    for folder in directories:
        if not os.path.exists(folder):
            print(f"Warning: Directory not found -> {folder}, skipping.")
            continue
            
        for filename in os.listdir(folder):
            if not filename.endswith(".txt"):
                continue

            if "Fig.4a_latency" in filename:
                target_data = data_4a
            elif "Fig.4b_latency" in filename:
                target_data = data_4b
            else:
                continue

            # Identify scheme names based on filenames
            name_lower = filename.lower()
            if "ours(parallel)" in name_lower:
                scheme_name = "Ours(Parallel)"
            elif "ours(sequential)" in name_lower:
                scheme_name = "Ours(Sequential)"
            elif "scheme [11]" in name_lower:
                scheme_name = "Scheme [11]"
            elif "scheme [12]" in name_lower:
                scheme_name = "Scheme [12]"
            elif "scheme [13]" in name_lower:
                scheme_name = "Scheme [13]"
            else:
                scheme_name = filename.replace(".txt", "")

            filepath = os.path.join(folder, filename)
            with open(filepath, 'r', encoding='utf-8') as f:
                lines = f.readlines()

            # Persistent state variables per file
            current_k = None
            current_n = None

            for line in lines:
                # Update current_k state
                mk = k_pattern.search(line)
                if mk:
                    current_k = int(mk.group(1))
                    
                # Update current_n state
                mn = n_pattern.search(line)
                if mn:
                    current_n = int(mn.group(1))
                    
                # Once latency is found, record it using current persistent states
                ml = latency_pattern.search(line)
                if ml:
                    latency_val = float(ml.group(1))
                    
                    # For Fig.4a, k is usually fixed at 5 if not specified
                    k_val = current_k if current_k is not None else 5
                    
                    if current_n is not None:
                        target_data.append({
                            "Scheme": scheme_name,
                            "n": current_n,
                            "k": k_val,
                            "Latency": latency_val
                        })
                        # Optional: Reset current_n to prevent duplicate recording 
                        # if the same latency appears twice unexpectedly
                        current_n = None

    return pd.DataFrame(data_4a), pd.DataFrame(data_4b)

def format_with_unit(val):
    """Formats values with 's' unit for display."""
    if pd.notnull(val):
        return f"{val} s"
    return val

if __name__ == "__main__":
    print(">>>Extracting Fig.4 (Latency Comparison) experimental results...\n")
    df_4a, df_4b = parse_fig4_results()

    summary_file = "Fig4_LatencyResult_Summary.txt"
    with open(summary_file, "w", encoding="utf-8") as f:
        f.write("=== Fig.4 Query Latency Comparison ===\n\n")
        
        # Fig.4a Summary
        if not df_4a.empty:
            p4a = df_4a.pivot_table(index="n", columns="Scheme", values="Latency").sort_index()
            for col in p4a.columns:
                p4a[col] = p4a[col].apply(format_with_unit)
            
            f.write("--- Fig.4a: Varying dataset size n (Fixed k=5) ---\n")
            f.write(p4a.to_string() + "\n\n")

        # Fig.4b Summary
        if not df_4b.empty:
            # Important: We sort the index (k) to ensure 5, 10, 15, 20, 25 order
            p4b = df_4b.pivot_table(index="k", columns="Scheme", values="Latency").sort_index()
            for col in p4b.columns:
                p4b[col] = p4b[col].apply(format_with_unit)
            
            f.write("--- Fig.4b: Varying neighbor size k (Fixed n=400) ---\n")
            f.write(p4b.to_string() + "\n")

    print(f"Data extraction complete! Summary results saved to txt document: {summary_file}")