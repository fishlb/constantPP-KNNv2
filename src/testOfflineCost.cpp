#include "../include/secknn.h"
#include <thread>
#include <algorithm>
#include <omp.h>
#include <iomanip>
/*
 *test offline cost
 */
void cal_cost(
    size_t n,
    size_t m,
    size_t k,
    ofstream &fout,
    int is_parallel)
{
    // cout << "n = " << n << "; m = " << m << endl;

    /* ===== 1. Init (not counted) ===== */
    Fss fClient, fServer;
    initializeClient(&fClient, NUM_BIT, NUM_PARTIES);
    initializeServer(&fServer, &fClient);

    /* ===== 2. Offline call counts ===== */
    size_t num_triples = m * n + k;
    size_t num_dcf_calls = n * n + k * k;
    size_t num_dpf_calls = k * k + k;
    size_t num_rand_calls = 2 * n + 2 * k + k * k;

    double T_triple = 0.0, T_dcf = 0.0, T_dpf = 0.0, T_rand = 0.0;

    /* ===== Beaver triples ===== */
    {
        auto t0 = clock_start();
        for (size_t i = 0; i < num_triples; i++)
        {
            auto triple = generate_beaver_triple(NUM_PARTIES, DEFAULT_MOD);
            (void)triple;
        }
        T_triple = sec_from(t0);
    }

    /* ===== DCF / DPF ===== */
    if (is_parallel == 0)
    {
        /* --- DCF (single-thread) --- */
        {
            ServerKeyLt k0_lt, k1_lt;
            auto t0 = clock_start();
            for (size_t i = 0; i < num_dcf_calls; i++)
            {

                generateTreeLt(&fServer, &k0_lt, &k1_lt, 1, 1);
            }
            T_dcf = sec_from(t0);
        }

        /* --- DPF (single-thread) --- */
        {
            ServerKeyEq k0_eq, k1_eq;
            auto t0 = clock_start();
            for (size_t i = 0; i < num_dpf_calls; i++)
            {

                generateTreeEq(&fServer, &k0_eq, &k1_eq, 1, 1);
            }
            T_dpf = sec_from(t0);
        }
    }
    else
    {
        // int n_threads = omp_get_max_threads();
        const unsigned n_threads =
            std::max(1u, std::min(6u, std::thread::hardware_concurrency()));
        // cout << "#thread: " << n_threads << endl;

        // Pre-allocate server pool to avoid race conditions
        std::vector<Fss> server_pool(n_threads);
        for (int i = 0; i < n_threads; i++)
        {
            initializeServer(&server_pool[i], &fClient);
        }

        /* --- DCF (multi-thread) --- */
        double start_dcf = omp_get_wtime();
#pragma omp parallel num_threads(n_threads)
        {
            int tid = omp_get_thread_num();
#pragma omp for schedule(static)
            for (size_t i = 0; i < num_dcf_calls; i++)
            {
                ServerKeyLt k0, k1;
                generateTreeLt(&server_pool[tid], &k0, &k1, 1, 1);
            }
        }
        T_dcf = omp_get_wtime() - start_dcf;
        printf("Parallel DCF Time: %f s\n", T_dcf);

        /* --- DPF (multi-thread) --- */
        double start_dpf = omp_get_wtime();
#pragma omp parallel num_threads(n_threads)
        {
            int tid = omp_get_thread_num();
#pragma omp for schedule(static)
            for (size_t i = 0; i < num_dpf_calls; i++)
            {
                ServerKeyEq k0e, k1e;
                generateTreeEq(&server_pool[tid], &k0e, &k1e, 1, 1);
            }
        }
        T_dpf = omp_get_wtime() - start_dpf;
        printf("Parallel DPF Time: %f s\n", T_dpf);
    }

    /* ===== Random masks ===== */
    {
        auto t0 = clock_start();
        for (size_t i = 0; i < num_rand_calls; i++)
        {
            auto s = additive_secret_sharing((uint64_t)i, NUM_PARTIES);
            (void)s;
        }
        T_rand = sec_from(t0);
    }

    double T_total = T_triple + T_dcf + T_dpf + T_rand;

    /* ===== Save ===== */
    fout << "Offline time breakdown (seconds):\n";
    fout << "  Beaver triples : " << T_triple << "\n";
    fout << "  DCF keys       : " << T_dcf << "\n";
    fout << "  DPF keys       : " << T_dpf << "\n";
    fout << "  Random masks   : " << T_rand << "\n";
    fout << "  --------------------------------\n";
    fout << fixed << setprecision(3) << "  Total offline  : " << T_total << " s\n";

    cout << fixed << setprecision(3) << "Total offline : " << T_total << " s\n";
    // fout << "  Total offline  : " << T_total << " s\n";
    // cout << "Total offline : " << T_total << " s\n";
}

int test_offline_cost(int test_flag, int is_parallel)
{
    cout << "=============================================\n";
    cout << "===== Offline Cost Evaluation =====\n";
    string data_dir = "../dataset/synthetic/";
    string output_dir = "../evalResult/";
    string out_file;

    vector<string> datasets;
    vector<int> k_values;

    string parallel_flag = "";
    if (is_parallel == 0)
    {
        parallel_flag = "sequential";
    }
    else
    {
        parallel_flag = "parallel";
    }

    if (test_flag == 0)
    {
        // fixed k，different n
        k_values = {5};
        datasets = {
            "test100-60.csv",
            "test200-60.csv",
            "test300-60.csv",
            "test400-60.csv",
            "test500-60.csv",
        };
        out_file = output_dir + "Fig.3a_offline_cost(" + parallel_flag + ").txt";
    }
    else
    {
        // fixed n, different k
        k_values = {5, 10, 15, 20, 25};
        datasets = {
            "test400-60.csv",
        };
        out_file = output_dir + "Fig.3b_offline_cost(" + parallel_flag + ").txt";
    }

    ofstream fout(out_file, ios::trunc);
    if (!fout.is_open())
    {
        cerr << "Cannot open " << out_file << "\n";
        return 1;
    }

    fout << "===== Offline Cost Evaluation =====\n";

    /* ===== loop ===== */
    for (int k : k_values)
    {
        fout << "k = " << k << "\n";

        cout << "\n================ k = " << k << " ================\n";

        for (const auto &file_name : datasets)
        {
            string dataset_path = data_dir + file_name;

            cout << "Dataset: " << file_name << endl;

            Dataset data = load_mapped_dataset(dataset_path);

            /* ---- print & save dataset info ---- */
            print_dataset_info(file_name, data);
            print_dataset_info_to_file(file_name, data, fout);

            size_t n = data.info.num_samples;
            size_t m = data.info.num_features;

            if (n == 0 || (size_t)k >= n)
            {
                fout << "Invalid n or k, skip.\n\n";
                continue;
            }

            /* ---- offline test ---- */
            cal_cost(n, m, k, fout, is_parallel);
        }
    }

    fout << "\n===== Finished =====\n";
    fout.close();

    cout << "\n=============================================\n";
    cout << "Offline cost evaluation finished.\n";
    cout << "Saved to " << out_file << endl;

    return 0;
}
