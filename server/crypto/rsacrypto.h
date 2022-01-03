#ifndef RSACRYPTO_H
#define RSACRYPTO_H

#include "openssl/err.h"
#include "openssl/pem.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "base64.h"

#define KEY_LENGTH  2048
#define PUB_EXP     3
//#define PRINT_KEYS
//#define WRITE_TO_FILE

// https://blog.csdn.net/fengbingchun/article/details/43638013

using namespace std;

typedef enum {
    GENERAL = 0,
    ECB,
    CBC,
    CFB,
    OFB,
    TRIPLE_ECB,
    TRIPLE_CBC
}CRYPTO_MODE;

class RsaCrypto
{
public:
    string publicKey;
    string privateKey;
    string encryptedstr;
    string decryptedstr;
    int padding;
    Base64 b64;


    explicit RsaCrypto();
    int GenerateRSAKey(string strKey[]);
    RSA* createRSA(unsigned char* key, int flag);
    int public_encrypt(unsigned char* data, int data_len, unsigned char* key, unsigned char* encrypted);
    int private_decrypt(unsigned char* enc_data, int data_len, unsigned char* key, unsigned char* decrypted);
    int private_encrypt(unsigned char* data, int data_len, unsigned char* key, unsigned char* encrypted);
    int public_decrypt(unsigned char* enc_data, int data_len, unsigned char* key, unsigned char* decrypted);
    void printLastError(const char *msg);
    void Rsa_Test();
    void RSA_test1(const string cleartext);
    void RSA_test2(const string cleartext);

    int public_encrypt(unsigned char* data, int data_len);
    int private_decrypt(string& data, unsigned char* decrypted);
    string private_decrypt(string& data);
    int private_encrypt(string& data);
    int public_decrypt(string& enc_data);

};

#endif // RSACRYPTO_H
