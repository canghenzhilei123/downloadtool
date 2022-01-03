#ifndef BASE64_H
#define BASE64_H

#include <string>

using namespace std;

class Base64
{
public:
    char * Base64Encode(const char * input, int length, bool with_new_line);
    void Base64Encode(const unsigned char * input, int length, string& output);
    char * Base64Decode(char * input, int length, bool with_new_line);
    unsigned char * Base64Decode(string& input, int& outlen);
    void Test();
};

#endif // BASE64_H
