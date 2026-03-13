#pragma once
#include <chrono>
#include <vector>
#include <cstdint>
#include <cstdlib> // for rand()
#include "timer.h"
#include "dataset.h"
#include <gmpxx.h> //for mpz_class
#include "../libfss/cpp/fss-common.h"
#include "../libfss/cpp/fss-server.h"
#include "../libfss/cpp/fss-client.h"
#include "../libfss/cpp/utils.h"
#include "../include/util.h"

#define NUM_BIT 10    // data length of fss
#define NUM_PARTIES 2 // num of parties in fss
#define DEFAULT_MOD 1 // a,b,c=ab
#define SED_MOD 0     // a,a,c=a*a, to compute secure ED

#define rin 128
struct ShuffleMaterial
{
    vector<int> pi0;
    vector<int> pi1;

    vector<uint64_t> a0;
    vector<uint64_t> a1;

    vector<uint64_t> b0;
    vector<uint64_t> b1;
};

vector<DataPointShares> getSharedData(vector<DataPoint> data); // each point with n dimension features and one label

vector<uint64_t> additive_secret_sharing(uint64_t secret, int num_participants);
vector<mpz_class> additive_secret_sharing_mpz(mpz_class secret, int num_participants);
// SS for n data
vector<vector<uint64_t>> additive_secret_sharing(vector<uint64_t> secret, int num_participants);
vector<vector<uint64_t>> generate_beaver_triple(int num_participants, bool test_flag);
vector<vector<mpz_class>> generate_beaver_triple_mpz(int num_participants, bool test_flag);
vector<uint64_t> secMulxy(const vector<vector<uint64_t>> &triple_shares,
                          uint64_t x_share1, uint64_t x_share2,
                          uint64_t y_share1, uint64_t y_share2);
vector<mpz_class> secMulxy_mpz(const vector<vector<mpz_class>> &triple_shares,
                               mpz_class x_share1, mpz_class x_share2,
                               mpz_class y_share1, mpz_class y_share2);
// a data point with one dimension
vector<uint64_t> secED_1Dim(const vector<vector<uint64_t>> triple_shares,
                            uint64_t x_share1, uint64_t x_share2,
                            uint64_t y_share1, uint64_t y_share2);
// a data point with high dimension
vector<uint64_t> secED_nDim(const vector<vector<uint64_t>> triple_shares,
                            vector<uint64_t> x_share1, vector<uint64_t> x_share2,
                            vector<uint64_t> y_share1, vector<uint64_t> y_share2);
// input shares of x and y
vector<mpz_class> secEqTest(uint64_t x_share1, uint64_t x_share2,
                            int64_t y_share1, uint64_t y_share2,
                            Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1); // k0,k1<-Gen_{0,1}
// input shares of n x and a scalar y
vector<vector<mpz_class>> secEqTest(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, uint64_t y,
                                    Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1);                      // k0,k1<-Gen_{0,1}
vector<vector<mpz_class>> secZeroTest(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, uint64_t y, // input y=0
                                      Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1);                    // k0,k1<-Gen_{0,1}
vector<uint64_t> secMul(const vector<vector<uint64_t>> triple_shares,                                     // for one x                                 // for one x
                        uint64_t x_share1, uint64_t x_share2,
                        uint64_t u_share1, uint64_t u_share2);
vector<mpz_class> secMul_mpz(const vector<vector<mpz_class>> triple_shares, // for one x
                             mpz_class x_share1, mpz_class x_share2,
                             mpz_class u_share1, mpz_class u_share2);
vector<vector<uint64_t>> secMul(const vector<vector<uint64_t>> &triple_shares,
                                vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, // for n x
                                vector<uint64_t> U_shares1, vector<uint64_t> U_shares2);
vector<vector<mpz_class>> secMul_mpz(const vector<vector<mpz_class>> &triple_shares,
                                     vector<mpz_class> X_shares1, vector<mpz_class> X_shares2, // for n x
                                     vector<mpz_class> U_shares1, vector<mpz_class> U_shares2);
vector<vector<uint64_t>> secBLtCom(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                   //    vector<uint64_t> r_shares,
                                   Fss *fServer, ServerKeyLt *k0, ServerKeyLt *k1); // Gen_{r,1}
