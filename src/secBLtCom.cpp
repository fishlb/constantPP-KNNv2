#include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of the "less-than" branch of SecBCom (Algorithm 7 in the paper).
 * In the paper:
 *     SecBCom([x_i], less)

 * Invoked in:
 *     Algorithm 8 (SecKMin), Line 2 (Online)
 * ============================================================
 */
#include <vector>
#include <cstdint>

using namespace std;

vector<vector<uint64_t>> secBLtCom(vector<uint64_t> X_shares1, vector<uint64_t> X_shares2,
                                   Fss *fServer, ServerKeyLt *k0, ServerKeyLt *k1)
{
    uint64_t n = X_shares1.size();
    vector<uint64_t> su1(n, 0);
    vector<uint64_t> su2(n, 0);
    vector<vector<uint64_t>> su(2, vector<uint64_t>(n, 0)); 
    
    
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = i + 1; j < n; j++) 
        {
            uint64_t delta_ij=2;
            uint64_t an0ij_lt = 0, an1ij_lt = 0;
            uint64_t an0ji_lt = 0, an1ji_lt = 0;
            uint64_t xi = X_shares1[i] + X_shares2[i];
            uint64_t xj = X_shares1[j] + X_shares2[j];
            
            if (xi <= xj) delta_ij = 0;
            an0ij_lt = evaluateLt(fServer, k0, delta_ij); 
            an1ij_lt = evaluateLt(fServer, k1, delta_ij);

            su1[i] += an0ij_lt;
            su2[i] += an1ij_lt;


            an0ji_lt = 1 + an1ij_lt;
            an1ji_lt = an0ij_lt;

            su1[j] += an0ji_lt;
            su2[j] += an1ji_lt;
        }
    }    
    su[0] = su1;
    su[1] = su2;
    return su; 
}

static inline uint64_t add_u64(uint64_t a, uint64_t b) {
    return a + b; // mod 2^64
}

// Parallelized version of secBLtCom
vector<vector<uint64_t>> secBLtCom_parallel(
    const vector<uint64_t>& X_shares1,
    const vector<uint64_t>& X_shares2,
    Fss* fServer,
    ServerKeyLt* k0,
    ServerKeyLt* k1,
    unsigned num_threads
) {
    const size_t n = X_shares1.size();
    vector<vector<uint64_t>> su(2, vector<uint64_t>(n, 0));
    if (n == 0) return su;

    vector<uint64_t> x(n);
    for (size_t i = 0; i < n; i++)
        x[i] = X_shares1[i] + X_shares2[i];

    mutex global_mtx;
    atomic<size_t> next_i{0};

    auto worker = [&]() {
        while (true) {
            size_t i = next_i.fetch_add(1, memory_order_relaxed);
            if (i >= n) break;

            const uint64_t xi = x[i];
            for (size_t j = i + 1; j < n; j++) {
                const uint64_t xj = x[j];

                uint64_t delta_ij = 2;
                if (xi <= xj) delta_ij = 0;

                uint64_t an0ij = evaluateLt(fServer, k0, delta_ij);
                uint64_t an1ij = evaluateLt(fServer, k1, delta_ij);

                uint64_t an0ji = 1 + an1ij;
                uint64_t an1ji = an0ij;

                lock_guard<mutex> lk(global_mtx);
                su[0][i] = add_u64(su[0][i], an0ij);
                su[1][i] = add_u64(su[1][i], an1ij);
                su[0][j] = add_u64(su[0][j], an0ji);
                su[1][j] = add_u64(su[1][j], an1ji);
            }
        }
    };

    vector<thread> threads;
    threads.reserve(num_threads);
    for (unsigned t = 0; t < num_threads; t++)
        threads.emplace_back(worker);
    for (auto& th : threads)
        th.join();

    return su;
}
