#include "../include/secknn.h"
#include <iomanip>
/*
*test latency for one query
*/
int testLaten(int k1, vector<int> k2, int test_flag, int is_parallel, const int num_threads)
{
    cout << "=============================================\n";
    string fig_flag = "";
    if (test_flag == 0)
    {
        fig_flag = "Fig.4a";
    }
    else
    {
        fig_flag = "Fig.4b";
    }

    string parallel_flag = "";
    if (is_parallel == 0)
    {
        parallel_flag = "sequential";
    }
    else
    {
        parallel_flag = "parallel";
    }

    cout << "=====" + fig_flag + " Query Latency Test Ours(" + parallel_flag + ") =====\n";

    /* ===== data and output dir ===== */
    string data_dir = "../dataset/synthetic/";
    string output_dir = "../evalResult/";
    string out_file;
    vector<string> datasets;
    /* ===== k values ===== */
    vector<int> k_values;

    if (test_flag == 0)
    {
        k_values = {k1};
        datasets = {
            // Fig 4a, for different n, k = 5,m = 60, c = 3
            "test100-60.csv",
            "test200-60.csv",
            "test300-60.csv",
            "test400-60.csv",
            "test500-60.csv",
        };
        out_file = output_dir + "Fig.4a_latency_result_ours(" + parallel_flag + ").txt";
    }
    else if (test_flag == 1)
    {
        // Fig 4b, for different k, n = 600, m = 60, c = 3
        k_values = k2;
        datasets = {
            "test400-60.csv",
        };
        out_file = output_dir + "Fig.4b_latency_result_ours(" + parallel_flag + ").txt";
    }

    /* ===== Init FSS / keys / triples ===== */
    Fss fClient, fServer;
    initializeClient(&fClient, NUM_BIT, NUM_PARTIES);
    initializeServer(&fServer, &fClient);

    ServerKeyGt k0_gt2, k1_gt2;
    ServerKeyLt k0_lt, k1_lt;
    ServerKeyEq k0_eq, k1_eq;

    generateTreeGt(&fServer, &k0_gt2, &k1_gt2, 0, 1);
    generateTreeLt(&fServer, &k0_lt, &k1_lt, 1, 1);
    generateTreeEq(&fServer, &k0_eq, &k1_eq, 0, 1);

    vector<vector<uint64_t>> triple_shares =
        generate_beaver_triple(NUM_PARTIES, DEFAULT_MOD);
    vector<vector<mpz_class>> triple_shares_mpz =
        generate_beaver_triple_mpz(NUM_PARTIES, DEFAULT_MOD);
    vector<vector<uint64_t>> triple_ed =
        generate_beaver_triple(NUM_PARTIES, SED_MOD);

    /* ============================================================
       test different k and datasets
       ============================================================ */
    ofstream fout(out_file, ios::trunc);
    if (!fout.is_open())
    {
        cerr << "Cannot open " << out_file << "\n";
        // continue;
    }
    for (int k : k_values)
    {

        cout << "\n======================================================== k = " << k << " =====\n";
        fout << "===== Query Latency Test (k = " << k << ") =====\n";

        /* ===== latency for each dataset ===== */
        for (const auto &file_name : datasets)
        {
            string dataset_path = data_dir + file_name;

            cout << "---------------------------------------------\n";
            cout << "Dataset: " << file_name << "\n";
            cout << "Path   : " << dataset_path << "\n";

            /* ---- load dataset ---- */
            Dataset data = load_mapped_dataset(dataset_path);

            /* ---- print & save dataset info ---- */
            print_dataset_info(file_name, data);
            print_dataset_info_to_file(file_name, data, fout);

            uint64_t n = data.data.size();
            if (n == 0 || (uint64_t)k >= n)
            {
                cout << "Invalid n or k, skip.\n";
                fout << "Invalid n or k, skip.\n\n";
                continue;
            }

            /* ---- gen key for f>_{n-k-1,1}  ---- */
            ServerKeyGt k0_gt, k1_gt;
            generateTreeGt(&fServer, &k0_gt, &k1_gt, n - 5 - 1, 1);

            /* ---- secret share all data ---- */
            vector<DataPointShares> shares = getSharedData(data.data);

            vector<vector<uint64_t>> X1, X2;
            vector<uint64_t> L1, L2;

            for (const auto &p : shares)
            {
                X1.push_back(p.features_shares[0]);
                X2.push_back(p.features_shares[1]);
                L1.push_back(p.target_shares[0]);
                L2.push_back(p.target_shares[1]);
            }

            /* ---- query = first data point ---- */
            vector<uint64_t> y1 = shares[0].features_shares[0];
            vector<uint64_t> y2 = shares[0].features_shares[1];

            vector<uint64_t> D1, D2, d;
            D1.reserve(n);
            D2.reserve(n);

            auto start = clock_start();

            /* ===== secED ===== */
            for (size_t j = 0; j < n; j++)
            {
                d = secED_nDim(triple_ed, X1[j], X2[j], y1, y2);
                D1.push_back(d[0]);
                D2.push_back(d[1]);
            }
            ShuffleMaterial shuffleMaterial = generateShuffleMaterial(n);
            vector<vector<uint64_t>> lkmin;
            /* ===== secKMin ===== */
            if (is_parallel == 0)
            {
                lkmin = secKMin(triple_shares, D1, D2, L1, L2,
                                shuffleMaterial,
                                &fServer,
                                &k0_lt, &k1_lt,
                                &k0_gt, &k1_gt,
                                &k0_eq, &k1_eq,1);
            }
            else if (is_parallel == 1)
            {
                lkmin = secKMin_parallel(triple_shares, D1, D2, L1, L2,
                                         shuffleMaterial,
                                         &fServer,
                                         &k0_lt, &k1_lt,
                                         &k0_gt, &k1_gt,
                                         &k0_eq, &k1_eq,
                                        num_threads);
            }

            /* ===== secFre ===== */
            vector<vector<mpz_class>> f_x_shares =
                secFre(lkmin, &fServer, &k0_eq, &k1_eq);

            vector<vector<uint64_t>> fxshares =
                convertMpzClassToUint64(f_x_shares);
            vector<vector<mpz_class>> lkmins =
                convertUint64ToMpzClass(lkmin);

            for (size_t j = 0; j < fxshares[0].size(); j++)
                fxshares[1][j] = 0 - fxshares[1][j];

            /* ===== secMax ===== */
            secMax(triple_shares_mpz,
                   fxshares[0], fxshares[1],
                   lkmins[0], lkmins[1],
                   &fServer,
                   &k0_gt2, &k1_gt2,
                   &k0_eq, &k1_eq);

            double latency = sec_from(start);

            fout << fixed << setprecision(3) << "Latency (1 query) = " << latency << " s\n";

            cout << fixed << setprecision(3) << "Latency (1 query) = " << latency << " s\n\n";
        }
        cout << "Saved to " << out_file << "\n";
    }
    cout << "\n=============================================\n";
    cout << "All tests finished.\n";
    fout << "\n===== Finished =====\n";
    fout.close();
    return 0;
}
