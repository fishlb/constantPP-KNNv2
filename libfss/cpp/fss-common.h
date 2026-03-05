#ifndef FSS_COMMON_H
#define FSS_COMMON_H


#include "openssl-aes.h"

#include <cpuid.h>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdint.h>
#include <string>

using namespace std;

const int initPRFLen = 4;

struct Fss {
    // store keys in fixedBlocks
    AES_KEY* aes_keys;
    uint32_t numBits; // number of bits in domain 
    mpz_class prime;
    uint32_t numParties; // used only in multiparty. Default is 3
    uint32_t numKeys;
};

struct CWEq {
    unsigned char cs[2][16];
    unsigned char ct[2];
};

struct CWLt //56bytes
{//在很多情况下，编译器会对结构体进行内存对齐以提高访问效率。这意味着结构体的大小可能会被调整为某个特定的值的倍数，而不仅仅是其成员变量所占内存大小的总和。
    unsigned char cs[2][16];//32 bytes
    unsigned char ct[2];//2 bytes
    uint64_t cv[2]; //16 bytes
};

struct ServerKeyEq {
    unsigned char s[2][16];
    unsigned char t[2];
    CWEq* cw[2];
    mpz_class w;
};

struct ServerKeyLt {//total 66? but 
    unsigned char s[2][16];//32 bytes
    unsigned char t[2];//2 bytes
    uint64_t v[2];//16 bytes
    CWLt* cw[2];//2*8 = bytes?
};
struct ServerKeyGt {//total 66? but 
    unsigned char s[2][16];//32 bytes
    unsigned char t[2];//2 bytes
    uint64_t v[2];//16 bytes
    CWLt* cw[2];//2*8 = bytes?
};
// struct ServerKeyLt2 {//total 50+2*?
//     unsigned char s[2][16];//32 bytes
//     unsigned char t[2];//2 bytes
//     uint64_t v[2];//16 bytes
//     // CWLt* cw[2];//2* = bytes?
// };

struct MPLtKey {
    unsigned char*** s;
    uint32_t** aValue;
    uint32_t** cw;
    uint32_t* wVal;
};

struct MPKey {
    unsigned char** sigma;
    uint32_t** cw;
};

// Assumes integers are 64 bits
inline int getBit(uint64_t n, uint64_t pos) {
    return (n & ( 1 << (64-pos))) >> (64-pos);
}

// Converts byte array into 64-bit integer
inline uint64_t byteArr2Int64(unsigned char* arr)
{
    uint64_t i = ((unsigned long) arr[7] << 56) | ((unsigned long)arr[6] << 48) | ((unsigned long)arr[5] << 40) |
                ((unsigned long) arr[4] << 32) | ((unsigned long) arr[3] << 24) |
                ((unsigned long) arr[2] << 16) | ((unsigned long) arr[1] << 8) | ((unsigned long) arr[0]);
    return i;
}

// Converts byte array into 32-bit integer
inline uint32_t byteArr2Int32(unsigned char* arr)
{
    uint32_t a = uint32_t((unsigned char)(arr[0]) << 24 |
            (unsigned char)(arr[1]) << 16 |
            (unsigned char)(arr[2]) << 8 |
            (unsigned char)(arr[3]));
    return a;
}

AES_KEY* prf(unsigned char* out, unsigned char* key, uint64_t in_size, AES_KEY* aes_keys, uint32_t numKeys);

#endif
