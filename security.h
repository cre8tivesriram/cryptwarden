#ifndef SECURITY_H
#define SECURITY_H

#define SALT_SIZE 16
#define IV_SIZE 16
#define HASH_SIZE 32
#define ITERATIONS 100000
#define KEY_TYPE_PASSWORD 0x01
#define KEY_TYPE_SEED     0x02

int hashPasswordPBKDF2(const char *password, const unsigned char *salt, unsigned char *outHash);
int generateSalt(unsigned char *salt);
int encryptData(const unsigned char *plaintext, int plaintext_len,
                const unsigned char *key, const unsigned char *iv,
                unsigned char *ciphertext);
int decryptData(const unsigned char *ciphertext, int ciphertext_len,
                const unsigned char *key, const unsigned char *iv,
                unsigned char *plaintext);

#endif //SECURITY_H
