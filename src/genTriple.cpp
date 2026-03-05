#include "../include/secknn.h"
// generate Beaver triple
vector<vector<uint64_t>> generate_beaver_triple(int num_participants, bool flag = DEFAULT_MOD)
{
    vector<vector<uint64_t>> triple_shares(3, vector<uint64_t>(num_participants, 0));
    uint64_t a, b, c;
    vector<uint64_t> a_shares, b_shares, c_shares;

    if (flag)
    {
        a = rand();
        b = rand();
        c = a * b;
        a_shares = additive_secret_sharing(a, num_participants);
        b_shares = additive_secret_sharing(b, num_participants);
        c_shares = additive_secret_sharing(c, num_participants);

        for (int i = 0; i < num_participants; ++i)
        {
            triple_shares[0][i] = a_shares[i]; 
            triple_shares[1][i] = b_shares[i]; 
            triple_shares[2][i] = c_shares[i]; 
        }
    }
    else
    {
        a = rand();
        b = a;
        c = a * b;
        a_shares = additive_secret_sharing(a, num_participants);
        b_shares = a_shares;
        c_shares = additive_secret_sharing(c, num_participants);

        for (int i = 0; i < num_participants; ++i)
        {
            triple_shares[0][i] = a_shares[i]; 
            triple_shares[1][i] = b_shares[i]; 
            triple_shares[2][i] = c_shares[i]; 
        }
    }
    return triple_shares;
}
//triple for mpz_class type
vector<vector<mpz_class>> generate_beaver_triple_mpz(int num_participants, bool flag)
{
    vector<vector<mpz_class>> triple_shares(3, vector<mpz_class>(num_participants, 0));
    mpz_class a, b, c;
    vector<mpz_class> a_shares, b_shares, c_shares;
    if (flag)
    {
        a = rand();
        b = rand();
        c = a * b;
        a_shares = additive_secret_sharing_mpz(a, num_participants);
        b_shares = additive_secret_sharing_mpz(b, num_participants);
        c_shares = additive_secret_sharing_mpz(c, num_participants);

        for (int i = 0; i < num_participants; ++i)
        {
            triple_shares[0][i] = a_shares[i];
            triple_shares[1][i] = b_shares[i]; 
            triple_shares[2][i] = c_shares[i]; 
        }
    }
    else
    {
        a = rand();
        b = a;
        c = a * b;
        a_shares = additive_secret_sharing_mpz(a, num_participants);
        b_shares = a_shares;
        c_shares = additive_secret_sharing_mpz(c, num_participants);

        for (int i = 0; i < num_participants; ++i)
        {
            triple_shares[0][i] = a_shares[i]; 
            triple_shares[1][i] = b_shares[i]; 
            triple_shares[2][i] = c_shares[i];
        }
    }

    return triple_shares;
}
