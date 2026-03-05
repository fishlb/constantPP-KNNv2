#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecMul (Algorithm 1 in the paper).
 * In the paper:
 *     SecMul([x],[y])

 * Invoked in:
 *     Algorithm 11 (SecMax), Line 3
 * ============================================================
 */
// Algorithm 1: SecMul for scalar 
vector<uint64_t> secMulxy(const vector<vector<uint64_t>> &triple_shares,
                        uint64_t x_share1, uint64_t x_share2,
                        uint64_t y_share1, uint64_t y_share2)
{
    // compute e1=x1-a1, e2=x2-a2, f1=y1-b1, f2=y2-b2
    uint64_t e1 = x_share1 - triple_shares[0][0];
    uint64_t e2 = x_share2 - triple_shares[0][1];
    uint64_t f1 = y_share1 - triple_shares[1][0];
    uint64_t f2 = y_share2 - triple_shares[1][1];

    // reconstruct e=x-a, f=y-b
    uint64_t e = e1 + e2;
    uint64_t f = f1 + f2;

    vector<uint64_t> xy_shares(2, 0);

    xy_shares[0] = e * f + triple_shares[1][0] * e + triple_shares[0][0] * f + triple_shares[2][0];
    xy_shares[1] = triple_shares[1][1] * e + triple_shares[0][1] * f + triple_shares[2][1];

    return xy_shares;
}
vector<mpz_class> secMulxy_mpz(const vector<vector<mpz_class>> &triple_shares,
                        mpz_class x_share1, mpz_class x_share2,
                        mpz_class y_share1, mpz_class y_share2)
{

    // e1=x1-a1, e2=x2-a2, f1=y1-b1, f2=y2-b2
    mpz_class e1 = x_share1 - triple_shares[0][0];
    mpz_class e2 = x_share2 - triple_shares[0][1];
    mpz_class f1 = y_share1 - triple_shares[1][0];
    mpz_class f2 = y_share2 - triple_shares[1][1];

    // e=x-a, f=y-b
    mpz_class e = e1 + e2;
    mpz_class f = f1 + f2;

    vector<mpz_class> xy_shares(2, 0);

    xy_shares[0] = e * f + triple_shares[1][0] * e + triple_shares[0][0] * f + triple_shares[2][0];
    xy_shares[1] = triple_shares[1][1] * e + triple_shares[0][1] * f + triple_shares[2][1];

    return xy_shares;
}
// Algorithm 1: SecMul for vector
vector<vector<uint64_t>> secMul(const vector<vector<uint64_t>> &triple_shares,
                         vector<uint64_t> X_shares1, vector<uint64_t> X_shares2, 
                         vector<uint64_t> U_shares1, vector<uint64_t> U_shares2)
{
    uint64_t n = X_shares1.size();
    // vector<uint64_t> xu_i;//new shares of x : {(xu_i_1,xu_i_2)}
    vector<vector<uint64_t>> X_pi(2, vector<uint64_t>(n, 0));//2*n matrix, 2 means two shares
    vector<uint64_t> x_reshared(2,0);
    
    for (size_t i = 0; i < n; i++)
    {
        x_reshared = secMulxy(triple_shares, X_shares1[i], X_shares2[i], U_shares1[i], U_shares2[i]);
        X_pi[0][i] = x_reshared[0];
        X_pi[1][i] = x_reshared[1];
    }

    return X_pi; // x_pi={}
}
vector<vector<mpz_class>> secMul_mpz(const vector<vector<mpz_class>> &triple_shares,
                         vector<mpz_class> X_shares1, vector<mpz_class> X_shares2, 
                         vector<mpz_class> U_shares1, vector<mpz_class> U_shares2)
{
    mpz_class n = X_shares1.size();
    // vector<mpz_class> xu_i;//new shares of x : {(xu_i_1,xu_i_2)}
    vector<vector<mpz_class>> X_pi(2);//2*n matrix, 2 means two shares
    vector<mpz_class> x_reshared(2,0);
    for (size_t i = 0; i < n; i++)
    {
        x_reshared = secMulxy_mpz(triple_shares, X_shares1[i], X_shares2[i], U_shares1[i], U_shares2[i]);
        X_pi[0].push_back(x_reshared[0]);
        X_pi[1].push_back(x_reshared[1]);
    }
    return X_pi; // x_pi={}
}