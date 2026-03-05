#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecEqTest (Algorithm 9 in the paper).
 * In the paper:
 *     SecEqTest([x],[y] or y)

 * Invoked in:
 *     Algorithm 10 (SecFre), Line 2
 *     Algorithm 11 (SecMax), Line 2
 * ============================================================
 */

// input shares of x and y
vector<mpz_class> secEqTest(uint64_t x_share1, uint64_t x_share2,
                            int64_t y_share1, uint64_t y_share2,
                            Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1)//k0,k1<-Gen_{0,1}
{
    // cout << "secEqTest" << endl;
    vector<mpz_class> u(2, 0);
    uint64_t delta = (x_share1 + x_share2) - (y_share1 + y_share2);//for fss shares, f(x)=f_1(x)-f_2
    u[0] = evaluateEq(fServer, k0, delta);
    u[1] = evaluateEq(fServer, k1, delta);
    return u;
}
// input shares of n x and a scalar y
vector<vector<mpz_class>> secEqTest(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, uint64_t y,
                                      Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1)//k0,k1<-Gen_{0,1}
{
    uint64_t k = X_shares1.size();
    // y = k - 1 or 0;
    vector<vector<mpz_class>> U(2, vector<mpz_class>(k, 0));//2*k matrix
    for (size_t i = 0; i < k; i++)
    {
        // cout << "x_i= " << X_shares1[i] + X_shares2[i] << endl;
        uint64_t delta_i = (X_shares1[i] + X_shares2[i]) - y; // x-y
        // cout << "delta_i = " << delta_i << endl;
        U[0][i] = evaluateEq(fServer, k0, delta_i);
        U[1][i] = evaluateEq(fServer, k1, delta_i);
    }
    return U;
}
vector<vector<mpz_class>> secZeroTest(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, uint64_t y,//input y=0
                                      Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1)//k0,k1<-Gen_{0,1}
{
    return secEqTest(X_shares1, X_shares2, y, fServer, k0, k1);
}

