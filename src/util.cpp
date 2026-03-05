#include "../include/secknn.h"

using namespace std;
/* ========== CSV split ========== */
static inline vector<string> split_csv_simple(const string& line) {
    vector<string> out;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ',')) out.push_back(token);
    return out;
}

/* ========== load data ========== */
Dataset load_mapped_dataset(const string& mapped_path) {
    ifstream file(mapped_path);
    if (!file.is_open()) {
        cerr << "Cannot open mapped file: " << mapped_path << endl;
        exit(1);
    }

    Dataset ds;
    string line;
    bool first = true;
    unordered_set<uint64_t> label_set;

    while (getline(file, line)) {
        if (line.empty()) continue;
        if (first) { first = false; continue; } // skip header

        auto toks = split_csv_simple(line);
        if (toks.size() < 2) {
            cerr << "Invalid mapped row: " << line << endl;
            exit(1);
        }

        if (ds.info.num_features == 0)
            ds.info.num_features = toks.size() - 1;

        vector<uint64_t> feats;
        feats.reserve(ds.info.num_features);
        for (size_t i = 0; i < ds.info.num_features; i++)
            feats.push_back(stoull(toks[i]));

        uint64_t lbl = stoull(toks.back());
        ds.data.push_back({feats, lbl});
        label_set.insert(lbl);
    }

    ds.info.num_samples = ds.data.size();
    ds.info.num_labels  = label_set.size();
    return ds;
}

/* ========== extract dataset name ========== */
string infer_dataset_name(const string& path) {
    size_t pos = path.find_last_of("/\\");
    string name = (pos == string::npos) ? path : path.substr(pos + 1);
    pos = name.find_last_of('.');
    if (pos != string::npos)
        name = name.substr(0, pos);

    return name;
}

/* ========== print data information ========== */
void print_dataset_info(const string& name, const Dataset& ds) {
    cout << "================*******==================\n";
    cout << "Dataset        : " << name << "\n";
    cout << "#Sample (n)    : " << ds.info.num_samples << "\n";
    cout << "#Feature(m)    : " << ds.info.num_features << "\n";
    cout << "#Label(c)      : " << ds.info.num_labels << "\n";
    cout << "===========runing...============\n";
}
void print_dataset_info_to_file(const string& name, const Dataset& ds, ostream& os) {
    os << "================*******==================\n";
    os << "Dataset        : " << name << "\n";
    os << "#Sample (n)    : " << ds.info.num_samples << "\n";
    os << "#Feature(m)    : " << ds.info.num_features << "\n";
    os << "#Label(c)      : " << ds.info.num_labels << "\n";
}

/* ===================== split dataset ===================== */
/* stratified train / test split, we employ 8:2 */

void stratified_split(
    const Dataset& full,
    Dataset& train,
    Dataset& test,
    double test_ratio,
    uint32_t seed
) {
    size_t n = full.data.size();
    size_t L = full.info.num_labels;

    vector<vector<size_t>> by_label(L);
    for (size_t i = 0; i < n; i++)
        by_label[full.data[i].label].push_back(i);

    mt19937 rng(seed);

    vector<size_t> train_idx, test_idx;

    for (auto& idxs : by_label) {
        shuffle(idxs.begin(), idxs.end(), rng);
        size_t n_test = static_cast<size_t>(
            round(idxs.size() * test_ratio)
        );

        for (size_t i = 0; i < idxs.size(); i++) {
            if (i < n_test)
                test_idx.push_back(idxs[i]);
            else
                train_idx.push_back(idxs[i]);
        }
    }

    train.data.clear();
    for (size_t i : train_idx)
        train.data.push_back(full.data[i]);

    train.info = full.info;
    train.info.num_samples = train.data.size();

    test.data.clear();
    for (size_t i : test_idx)
        test.data.push_back(full.data[i]);

    test.info = full.info;
    test.info.num_samples = test.data.size();
}
/* ======= squared Euclidean distance====== */
/*
 * d(x,y) = sum_i (x_i - y_i)^2 
 */
uint64_t squared_distance_ring(
    const vector<uint64_t>& a,
    const vector<uint64_t>& b)
{
    uint64_t dist = 0; 

    for (size_t i = 0; i < a.size(); i++) {
        uint64_t diff = a[i] - b[i]; 
        uint64_t sq   = diff * diff;  
        dist += sq;                   
    }
    return dist;
}

/* ====== plaintext KNN ======== */

uint64_t knn_predict(
    const vector<DataPoint>& data,
    const vector<uint64_t>& query,
    int k,
    size_t num_labels)
{
    vector<pair<uint64_t, uint64_t>> dl;
    dl.reserve(data.size());

    for (const auto& p : data) {
        uint64_t d = squared_distance_ring(p.features, query);
        dl.emplace_back(d, p.label);
    }

    // unsigned comparison on uint64_t
    sort(dl.begin(), dl.end(),
     [](const pair<uint64_t, uint64_t>& x,
        const pair<uint64_t, uint64_t>& y) {
         if (x.first != y.first) return x.first < y.first;
         return x.second < y.second;
     });


    vector<int> votes(num_labels, 0);
    for (int i = 0; i < k; i++) {
        votes[dl[i].second]++;
    }

    return static_cast<uint64_t>(
        max_element(votes.begin(), votes.end()) - votes.begin()
    );
}
