#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of the "greater-than" branch of SecBCom (Algorithm 7 in the paper).
 * In the paper:
 *     SecBCom([x_i], greater)

 * Invoked in:
 *     Algorithm 11 (SecMax), Line 1
 * ============================================================
 */
vector<vector<uint64_t>> secBGtCom(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                //    vector<uint64_t> r_shares,
                                   Fss *fServer, ServerKeyGt *k0, ServerKeyGt *k1) // Gen_{0,1}
{
    uint64_t n = X_shares1.size();
    vector<uint64_t> su1(n, 0);
    vector<uint64_t> su2(n, 0);
    vector<vector<uint64_t>> su(2, vector<uint64_t>(n, 0)); // 2*n matrix
    uint64_t delta_ij;
    for (size_t i = 0; i < n; i++)
    {
        uint64_t an0ij_gt = 0, an1ij_gt = 0;
        uint64_t an0ji_gt = 0, an1ji_gt = 0;
        uint64_t xi = X_shares1[i] + X_shares2[i];
        for (size_t j = i + 1; j < n; j++) 
        {
            uint64_t xj =  X_shares1[j] + X_shares2[j];              
            
            delta_ij = xi - xj + rin;
            
            if (delta_ij == rin) delta_ij = rin + 1;

            //Algorithm 6: DCF for greater-than comparison
            an0ij_gt = evaluateGt(fServer, k0, delta_ij); 
            an1ij_gt = evaluateGt(fServer, k1, delta_ij);

            su1[i] += an0ij_gt;
            su2[i] += an1ij_gt; 
                               
            an0ji_gt = an1ij_gt;
            an1ji_gt = an0ij_gt - 1;

            su1[j] += an0ji_gt;
            su2[j] += an1ji_gt;
        }
    }
    su[0] = su1;
    su[1] = su2;
    return su;
}