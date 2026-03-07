#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecShuffle (Algorithm 5 in the paper).
 * In the paper:
 *     SecShuffle([x])

 * Invoked in:
 *     Algorithm 8 (SecKMin), Line 1 (Online)
 * ============================================================
 */

 // generate random permutation
// vector<int> random_permutation(int n) {
//     vector<int> pi(n);
//     for (int i = 0; i < n; i++) pi[i] = i;

//     random_device rd;
//     mt19937 g(42);
//     shuffle(pi.begin(), pi.end(), g);
//     return pi;
// }

vector<int> random_permutation(int n) {
    vector<int> pi(n);
    for (int i = 0; i < n; i++) pi[i] = i;

    mt19937 g(42);

    for (int i = n - 1; i > 0; --i) {
        int j = g() % (i + 1);
        std::swap(pi[i], pi[j]);
    }
    
    return pi;
}

// apply permutation to vector
vector<uint64_t> apply_permutation(const vector<uint64_t>& v,
                                   const vector<int>& pi) {
    int n = v.size();
    vector<uint64_t> res(n);
    for (int i = 0; i < n; i++)
        res[i] = v[pi[i]];
    return res;
}

ShuffleMaterial generateShuffleMaterial(int n) {

    ShuffleMaterial shuffleMaterial;
    //generate pi0 & pi1
    shuffleMaterial.pi0 = random_permutation(n);
    shuffleMaterial.pi1 = random_permutation(n);

    shuffleMaterial.a0.resize(n);
    shuffleMaterial.a1.resize(n);

    // generate masking
    for (int i = 0; i < n; i++) {
        uint64_t r = rand() % 1024;
        vector<uint64_t> r_share= additive_secret_sharing(r,NUM_PARTIES);
        shuffleMaterial.a0[i] = r_share[0];
        shuffleMaterial.a1[i] = r_share[1];
    }

    // compute b = pi0(pi1(a0)+a1), then generate b0 & b1
    auto pi1_a0 = apply_permutation(shuffleMaterial.a0, shuffleMaterial.pi1);

    vector<uint64_t> tmp(n);
    for (int i = 0; i < n; i++)
        tmp[i] = pi1_a0[i] + shuffleMaterial.a1[i];

    auto b_plain = apply_permutation(tmp, shuffleMaterial.pi0);

    shuffleMaterial.b0.resize(n);
    shuffleMaterial.b1.resize(n);

    for (int i = 0; i < n; i++) {
        uint64_t r = rand() % 1024;
        shuffleMaterial.b0[i] = r;
        shuffleMaterial.b1[i] = b_plain[i] - r;
    }

    return shuffleMaterial;
}
void secShuffle(
    vector<uint64_t>& share0,
    vector<uint64_t>& share1,
    const ShuffleMaterial& shuffleMaterial
) {
    int n = share0.size();

    // Step 1: S0 computes x'_0 = x0 + a0
    vector<uint64_t> x_prime0(n);
    for (int i = 0; i < n; i++)
        x_prime0[i] = share0[i] + shuffleMaterial.a0[i];

    // Step 2: S1 computes x''
    vector<uint64_t> tmp(n);
    for (int i = 0; i < n; i++)
        tmp[i] = x_prime0[i] + share1[i];

    auto pi1_tmp = apply_permutation(tmp, shuffleMaterial.pi1);

    vector<uint64_t> x_double(n);
    for (int i = 0; i < n; i++)
        x_double[i] = pi1_tmp[i] + shuffleMaterial.a1[i];

    // Step 3: S0 computes y0
    auto pi0_x_double = apply_permutation(x_double, shuffleMaterial.pi0);
    for (int i = 0; i < n; i++)
        share0[i] = pi0_x_double[i] - shuffleMaterial.b0[i];

    // Step 4: S1 sets y1
    for (int i = 0; i < n; i++)
        share1[i] = -shuffleMaterial.b1[i];
}
