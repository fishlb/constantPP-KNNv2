import random
import time
import os  # Used for handling absolute paths
from dataprocessing import gen_additive_shares, generate_beaver_triple, read_csv_dataset, share_dataset
from FSS.dcf import keygenDCF, evalDCF
from FSS.dpf import keygenDPF, evalDPF
from Common.group_elements import GroupElements
from Communication.semi_honest_party import SemiHonestParty
import Configs.fss as config

# Configuration constants
MODULUS = 2**64
NUM_PARTIES = 2
LAMBDA = 3  # Corresponds to the number of labels (c)
SEC_PARA = config.sec_para

# Initialize Function Secret Sharing (FSS) components
alpha_zero = GroupElements(0)
party0 = SemiHonestParty(party_id=0, partner_addr='127.0.0.1', partner_recv_port=44000, recv_port=43000)
party1 = SemiHonestParty(party_id=1, partner_addr='127.0.0.1', partner_recv_port=43000, recv_port=44000)
k0_dcf, k1_dcf = keygenDCF(alpha_zero, party1, None, None, SEC_PARA, True, config.DEBUG)
k0_dpf, k1_dpf = keygenDPF(alpha_zero, party0, None, SEC_PARA, None, True, config.DEBUG)

# --- Secure MPC Protocols ---

def Mul(beaver_triple, x_shares, y_shares):
    """Secure Multiplication using Beaver Triples"""
    shares_a, shares_b, shares_c = beaver_triple
    e0 = (x_shares[0] - shares_a[0]) % MODULUS
    e1 = (x_shares[1] - shares_a[1]) % MODULUS
    f0 = (y_shares[0] - shares_b[0]) % MODULUS
    f1 = (y_shares[1] - shares_b[1]) % MODULUS
    e = (e0 + e1) % MODULUS
    f = (f0 + f1) % MODULUS
    xy0 = (e * f + shares_b[0] * e + shares_a[0] * f + shares_c[0]) % MODULUS
    xy1 = (shares_b[1] * e + shares_a[1] * f + shares_c[1]) % MODULUS
    return [xy0, xy1]

def SD(beaver_triple, x_shares_list, y_shares_list):
    """Secure Euclidean Distance Computation"""
    DA, DB = 0, 0 
    for i in range(len(x_shares_list)):
        ziA = (x_shares_list[i][0] - y_shares_list[0]) % MODULUS
        ziB = (x_shares_list[i][1] - y_shares_list[1]) % MODULUS
        ZiA_tmp, ZiB_tmp = Mul(beaver_triple, [ziA, ziB], [ziA, ziB])
        DA += ZiA_tmp
        DB += ZiB_tmp
    return DA, DB

def SC_DCF(beaver_triple, x_shares, y_shares):
    """Secure Comparison using Distributed Comparison Function (DCF)"""
    eval_val = GroupElements((x_shares[0] + x_shares[1]) - (y_shares[0] + y_shares[1]))
    uA = evalDCF(party0, eval_val, k0_dcf, None, SEC_PARA, config.DEBUG)
    uB = evalDCF(party1, eval_val, k1_dcf, None, SEC_PARA, config.DEBUG)
    return uA, uB

def SS(beaver_triple, x1_shares, x2_shares, t1_shares, t2_shares):
    """Secure Selection Primitive: Swaps values based on secure comparison"""
    u = SC_DCF(beaver_triple, x1_shares, x2_shares)
    y1 = Mul(beaver_triple, u, x1_shares)
    y2 = Mul(beaver_triple, u, x2_shares)
    z1 = Mul(beaver_triple, u, t1_shares)
    z2 = Mul(beaver_triple, u, t2_shares)
    x_min = [(x1_shares[0] - y1[0] + y2[0]) % MODULUS, (x1_shares[1] - y1[1] + y2[1]) % MODULUS]
    t_min = [(t1_shares[0] - z1[0] + z2[0]) % MODULUS, (t1_shares[1] - z1[1] + z2[1]) % MODULUS]
    x_max = [(y1[0] + x2_shares[0] - y2[0]) % MODULUS, (y1[1] + x2_shares[1] - y2[1]) % MODULUS]
    t_max = [(z1[0] + t2_shares[0] - z2[0]) % MODULUS, (z1[1] + t2_shares[1] - z2[1]) % MODULUS]
    return [x_min, t_min, x_max, t_max]

def SMin(beaver_triple, a_shares_list, t_shares_list):
    """Finds the secure minimum of n shared numbers"""
    res = SS(beaver_triple, a_shares_list[0], a_shares_list[1], t_shares_list[0], t_shares_list[1])
    for i in range(2, len(a_shares_list)):
        res = SS(beaver_triple, res[0], res[1], a_shares_list[i], t_shares_list[i])
    return res[1]

