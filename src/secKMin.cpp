#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecKMin (Algorithm 8 in the paper).
 * In the paper:
 *     SecKMin([x_i],[l_i])

 * Invoked in:
 *     Algorithm 3 (SecKC), Line 2
 * ============================================================
 */
vector<vector<uint64_t>> secKMin(const vector<vector<uint64_t>> &triple_shares,
                                 vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                 vector<uint64_t> L_shares1, vector<uint64_t> L_shares2,
                                 const ShuffleMaterial &shuffleMaterial,
                                 Fss *fServer,
                                 ServerKeyLt *k0_lt, ServerKeyLt *k1_lt, // less than, compare to 1
                                 ServerKeyGt *k0_gt, ServerKeyGt *k1_gt, // greater than, compare to n-k-1
                                 ServerKeyEq *k0_eq, ServerKeyEq *k1_eq) // eq test, compare to 0
{
    uint64_t n = X_shares1.size();
    secShuffle(X_shares1, X_shares2, shuffleMaterial);
    secShuffle(L_shares1, L_shares2, shuffleMaterial);
    vector<uint64_t> delta_su(n, 0);
    vector<vector<uint64_t>> l_pi(2); // l_pi store the k-nearest labels' shares
    vector<vector<uint64_t>> su = secBLtCom(X_shares1, X_shares2, fServer, k0_lt, k1_lt);

    uint64_t ui0, ui1;
    for (size_t i = 0; i < n; i++)
    {
        delta_su[i] = su[0][i] - su[1][i];             // for fss shares, f(x)=f_1(x)-f_2(x), compare if dalta_su[i] > n-k-1
        ui0 = evaluateGt(fServer, k0_gt, delta_su[i]); // u_i_1
        ui1 = evaluateGt(fServer, k1_gt, delta_su[i]); // u_i_2
        uint64_t ui = ui0 - ui1;

        if (ui == 1)
        { // if ui=1, then store the label shares in l_pi, which is one of the k-min
            l_pi[0].push_back(L_shares1[i]);
            l_pi[1].push_back(L_shares2[i]);
        }
    }
    return l_pi;
}
vector<vector<uint64_t>> secKMin_parallel(const vector<vector<uint64_t>> &triple_shares,
                                          vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                          vector<uint64_t> L_shares1, vector<uint64_t> L_shares2,
                                          const ShuffleMaterial &shuffleMaterial,
                                          Fss *fServer,
                                          ServerKeyLt *k0_lt, ServerKeyLt *k1_lt, // less than, compare to 1
                                          ServerKeyGt *k0_gt, ServerKeyGt *k1_gt, // greater than, compare to n-k-1
                                          ServerKeyEq *k0_eq, ServerKeyEq *k1_eq) // eq test, compare to 0
{
    // cout<<"secKMin_parallel"<<endl;
    uint64_t n = X_shares1.size();
    vector<uint64_t> delta_su(n, 0);
    vector<vector<uint64_t>> l_pi(2); // l_pi store the k-nearest labels' shares
    const unsigned num_threads = 
        std::max(1u, std::min(6u, std::thread::hardware_concurrency()));
    // std::cout <<"#thread = "<< num_threads <<endl;
    vector<vector<uint64_t>> su = secBLtCom_parallel(X_shares1, X_shares2, fServer, k0_lt, k1_lt, num_threads);
    // /*num_threads=*/std::thread::hardware_concurrency());

    uint64_t ui0, ui1;
    for (size_t i = 0; i < n; i++)
    {
        delta_su[i] = su[0][i] - su[1][i];             // for fss shares, f(x)=f_1(x)-f_2(x), compare if dalta_su[i] > n-k-1
        ui0 = evaluateGt(fServer, k0_gt, delta_su[i]); // u_i_1
        ui1 = evaluateGt(fServer, k1_gt, delta_su[i]); // u_i_2
        uint64_t ui = ui0 - ui1;

        if (ui == 1)
        { // if ui=1, then store the label shares in l_pi, which is one of the k-min
            l_pi[0].push_back(L_shares1[i]);
            l_pi[1].push_back(L_shares2[i]);
        }
    }
    return l_pi;
}