vector<vector<uint64_t>> secBGtCom(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                   //    vector<uint64_t> r_shares,
                                   Fss *fServer, ServerKeyGt *k0, ServerKeyGt *k1); // Gen_{r,1}

vector<vector<uint64_t>> secBLtCom_parallel(
                                const vector<uint64_t> &X_shares1,
                                const vector<uint64_t> &X_shares2,
                                Fss *fServer,
                                ServerKeyLt *k0,
                                ServerKeyLt *k1,
                                unsigned num_threads);
vector<vector<uint64_t>> secKMin(const vector<vector<uint64_t>> &triple_shares,
                                 vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                 vector<uint64_t> L_shares1, vector<uint64_t> L_shares2,
                                 const ShuffleMaterial &shuffleMaterial,
                                 Fss *fServer,
                                 ServerKeyLt *k0_lt, ServerKeyLt *k1_lt,            // // Gen_{1,1} less than, compare to 1
                                 ServerKeyGt *k0_gt, ServerKeyGt *k1_gt,            // greater than, compare to n-k-1
                                 ServerKeyEq *k0_eq, ServerKeyEq *k1_eq, size_t i); // eq test, compare to 0
vector<vector<uint64_t>> secKMin_parallel(const vector<vector<uint64_t>> &triple_shares,
                                          vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                          vector<uint64_t> L_shares1, vector<uint64_t> L_shares2,
                                          const ShuffleMaterial &shuffleMaterial,
                                          Fss *fServer,
                                          ServerKeyLt *k0_lt, ServerKeyLt *k1_lt, // // Gen_{1,1} less than, compare to 1
                                          ServerKeyGt *k0_gt, ServerKeyGt *k1_gt, // greater than, compare to n-k-1
                                          ServerKeyEq *k0_eq, ServerKeyEq *k1_eq, // eq test, compare to 0
                                          const int num_threads);
vector<vector<mpz_class>> secFre(vector<vector<uint64_t>> X_shares,
                                 Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1); // Gen_{0,1}

vector<mpz_class> secMin(const vector<vector<mpz_class>> triple_shares,
                         vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                         vector<mpz_class> L_shares1, vector<mpz_class> L_shares2,
                         // vector<uint64_t> r_shares,
                         Fss *fServer,
                         ServerKeyLt *k0_lt, ServerKeyLt *k1_lt,  // Gen_{1,1}
                         ServerKeyEq *k0_eq, ServerKeyEq *k1_eq); // Gen_{0,1}
vector<mpz_class> secMax(const vector<vector<mpz_class>> triple_shares,
                         vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                         vector<mpz_class> L_shares1, vector<mpz_class> L_shares2,
                         // vector<uint64_t> r_shares,
                         Fss *fServer,
                         ServerKeyGt *k0_gt, ServerKeyGt *k1_gt,  // Gen_{r,1}
                         ServerKeyEq *k0_eq, ServerKeyEq *k1_eq); // Gen_{r,1}

// mpz_class to uint64_t
vector<vector<uint64_t>> convertMpzClassToUint64(const vector<vector<mpz_class>> &mpzArray);
// uint64_t to mpz_class
vector<vector<mpz_class>> convertUint64ToMpzClass(const vector<vector<uint64_t>> &uint64Array);

// test plaintext model accuracy
int testPlainAcc(int k, double split_ratio);
// test constant secKNN accuracy
int testConstKNNAcc(int k, double split_ratio,const int num_threads);

// test offline cost
void cal_cost(size_t n, size_t m, size_t k, ofstream &fout, int is_parallel, const int num_threads);
int test_offline_cost(int k1, vector<int> k2, int test_flag, int is_parallel, const int num_threads);

// test latency for one query
int testLaten(int k1, vector<int> k2, int test_flag, int is_parallel, const int num_threads);

// shuffle function
ShuffleMaterial generateShuffleMaterial(int n);
void secShuffle(
    vector<uint64_t> &share0,
    vector<uint64_t> &share1,
    const ShuffleMaterial &shuffleMaterial);