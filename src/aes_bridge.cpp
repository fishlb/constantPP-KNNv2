#include <openssl/aes.h>

// -> openssl-aes.h 
extern "C" {
    void aesni_set_encrypt_key(const unsigned char *userKey, int bits, AES_KEY *key) {
        AES_set_encrypt_key(userKey, bits, key);
    }
    void aesni_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key) {
        AES_encrypt(in, out, key);
    }
}