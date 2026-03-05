import random
import os

n_list = [100, 200, 300, 400, 500]     # samples,n
m_list = [20, 40, 60, 80]              # features,m
c = 3                                  # labels,c（0 ~ c-1）
value_range = (0, 100)
random_seed = 42  #fixed seed
# ==============================


def generate_random_data(n, m, c):
    """Generate n samples with m features and c labels."""
    data = []
    for _ in range(n):
        features = [random.randint(*value_range) for _ in range(m)]
        label = random.randint(0, c - 1)
        data.append(features + [label])
    return data


def write_data_to_file(data, n, m, out_dir):
    """Write dataset to synthetic/test{n}-{m}.csv"""
    filename = f"test{n}-{m}.csv"
    file_path = os.path.join(out_dir, filename)

    with open(file_path, "w") as f:
        header = [f"f{i+1}" for i in range(m)] + ["label"]
        f.write(",".join(header) + "\n")

        for row in data:
            f.write(",".join(map(str, row)) + "\n")

    return file_path


def main():
    random.seed(random_seed)

    # current dir
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # data dir synthetic/
    out_dir = os.path.join(script_dir, script_dir)
    os.makedirs(out_dir, exist_ok=True)

    for n in n_list:
        for m in m_list:
            data = generate_random_data(n, m, c)
            file_path = write_data_to_file(data, n, m, out_dir)

            print(f"Generated: {file_path}  (n={n}, m={m}, c={c})")


if __name__ == "__main__":
    main()