def SMax(beaver_triple, a_shares_list, t_shares_list):
    """Finds the secure maximum of n shared numbers"""
    res = SS(beaver_triple, a_shares_list[0], a_shares_list[1], t_shares_list[0], t_shares_list[1])
    for i in range(2, len(a_shares_list)):
        res = SS(beaver_triple, res[2], res[3], a_shares_list[i], t_shares_list[i])
    return res[3]

def SF_DPF(beaver_triple, a_shares_list):
    """Secure Frequency counting using Distributed Point Function (DPF)"""
    fA, fB = [], []
    n = len(a_shares_list)
    for i in range(n):
        f_xi_0, f_xi_1 = 0, 0
        for j in range(n):
            diff = (a_shares_list[i][0] + a_shares_list[i][1]) - (a_shares_list[j][0] + a_shares_list[j][1])
            eval_val = GroupElements(diff)
            f_xi_0 += evalDPF(party0, eval_val, k0_dpf, None, False, 1, SEC_PARA, True, False, config.DEBUG)
            f_xi_1 += evalDPF(party1, eval_val, k1_dpf, None, False, 1, SEC_PARA, True, False, config.DEBUG)
        fA.append(f_xi_0)
        fB.append(f_xi_1)
    return [fA, fB]

# --- Experiment Utilities ---

def print_experiment_header(k_val):
    return f"===== Query Latency Test (k = {k_val}) =====\n"

def print_dataset_info(name, n, m, c, latency):
    info = (
        f"================*******==================\n"
        f"Dataset         : {name}\n"
        f"#Sample (n)     : {n}\n"
        f"#Feature(m)     : {m}\n"
        f"#Label(c)       : {c}\n"
        f"Latency (1 query) = {latency:.3f} s\n"
    )
    return info

def run_experiment(file_path, k_val, beaver_triple):
    """Executes the secure KNN process and records latency"""
    features, labels = read_csv_dataset(file_path)
    shared_features, shared_labels = share_dataset(features, labels)
    
    n, m = len(shared_features), len(shared_features[0])
    c = LAMBDA
    
    start_time = time.time()
    
    # 1. Compute secure distances
    q = gen_additive_shares(1, NUM_PARTIES)
    d_shares = [SD(beaver_triple, shared_features[i], q) for i in range(n)]
    
    # 2. Extract k nearest neighbors
    tmp_d, tmp_l = list(d_shares), list(shared_labels)
    k_labels = []
    for _ in range(k_val):
        _ = SMin(beaver_triple, tmp_d, tmp_l)
        tmp_d.pop()
        k_labels.append(tmp_l.pop())
        
    # 3. Secure classification
    f_xi_shares = SF_DPF(beaver_triple, k_labels)
    _ = SMax(beaver_triple, f_xi_shares, shared_labels[:len(f_xi_shares[0])])
    
    end_time = time.time()
    latency = end_time - start_time
    
    # Generate log output
    log_content = print_dataset_info(file_path.split(os.sep)[-1], n, m, c, latency)
    print(log_content)
    return log_content


if __name__ == "__main__":
    beaver_triple = generate_beaver_triple(NUM_PARTIES)
    
    # Determine the directory where this script is located
    current_script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Set up results directory
    eval_result_dir = os.path.join(current_script_dir, "evalResult")
    if not os.path.exists(eval_result_dir):
        os.makedirs(eval_result_dir)

    # --- Test 1: Test different dataset sizes (n) with fixed k = 5 ---
    dataset_files_n = [
        'test100-60.csv', 'test200-60.csv', 'test300-60.csv', 
        'test400-60.csv', 'test500-60.csv'
    ]
    output_path_n = os.path.join(eval_result_dir, "Fig.4a_latency_result_scheme [12].txt")
    
    print(print_experiment_header(5))
    with open(output_path_n, "w") as f_n:
        f_n.write(print_experiment_header(5))
        for filename in dataset_files_n:
            full_path = os.path.join(current_script_dir, 'dataset', filename)
            if os.path.exists(full_path):
                res = run_experiment(full_path, 5, beaver_triple)
                f_n.write(res)
            else:
                print(f"Error: Dataset not found at {full_path}")

    # --- Test 2: Test different k values with a fixed dataset ---
    k_values = [5, 10, 15, 20, 25]
    fixed_target_dataset = os.path.join(current_script_dir, 'dataset', 'test400-60.csv')
    output_path_k = os.path.join(eval_result_dir, "Fig.4b_latency_result_scheme [12].txt")
    
    with open(output_path_k, "w") as f_k:
        for kv in k_values:
            header = print_experiment_header(kv)
            print(header)
            f_k.write(header)
            if os.path.exists(fixed_target_dataset):
                res = run_experiment(fixed_target_dataset, kv, beaver_triple)
                f_k.write(res)
            else:
                print(f"Error: Dataset not found at {fixed_target_dataset}")
                
    print("\nExperiments completed successfully.")
    print(f"Results saved to: \n1. {output_path_n}\n2. {output_path_k}")