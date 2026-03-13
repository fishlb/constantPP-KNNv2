#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecMax (Algorithm 11 in the paper).
 * In the paper:
 *     SecMax([x_i],[l_i])

 * Invoked in:
 *     Algorithm 3 (SecKC), Line 3
 * ============================================================
 */
vector<mpz_class> secMax(const vector<vector<mpz_class>> triple_shares,
                        vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                        vector<mpz_class> L_shares1, vector<mpz_class> L_shares2,
                        // vector<uint64_t> r_shares,
                        Fss *fServer,
                        ServerKeyGt *k0_gt, ServerKeyGt *k1_gt, // Gen_{0,1}
                        ServerKeyEq *k0_eq, ServerKeyEq *k1_eq) // Gen_{0,1}
{
    uint64_t k = X_shares1.size();
    vector<vector<uint64_t>> SU = secBGtCom(X_shares1, X_shares2, fServer, k0_gt, k1_gt);

    for (size_t i = 0; i < SU[0].size(); i++)
    {
        SU[1][i] = 0 - SU[1][i];//for fss shares, f(x)=f_1(x)-f_2(x), so, f(x)=f_1(x)+(-f_2(x)), the operation if EqTest using +
        // cout << "after secBGCOm SU[i] = "<< SU[0][i] + SU[1][i] << endl;
    }
    vector<vector<mpz_class>> U = secEqTest(SU[0], SU[1], k - 1, fServer, k0_eq, k1_eq);
    for (size_t i = 0; i < U[0].size(); i++)
    {
        U[1][i] = 0 - U[1][i];
        // cout << "U[i] = "<< U[0][i].get_si() + U[1][i].get_si() << endl;   
    }
    vector<vector<mpz_class>> L_pi = secMul_mpz(triple_shares, L_shares1, L_shares2, U[0], U[1]);
    vector<mpz_class> lmax(2, 0);
    for (size_t i = 0; i < L_pi[0].size(); i++)
    {
        lmax[0] += L_pi[0][i];
        lmax[1] += L_pi[1][i];
    }
    mpz_class final_label = lmax[0] + lmax[1];

    return lmax;
}