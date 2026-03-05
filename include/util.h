#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "dataset.h"

//load data
Dataset load_mapped_dataset(const std::string& mapped_path);

std::string infer_dataset_name(const std::string& path);

//print data information
void print_dataset_info(const std::string& name, const Dataset& ds);

//save dataset info
void print_dataset_info_to_file( const std::string& name, const Dataset& ds,std::ostream& os );

//CSV split
static inline std::vector<std::string> split_csv_simple(const std::string& line);

//split dataset 
void stratified_split( const Dataset& full, Dataset& train, 
    Dataset& test,double test_ratio, uint32_t seed);

//squared Euclidean distance
uint64_t squared_distance_ring(
    const std::vector<uint64_t>& a,
    const std::vector<uint64_t>& b);

//plaintext KNN
uint64_t knn_predict(
    const std::vector<DataPoint>& data,
    const std::vector<uint64_t>& query,
    int k,
    size_t num_labels);
//plaintext KNN accuracy
// void evaluate_knn_accuracy(
//     const Dataset& train,
//     const Dataset& test,
//     const std::vector<int>& k_values
// );
