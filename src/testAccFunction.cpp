#include "../include/secknn.h"
#include <iomanip>
/*
 *test plaintext baseline accuracy and our constantPP-KNN accuracy
 */

// test plaintext baseline accuracy
int testPlainAcc()
{
    cout << "=============================================\n";
    cout << "=====Plaintext Baseline Accuracy (TABLE V, Real datasets) =====\n";
    // ===== data and output dir =====
    string data_dir = "../dataset/real/";
    string output_dir = "../evalResult/";
    /* ===== real datasets ===== */
    vector<string> datasets = {
        "Iris.csv",
        "Toxicity.csv",
        "Wine.csv",
        "Arcene.csv",
        "Chronic Kidney Disease.csv",
        "Breast Cancer.csv"};
    /* ===== output file ===== */
    ofstream fout(output_dir + "tableV_accuracy_result_plaintext baseline.txt", ios::trunc);
    if (!fout.is_open())
    {
        cerr << "Cannot open " + output_dir + "tableV_accuracy_result_plaintext baseline.txt\n";
        return 1;
    }
    /* ===== accuracy test ===== */
    fout << "=====TABLE V - Plaintext Baseline Accuracy (Real datasets)  =====\n";
    for (const auto &file_name : datasets)
    {
        string dataset_paths = data_dir + file_name;
        cout << "=============================================\n";
        cout << "Dataset: " << file_name << endl;
        cout << "Path:    " << dataset_paths << endl;
        /* ---- load dataset ---- */
        Dataset data = load_mapped_dataset(dataset_paths);
        /* ---- print and save results ---- */
        print_dataset_info(file_name, data);
        print_dataset_info_to_file(file_name, data, fout);
        /* split data, train:test=8:2 */
        Dataset train, test;
        stratified_split(data, train, test, 0.2, 42);

        cout << "Train samples: " << train.info.num_samples << endl;
        cout << "Test samples : " << test.info.num_samples << endl;

        uint64_t n = train.data.size();
        if (n == 0 || test.data.empty())
        {
            cout << "Invalid split, skip.\n";
            continue;
        }

        /* KNN accuracy test */
        int k_values[] = {5};

        for (int k : k_values)
        {
            if (k <= 0 || k > static_cast<int>(train.data.size()))
                continue;

            int correct = 0;
            for (const auto &q : test.data)
            {
                uint64_t pred = knn_predict(
                    train.data,
                    q.features,
                    k,
                    train.info.num_labels);
                if (pred == q.label)
                    correct++;
            }

            double accuracy =
                static_cast<double>(correct) / test.data.size();

            // cout << "k = " << k
            //      << " | Accuracy = " << (double)accuracy << "\n";
            // fout << "Accuracy  = " << (double)accuracy << "\n";
            cout << "k = " << k
                 << " | Accuracy = " << fixed << setprecision(3) << accuracy << "\n";

            fout << "Accuracy = " << fixed << setprecision(3) << accuracy << "\n";
        }
    }
    fout << "\n===== Finished =====\n";
    fout.close();

    cout << "=============================================\n";
    cout << "\n===== Finished =====\n";
    cout << "Saved to " + output_dir + "tableV_accuracy_result_plaintext baseline.txt\n";
    return 0;
}
// test constantPP-KNN accuracy
int testConstKNNAcc()
{
    cout << "=============================================\n";
    cout << "=====TABLE V - Ours Accuracy (Real datasets) =====\n";
    // ===== data and output dir =====
    string data_dir = "../dataset/real/";
    string output_dir = "../evalResult/";
    vector<string> datasets = {
        "Iris.csv",
        "Toxicity.csv",
        "Wine.csv",
        "Arcene.csv",
        "Chronic Kidney Disease.csv",
        "Breast Cancer.csv"};
    /* ===== output file ===== */
    ofstream fout(output_dir + "tableV_accuracy_result_ours.txt", ios::trunc);
    if (!fout.is_open())
    {
        cerr << "Cannot open " + output_dir + "tableV_accuracy_result_ours.txt\n";
        return 1;
    }
    /* ===== Init ===== */
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

    /* ===== accuracy test ===== */
    fout << "=====TABLE V - Ours Accuracy (Real datasets) =====\n";
    for (const auto &file_name : datasets)
    {
        string dataset_paths = data_dir + file_name;
        cout << "=============================================\n";
        cout << "Dataset: " << file_name << endl;
        cout << "Path:    " << dataset_paths << endl;
        /* ---- load dataset ---- */
        Dataset data = load_mapped_dataset(dataset_paths);
        /* ---- print and save results ---- */
        print_dataset_info(file_name, data);
        print_dataset_info_to_file(file_name, data, fout);
        /* split data, train:test=8:2 */
        Dataset train, test;
        stratified_split(data, train, test, 0.2, 42);

        cout << "Train samples: " << train.info.num_samples << endl;
        cout << "Test samples : " << test.info.num_samples << endl;

        uint64_t n = train.data.size();
        if (n == 0 || test.data.empty())
        {
            cout << "Invalid split, skip.\n";
            continue;
        }
        /* ---- gen key for f>_{n-k-1,1} ---- */
        ServerKeyGt k0_gt, k1_gt;
        generateTreeGt(&fServer, &k0_gt, &k1_gt, n - 5 - 1, 1);

        /* ===== secret share data ===== */
        vector<DataPointShares> train_shares =
            getSharedData(train.data);
        vector<DataPointShares> test_shares =
            getSharedData(test.data);

        /* ---- matrix of feature and label shares ---- */
        vector<vector<uint64_t>> X_fea_shares1, X_fea_shares2;
        vector<uint64_t> L_shares1, L_shares2;

        for (const auto &p : train_shares)
        {
            X_fea_shares1.push_back(p.features_shares[0]);
            X_fea_shares2.push_back(p.features_shares[1]);
            L_shares1.push_back(p.target_shares[0]);
            L_shares2.push_back(p.target_shares[1]);
        }

        /* KNN accuracy test */
        int correctCount = 0;
        double time_sum = 0.0;

        vector<uint64_t> y_fea_shares1, y_fea_shares2;
        vector<uint64_t> d, D_shares1, D_shares2;

        for (size_t i = 0; i < test.data.size(); i++)
        {

            y_fea_shares1 = test_shares[i].features_shares[0];
            y_fea_shares2 = test_shares[i].features_shares[1];

            D_shares1.clear();
            D_shares2.clear();

            for (size_t j = 0; j < n; j++)
            {
                d = secED_nDim(
                    triple_ed,
                    X_fea_shares1[j], X_fea_shares2[j],
                    y_fea_shares1, y_fea_shares2);
                D_shares1.push_back(d[0]);
                D_shares2.push_back(d[1]);
            }
            ShuffleMaterial shuffleMaterial = generateShuffleMaterial(n);
            vector<vector<uint64_t>> lkmin;
            lkmin = secKMin_parallel(
                triple_shares,
                D_shares1, D_shares2,
                L_shares1, L_shares2,
                shuffleMaterial,
                &fServer,
                &k0_lt, &k1_lt,
                &k0_gt, &k1_gt,
                &k0_eq, &k1_eq);

            vector<vector<mpz_class>> f_x_shares =
                secFre(lkmin, &fServer, &k0_eq, &k1_eq);

            vector<vector<uint64_t>> fxshares =
                convertMpzClassToUint64(f_x_shares);

            vector<vector<mpz_class>> lkmins =
                convertUint64ToMpzClass(lkmin);

            for (size_t j = 0; j < fxshares[0].size(); j++)
                fxshares[1][j] = 0 - fxshares[1][j];

            vector<mpz_class> l_max = secMax(
                triple_shares_mpz,
                fxshares[0], fxshares[1],
                lkmins[0], lkmins[1],
                &fServer,
                &k0_gt2, &k1_gt2,
                &k0_eq, &k1_eq);

            mpz_class final_label = l_max[0] + l_max[1];

            if (final_label.get_ui() == test.data[i].label)
                correctCount++;
        }

        long double accuracy =
            (long double)correctCount /
            (long double)test.data.size();

        // cout << "k = " << 5
        //      << " | Accuracy = " << (double)accuracy << "\n";
        // fout << "Accuracy  = " << (double)accuracy << "\n";
        cout << "k = " << 5
             << " | Accuracy = " << fixed << setprecision(3) << accuracy << "\n";

        fout << "Accuracy = " << fixed << setprecision(3) << accuracy << "\n";
    }
    fout << "\n===== Finished =====\n";
    fout.close();

    cout << "=============================================\n";
    cout << "\n===== Finished =====\n";
    cout << "Saved to " + output_dir + "tableV_accuracy_result_ours.txt\n";
    return 0;
}

