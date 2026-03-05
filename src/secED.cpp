 #include "../include/secknn.h"
/*
 * ============================================================
 * Implementation of SecED (Algorithm 4 in the paper).
 * In the paper:
 *     SecED([x], [y])

 * Invoked in:
 *     Algorithm 3 (SecKC), Line 1
 * ============================================================
 */

// (x * y) mod 2^64: compute in 128-bit then truncate low 64 bits
static inline uint64_t mul_mod_2_64(uint64_t x, uint64_t y) {
    return (uint64_t)((__uint128_t)x * (__uint128_t)y);
}

// (x + y) mod 2^64
static inline uint64_t add_mod_2_64(uint64_t x, uint64_t y) {
    return x + y; 
}

// (x - y) mod 2^64
static inline uint64_t sub_mod_2_64(uint64_t x, uint64_t y) {
    return x - y; 
}

// a data point with one dimension
vector<uint64_t> secED_1Dim(const vector<vector<uint64_t>> triple_shares,
                           uint64_t x_share1, uint64_t x_share2,
                           uint64_t y_share1, uint64_t y_share2)
{
    // keep  original triple indexing
    const uint64_t a1 = triple_shares[0][0];
    const uint64_t a2 = triple_shares[0][1];
    const uint64_t c1 = triple_shares[2][0];
    const uint64_t c2 = triple_shares[2][1];

    // e1 = (x1 - y1 - a1) mod 2^64
    // e2 = (x2 - y2 - a2) mod 2^64
    const uint64_t e1 = sub_mod_2_64(sub_mod_2_64(x_share1, y_share1), a1);
    const uint64_t e2 = sub_mod_2_64(sub_mod_2_64(x_share2, y_share2), a2);

    // open e = e1 + e2  (mod 2^64)
    const uint64_t e  = add_mod_2_64(e1, e2);

    // (e + a)^2 = e^2 + 2ea + a^2  in Z_{2^64}
    const uint64_t e_sq   = mul_mod_2_64(e, e);
    const uint64_t two_e  = add_mod_2_64(e, e);           // 2e mod 2^64
    const uint64_t two_e_a1 = mul_mod_2_64(two_e, a1);
    const uint64_t two_e_a2 = mul_mod_2_64(two_e, a2);

    vector<uint64_t> d = {0, 0};
    d[0] = add_mod_2_64(add_mod_2_64(e_sq, two_e_a1), c1);
    d[1] = add_mod_2_64(two_e_a2, c2);

    return d;
}

// a data point with high dimension
vector<uint64_t> secED_nDim(const vector<vector<uint64_t>> triple_shares,
                           vector<uint64_t> x_share1, vector<uint64_t> x_share2,
                           vector<uint64_t> y_share1, vector<uint64_t> y_share2)
{
    vector<uint64_t> d(2, 0);

    for (size_t i = 0; i < x_share1.size(); i++) {
        const vector<uint64_t> di =
            secED_1Dim(triple_shares, x_share1[i], x_share2[i], y_share1[i], y_share2[i]);

        d[0] = add_mod_2_64(d[0], di[0]);
        d[1] = add_mod_2_64(d[1], di[1]);
    }
    return d;
}
