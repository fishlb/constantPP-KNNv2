import os
import re
import pandas as pd

def parse_fig3_results(eval_dir="./evalResult"):
    """
    Parse the Fig.3 Offline Cost results and return DataFrames for Fig.3a and Fig.3b.
    """
    # Regular expressions for matching variables and total offline cost
    k_pattern = re.compile(r"k\s*=\s*(\d+)")
    n_pattern = re.compile(r"#Sample\s*\(n\)\s*:\s*(\d+)")
    total_pattern = re.compile(r"Total offline\s*:\s*([0-9.eE+-]+)\s*s")

    data_3a = []
    data_3b = []

    if not os.path.exists(eval_dir):
        print(f"Directory not found: {eval_dir}")
        return pd.DataFrame(), pd.DataFrame()

    # Iterate through all txt files in the evalResult directory
    for filename in os.listdir(eval_dir):
        if not filename.endswith(".txt"):
            continue

        # Determine whether the data belongs to Fig.3a or Fig.3b
        if "Fig.3a_offline_cost" in filename:
            target_data = data_3a
        elif "Fig.3b_offline_cost" in filename:
            target_data = data_3b
        else:
            continue

        if "parallel" in filename.lower():
            scheme_name = "Offline(Parallel)"
        elif "sequential" in filename.lower():
            scheme_name = "Offline(Sequential)"
        else:
            scheme_name = filename.replace(".txt", "")

        filepath = os.path.join(eval_dir, filename)
        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        current_k = None
        current_n = None

        for line in lines:
            match_k = k_pattern.search(line)
            if match_k:
                current_k = int(match_k.group(1))

            match_n = n_pattern.search(line)
            if match_n:
                current_n = int(match_n.group(1))

            match_total = total_pattern.search(line)
            if match_total and current_k is not None and current_n is not None:
                target_data.append({
                    "Scheme": scheme_name,
                    "n": current_n,
                    "k": current_k,
                    "Total Offline Cost": float(match_total.group(1))
                })

    return pd.DataFrame(data_3a), pd.DataFrame(data_3b)

def format_with_unit(val):
    """Add 's' (seconds) unit to non-null data"""
    if pd.notnull(val):
        return f"{val} s"
    return val

if __name__ == "__main__":
    print(">>> Extracting Fig.3 (Offline Cost) experimental results...\n")
    df_3a, df_3b = parse_fig3_results()

    txt_filename = "Fig3_OfflineCost_Summary.txt"
    
    with open(txt_filename, "w", encoding="utf-8") as f:
        f.write("=== Fig.3 Offline Cost Count ===\n\n")
        
        # Process and output Fig.3a data
        if not df_3a.empty:
            pivot_3a = df_3a.pivot_table(
                index="n", 
                columns="Scheme", 
                values="Total Offline Cost"
            ).sort_index()
            
            # Apply unit formatting to each column
            for col in pivot_3a.columns:
                pivot_3a[col] = pivot_3a[col].apply(format_with_unit)
            
            # print("--- Fig.3a: Varying n (k=5) ---")
            # print(pivot_3a.to_string())
            # print("\n")
            
            f.write("--- Fig.3a: Varying dataset size n (Fixed k=5) ---\n")
            f.write(pivot_3a.to_string() + "\n\n")

        # Process and output Fig.3b data
        if not df_3b.empty:
            pivot_3b = df_3b.pivot_table(
                index="k", 
                columns="Scheme", 
                values="Total Offline Cost"
            ).sort_index()
            
            # Apply unit formatting to each column
            for col in pivot_3b.columns:
                pivot_3b[col] = pivot_3b[col].apply(format_with_unit)
            
            # print("--- Fig.3b: Varying k (n=400) ---")
            # print(pivot_3b.to_string())
            # print("\n")
            
            f.write("--- Fig.3b: Varying neighbor size k (Fixed n=400) ---\n")
            f.write(pivot_3b.to_string() + "\n\n")

    print(f"Data extraction complete! Summary results saved to txt document: {txt_filename}")