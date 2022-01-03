#ifndef AESCRYPTO_H
#define AESCRYPTO_H

#include <string>
#include "base64.h"

// https://blog.csdn.net/GerZhouGengCheng/article/details/106103039?ops_request_misc=&request_id=&biz_id=102&utm_term=c++%20aes%20openssl&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-5-106103039.pc_search_em_sort&spm=1018.2226.3001.4187
class AesCrypto
{
public:
    Base64 b64;
    string password;

    AesCrypto();
    ~AesCrypto();
    std::string static GenAesKey();
    std::string aes_256_cbc_encode(const std::string& password, const std::string& data);
    std::string aes_256_cbc_encode(const std::string& data);
    std::string aes_256_cbc_decode(const std::string& password, const std::string& strData);
    std::string aes_256_cbc_decode(std::string& strData);
    int test();
private:
};

extern string aespassword;

#endif // AESCRYPTO_H
