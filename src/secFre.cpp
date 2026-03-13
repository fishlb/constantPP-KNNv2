#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecFre (Algorithm 10 in the paper).
 * In the paper:
 *     SecFre([x_i])

 * Invoked in:
 *     Algorithm 3 (SecKC), Line 3
 * ============================================================
 */
vector<vector<mpz_class>> secFre(vector<vector<uint64_t>> X_shares,
                                Fss *fServer, ServerKeyEq *k0, ServerKeyEq *k1)//Gen_{0,1}
{
    uint64_t k = X_shares[0].size();
    uint64_t x_i_1, x_i_2, x_j_1, x_j_2;
    vector<vector<mpz_class>> F(2, vector<mpz_class>(k,0));
    vector<mpz_class> f_shares(2,0);
    for (size_t i = 0; i < k; i++)
    {
        x_i_1 = X_shares[0][i];
        x_i_2 = X_shares[1][i];
        for (size_t j = i+1; j < k; j++)//compare to others
        {
            x_j_1 = X_shares[0][j];
            x_j_2 = X_shares[1][j];
            f_shares = secEqTest(x_i_1, x_i_2, x_j_1, x_j_2, fServer, k0, k1);
            // cout<<"eqtest = "<<f_shares[0].get_si() - f_shares[1].get_si() <<endl;
            F[0][i] += f_shares[0];
            F[1][i] += f_shares[1];
            F[0][j] += f_shares[0];
            F[1][j] += f_shares[1];
        }
        F[0][i] = F[0][i] + 1;//plus one (frequency) for itself.
    }
    return F;
}