int testConstKNNAcc2(int is_parallel)
{
    string parallel_flag = "";
    if (is_parallel == 0)
    {
        parallel_flag = "sequential";
    }
    else
    {
        parallel_flag = "parallel";
    }

    cout << "=============================================\n";
    cout << "=====TABLE V - Ours Accuracy (Real datasets) =====\n";
    // ===== data and output dir =====
    string data_dir = "../dataset/real/";
    string output_dir = "../evalResult/";
    vector<string> datasets = {
        "Iris.csv",
        "Toxicity.csv",
        "Wine.csv",
        "Arcene.csv",
        "Chronic Kidney Disease.csv",
        "Breast Cancer.csv"};
    /* ===== output file ===== */
    ofstream fout(output_dir + "tableV_accuracy_result_ours_" + parallel_flag + ".txt", ios::trunc);
    if (!fout.is_open())
    {
        cerr << "Cannot open " + output_dir + "tableV_accuracy_result_ours_" + parallel_flag + ".txt\n";
        return 1;
    }
    /* ===== Init ===== */
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

    /* ===== accuracy test ===== */
    fout << "=====TABLE V - Ours Accuracy (Real datasets) =====\n";
    for (const auto &file_name : datasets)
    {
        string dataset_paths = data_dir + file_name;
        cout << "=============================================\n";
        cout << "Dataset: " << file_name << endl;
        cout << "Path:    " << dataset_paths << endl;
        /* ---- load dataset ---- */
        Dataset data = load_mapped_dataset(dataset_paths);
        /* ---- print and save results ---- */
        print_dataset_info(file_name, data);
        print_dataset_info_to_file(file_name, data, fout);
        /* split data, train:test=8:2 */
        Dataset train, test;
        stratified_split(data, train, test, 0.2, 42);

        cout << "Train samples: " << train.info.num_samples << endl;
        cout << "Test samples : " << test.info.num_samples << endl;

        uint64_t n = train.data.size();
        if (n == 0 || test.data.empty())
        {
            cout << "Invalid split, skip.\n";
            continue;
        }
        /* ---- gen key for f>_{n-k-1,1} ---- */
        ServerKeyGt k0_gt, k1_gt;
        generateTreeGt(&fServer, &k0_gt, &k1_gt, n - 5 - 1, 1);

        /* ===== secret share data ===== */
        vector<DataPointShares> train_shares =
            getSharedData(train.data);
        vector<DataPointShares> test_shares =
            getSharedData(test.data);

        /* ---- matrix of feature and label shares ---- */
        vector<vector<uint64_t>> X_fea_shares1, X_fea_shares2;
        vector<uint64_t> L_shares1, L_shares2;

        for (const auto &p : train_shares)
        {
            X_fea_shares1.push_back(p.features_shares[0]);
            X_fea_shares2.push_back(p.features_shares[1]);
            L_shares1.push_back(p.target_shares[0]);
            L_shares2.push_back(p.target_shares[1]);
        }

        /* KNN accuracy test */
        int correctCount = 0;
        double time_sum = 0.0;

        vector<uint64_t> y_fea_shares1, y_fea_shares2;
        vector<uint64_t> d, D_shares1, D_shares2;

        for (size_t i = 0; i < test.data.size(); i++)
        {

            y_fea_shares1 = test_shares[i].features_shares[0];
            y_fea_shares2 = test_shares[i].features_shares[1];

            D_shares1.clear();
            D_shares2.clear();

            for (size_t j = 0; j < n; j++)
            {
                d = secED_nDim(
                    triple_ed,
                    X_fea_shares1[j], X_fea_shares2[j],
                    y_fea_shares1, y_fea_shares2);
                D_shares1.push_back(d[0]);
                D_shares2.push_back(d[1]);
            }
            ShuffleMaterial shuffleMaterial = generateShuffleMaterial(n);
            vector<vector<uint64_t>> lkmin;
            if (is_parallel == 0)
            {
                lkmin = secKMin(
                    triple_shares,
                    D_shares1, D_shares2,
                    L_shares1, L_shares2,
                    shuffleMaterial,
                    &fServer,
                    &k0_lt, &k1_lt,
                    &k0_gt, &k1_gt,
                    &k0_eq, &k1_eq);
            }
            else if (is_parallel == 1)
            {
                lkmin = secKMin_parallel(
                    triple_shares,
                    D_shares1, D_shares2,
                    L_shares1, L_shares2,
                    shuffleMaterial,
                    &fServer,
                    &k0_lt, &k1_lt,
                    &k0_gt, &k1_gt,
                    &k0_eq, &k1_eq);
            }

            vector<vector<mpz_class>> f_x_shares =
                secFre(lkmin, &fServer, &k0_eq, &k1_eq);

            vector<vector<uint64_t>> fxshares =
                convertMpzClassToUint64(f_x_shares);

            vector<vector<mpz_class>> lkmins =
                convertUint64ToMpzClass(lkmin);

            for (size_t j = 0; j < fxshares[0].size(); j++)
                fxshares[1][j] = 0 - fxshares[1][j];

            vector<mpz_class> l_max = secMax(
                triple_shares_mpz,
                fxshares[0], fxshares[1],
                lkmins[0], lkmins[1],
                &fServer,
                &k0_gt2, &k1_gt2,
                &k0_eq, &k1_eq);

            mpz_class final_label = l_max[0] + l_max[1];

            if (final_label.get_ui() == test.data[i].label)
                correctCount++;
        }

        long double accuracy =
            (long double)correctCount /
            (long double)test.data.size();

        // cout << "k = " << 5
        //      << " | Accuracy = " << (double)accuracy << "\n";
        // fout << "Accuracy  = " << (double)accuracy << "\n";
        cout << "k = " << 5
             << " | Accuracy = " << fixed << setprecision(3) << accuracy << "\n";

        fout << "Accuracy = " << fixed << setprecision(3) << accuracy << "\n";
    }
    fout << "\n===== Finished =====\n";
    fout.close();

    cout << "=============================================\n";
    cout << "\n===== Finished =====\n";
    cout << "Saved to " + output_dir + "tableV_accuracy_result_ours_" + parallel_flag + ".txt\n";
    return 0;
}
