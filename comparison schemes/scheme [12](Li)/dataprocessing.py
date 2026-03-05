import csv
import random
MODULUS = 2**64
def gen_additive_shares(secret, num_shares, prime=MODULUS):
    shares = []
    for _ in range(num_shares - 1):
        share = random.randint(0, prime - 1)
        shares.append(share)
        secret -= share
    shares.append(secret % prime)
    return shares

def rec_additive_secret(shares, prime=MODULUS):
    secret = sum(shares) % prime
    return secret

def generate_beaver_triple(num_shares, prime=MODULUS):
    a = random.randint(0, prime - 1)
    b = random.randint(0, prime - 1)

    # c = ab
    c = (a * b) % prime
    # generate shares of a, b, c
    shares_a = gen_additive_shares(a, num_shares, prime)
    shares_b = gen_additive_shares(b, num_shares, prime)
    shares_c = gen_additive_shares(c, num_shares, prime)

    return shares_a, shares_b, shares_c

def read_csv_dataset(file_path):
    features = []
    labels = []
    with open(file_path, 'r', newline='') as file:
        csv_reader = csv.reader(file)
        for row in csv_reader:
            labels.append(int(row[0]))  # Convert the first element of each row to integer and append to labels
            features.append([int(x) for x in row[1:]])  # Convert the remaining elements to integers and append to features
    return features, labels

def share_dataset(features,labels):
    shared_features_list = []
    shared_labels_list = []
    for i in range(len(features)):
        shared_features = []
        for j in range(len(features[0])):
            shared_features.append(gen_additive_shares(features[i][j], 2, MODULUS))
        shared_features_list.append(shared_features)
        shared_labels_list.append(gen_additive_shares(labels[i],2,MODULUS))
    return shared_features_list,shared_labels_list
# test
# file_path = 'dataset/test100-40.csv' 
# features, labels = read_csv_dataset(file_path)
# shared_features_list,shared_labels_list = share_dataset(features,labels)
#shared_features_list[0] shared features of data 1,shared_features_list[0][0]:data 1's first feature's shares
# print('data 1\' shared features:',shared_features_list[0])
# print((shared_features_list[0][0][0]+shared_features_list[0][0][1])%MODULUS)

