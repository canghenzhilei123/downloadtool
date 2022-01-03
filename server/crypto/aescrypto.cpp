#include "aescrypto.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

using namespace std;

string aespassword;

AesCrypto::AesCrypto()
{
    password = "12345678901234561234567890123456";
}

AesCrypto::~AesCrypto()
{

}

std::string AesCrypto::GenAesKey()
{
    std::string key;
    srand((unsigned)time(NULL));
    for(int i = 0; i < 10; i++ ) {
        key.push_back(rand() % 10);
    }
    return key;
}

std::string AesCrypto::aes_256_cbc_encode(const std::string& password, const std::string& data)
{
    // 这里默认将iv全置为字符0
    unsigned char iv[AES_BLOCK_SIZE] = { '0','0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };

    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)password.c_str(), password.length() * 8, &aes_key) < 0)
    {
        //assert(false);
        return "";
    }
    std::string strRet;
    std::string data_bak = data;
    unsigned int data_length = data_bak.length();

    // ZeroPadding
    int padding = 0;
    if (data_bak.length() % (AES_BLOCK_SIZE) > 0)
    {
        padding = AES_BLOCK_SIZE - data_bak.length() % (AES_BLOCK_SIZE);
    }
    // 在一些软件实现中，即使是16的倍数也进行了16长度的补齐
    /*else
    {
        padding = AES_BLOCK_SIZE;
    }*/

    data_length += padding;
    while (padding > 0)
    {
        data_bak += '\0';
        padding--;
    }

    for (unsigned int i = 0; i < data_length / (AES_BLOCK_SIZE); i++)
    {
        std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        ::memset(out, 0, AES_BLOCK_SIZE);
        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, AES_BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

std::string AesCrypto::aes_256_cbc_encode(const std::string& data)
{
    string encrypted = aes_256_cbc_encode(password, data);
    string b64encry;
    b64.Base64Encode((unsigned char *)encrypted.c_str(), encrypted.size(), b64encry);
    return b64encry;
}

std::string AesCrypto::aes_256_cbc_decode(const std::string& password, const std::string& strData)
{
    // 这里默认将iv全置为字符0
    unsigned char iv[AES_BLOCK_SIZE] = { '0','0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };

    AES_KEY aes_key;
    if (AES_set_decrypt_key((const unsigned char*)password.c_str(), password.length() * 8, &aes_key) < 0)
    {
        //assert(false);
        return "";
    }
    std::string strRet;
    for (unsigned int i = 0; i < strData.length() / AES_BLOCK_SIZE; i++)
    {
        std::string str16 = strData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        ::memset(out, 0, AES_BLOCK_SIZE);
        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, AES_BLOCK_SIZE, &aes_key, iv, AES_DECRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

std::string AesCrypto::aes_256_cbc_decode(std::string& strData)
{
    unsigned char *basede;
    int basedelen;
    basede = b64.Base64Decode(strData, basedelen);
    string b64destr(basede, basede + basedelen);
    free(basede);
    string decrypted = aes_256_cbc_decode(password, b64destr);
    return decrypted;
}

int AesCrypto::test()
{
    // 原始字符串
    string str = "hello aes";

    cout << "str(origin):" << str.c_str() << endl;

    // AES key
//    string key = "12345678901234561234567890123456";

//    string str_encode = aes_256_cbc_encode(key, str);
//    string str_decode = aes_256_cbc_decode(key, str_encode);

    string str_encode = aes_256_cbc_encode(str);
    cout << "str_encode:" << str_encode << endl;
    string str_decode = aes_256_cbc_decode(str_encode);

    //解密后的结果
    cout << "str_decode:" << str_decode.c_str() << endl;

    return 0;
}
