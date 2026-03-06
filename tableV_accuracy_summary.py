import os
import re
import pandas as pd

def parse_table_v_results(eval_dir="./evalResult"):
    """
    Parse the Table V Accuracy results and return a DataFrame
    along with the original dataset order.
    """
    # Regular expressions to match various metrics
    dataset_pattern = re.compile(r"Dataset\s*:\s*([^\n]+)")
    n_pattern = re.compile(r"#Sample\s*\(n\)\s*:\s*(\d+)")
    m_pattern = re.compile(r"#Feature\s*\(m\)\s*:\s*(\d+)")
    c_pattern = re.compile(r"#Label\s*\(c\)\s*:\s*(\d+)")
    accuracy_pattern = re.compile(r"Accuracy\s*=\s*([0-9.]+)")

    data = []
    dataset_order = []  # To record the original order of datasets

    if not os.path.exists(eval_dir):
        print(f"Directory not found: {eval_dir}")
        return pd.DataFrame(), []

    # Iterate through all txt files in the evalResult directory
    for filename in os.listdir(eval_dir):
        # Only process files related to Table V
        if not (filename.startswith("tableV_") and filename.endswith(".txt")):
            continue
        
        # Extract the scheme name from the filename
        scheme_name = filename.replace("tableV_accuracy_result_", "").replace(".txt", "").strip()

        filepath = os.path.join(eval_dir, filename)
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        # Split the file into independent data blocks based on the separator
        blocks = content.split("================*******==================")
        
        for block in blocks:
            match_dataset = dataset_pattern.search(block)
            match_n = n_pattern.search(block)
            match_m = m_pattern.search(block)
            match_c = c_pattern.search(block)
            match_acc = accuracy_pattern.search(block)

            # If both Dataset and Accuracy are matched, it is a valid data block
            if match_dataset and match_acc:
                dataset_name = match_dataset.group(1).strip()
                
                # Automatically remove the .csv suffix to keep the table clean
                if dataset_name.lower().endswith('.csv'):
                    dataset_name = dataset_name[:-4]
                
                # Record the original order (preventing duplicates)
                if dataset_name not in dataset_order:
                    dataset_order.append(dataset_name)
                
                data.append({
                    "Scheme": scheme_name,
                    "Dataset": dataset_name,
                    "n": int(match_n.group(1)) if match_n else None,
                    "m": int(match_m.group(1)) if match_m else None,
                    "c": int(match_c.group(1)) if match_c else None,
                    "Accuracy": float(match_acc.group(1))
                })
    
    return pd.DataFrame(data), dataset_order

if __name__ == "__main__":
    print(">>> Extracting Table V (Accuracy) experimental results...\n")
    df_results, dataset_order = parse_table_v_results()

    if not df_results.empty:
        pd.set_option('display.max_columns', None)
        pd.set_option('display.width', 10000) 

        df_results['Dataset'] = pd.Categorical(df_results['Dataset'], categories=dataset_order, ordered=True)


        pivot_df = df_results.pivot_table(
            index=["Dataset", "n", "m", "c"], 
            columns="Scheme", 
            values="Accuracy"
        )
        
        pivot_df = pivot_df.dropna(how='all')
        
        pivot_df = pivot_df.fillna('-')
        
        txt_filename = "TableV_Accuracy_Summary.txt"
        with open(txt_filename, "w", encoding="utf-8") as f:
            f.write("=== Table V - Accuracy Comparison ===\n\n")
            f.write(pivot_df.to_string())
            
        print(f"Data extraction complete! Summary results saved to txt document: {txt_filename}")
        
    else:
        print("Failed to extract any valid data.")