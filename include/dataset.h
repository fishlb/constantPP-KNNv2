#pragma once
#include <vector>
#include <cstdint>

struct DataPoint {
    std::vector<uint64_t> features;
    uint64_t label;
};

struct DatasetInfo {
    size_t num_samples = 0;
    size_t num_features = 0;
    size_t num_labels = 0;
};

struct Dataset {
    std::vector<DataPoint> data;
    DatasetInfo info;
};
struct DataPointShares {
    std::vector<std::vector<uint64_t>> features_shares;//2*n matrix
    std::vector<uint64_t> target_shares;
};
