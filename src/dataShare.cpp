#include "../include/secknn.h"
//generate secret sharing (SS) of a secret
vector<uint64_t> additive_secret_sharing(uint64_t secret, int num_participants)
{
    vector<uint64_t> shares(num_participants, 0); 

    for (int i = 0; i < num_participants - 1; ++i)
    {
        if (secret == 0)
        {
            uint64_t random_share = rand();
            shares[i] = random_share;
            secret -= random_share; 
        }
        else
        {
            uint64_t random_share = rand() % secret; // generate the random part
            shares[i] = random_share;
            secret -= random_share; // update the remaining
        }
    }
    shares[num_participants - 1] = secret; // for the last party

    return shares;
}
// SS for n data
vector<vector<uint64_t>> additive_secret_sharing(vector<uint64_t> secret, int num_participants){
    vector<vector<uint64_t>> n_shares(2);
    for (size_t i = 0; i < secret.size(); i++)
    {
        vector<uint64_t> shares = additive_secret_sharing(secret[i], NUM_PARTIES);
        n_shares[0].push_back(shares[0]);
        n_shares[1].push_back(shares[1]);
    }
    return n_shares;
}

//SS for mpz_class type value
vector<mpz_class> additive_secret_sharing_mpz(mpz_class secret, int num_participants)
{
    vector<mpz_class> shares(num_participants, 0); 

    for (int i = 0; i < num_participants - 1; ++i)
    {
        if (secret == 0)
        {
            mpz_class random_share = rand();
            shares[i] = random_share;
            secret -= random_share; 
        }
        else
        {
            mpz_class random_share = rand() % secret;
            shares[i] = random_share;
            secret -= random_share; 
        }
    }
    shares[num_participants - 1] = secret; 

    return shares;
}

vector<DataPointShares> getSharedData(vector<DataPoint> data){//each point with n dimension features and one label
    vector<DataPointShares> data_shares;
     for (const auto& point : data) { 
        DataPointShares point_i;
        point_i.features_shares = additive_secret_sharing(point.features, NUM_PARTIES);//n features' shares
        point_i.target_shares = additive_secret_sharing(point.label, NUM_PARTIES);//one label's shares
        data_shares.push_back(point_i);
    }
    return data_shares;//n shared data,(each of them with two feature[n dimension] shares and label shares)
}