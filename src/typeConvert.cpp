#include "../include/secknn.h"

// mpz_class to uint64_t 
vector<vector<uint64_t>> convertMpzClassToUint64(const vector<vector<mpz_class>>& mpzArray) {
    vector<vector<uint64_t>> uint64Array(mpzArray.size());
    for (size_t i = 0; i < mpzArray.size(); ++i) {
        uint64Array[i].resize(mpzArray[i].size());
        for (size_t j = 0; j < mpzArray[i].size(); ++j) {
            uint64Array[i][j] = mpzArray[i][j].get_ui();  // convert to uint64_t
        }
    }
    return uint64Array;
}

// uint64_t to mpz_class 
vector<vector<mpz_class>> convertUint64ToMpzClass(const vector<vector<uint64_t>>& uint64Array) {
    vector<vector<mpz_class>> mpzArray(uint64Array.size());
    for (size_t i = 0; i < uint64Array.size(); ++i) {
        mpzArray[i].resize(uint64Array[i].size());
        for (size_t j = 0; j < uint64Array[i].size(); ++j) {
            mpzArray[i][j] = mpz_class(uint64Array[i][j]);  //convert to mpz_class
        }
    }
    return mpzArray;
}
