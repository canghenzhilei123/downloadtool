#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string.h>
#include <string>
#include <iostream>
#include "base64.h"

using namespace std;

char * Base64::Base64Encode(const char * input, int length, bool with_new_line)
{
    BIO * bmem = NULL;
    BIO * b64 = NULL;
    BUF_MEM * bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    if(!with_new_line) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char * buff = (char *)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;

    BIO_free_all(b64);

    return buff;
}

void Base64::Base64Encode(const unsigned char * input, int length, string& output)
{
    BIO * bmem = NULL;
    BIO * b64 = NULL;
    BUF_MEM * bptr = NULL;

    b64 = BIO_new(BIO_f_base64());

    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char * buff = (char *)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;

    output = buff;
    free(buff);
    BIO_free_all(b64);
}

char * Base64::Base64Decode(char * input, int length, bool with_new_line)
{
    BIO * b64 = NULL;
    BIO * bmem = NULL;
    char * buffer = (char *)malloc(length);
    memset(buffer, 0, length);

    b64 = BIO_new(BIO_f_base64());
    if(!with_new_line) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length); // 这里有bug，长度等于返回值，不是strlen(buffer)

    BIO_free_all(bmem);

    return buffer;
}

unsigned char * Base64::Base64Decode(string& input, int& outlen)
{
    BIO * b64 = NULL;
    BIO * bmem = NULL;
    unsigned char * buffer = (unsigned char *)malloc(input.size());
    memset(buffer, 0, input.size());

    b64 = BIO_new(BIO_f_base64());

    bmem = BIO_new_mem_buf(input.c_str(), input.size());
    bmem = BIO_push(b64, bmem);
    outlen = BIO_read(bmem, buffer, input.size());

    BIO_free_all(bmem);

    return buffer;
}

void Base64::Test()
{
    bool with_new_line = false;

    string enc_input = "Henry Alfred Kissinger is a German-born American writer, political scientist, diplomat, and businessman. A recipient of the Nobel Peace Prize, he served as National Security Advisor and later concurrently as Secretary of State in the administrations of Presidents Richard Nixon and Gerald Ford.";

    cout << endl << "To be encoded:" << endl << "~" << enc_input << "~" << endl << endl;

    char * enc_output = Base64Encode(enc_input.c_str(), enc_input.length(), with_new_line);
    cout << "Base64 Encoded:" << endl << "~" << enc_output << "~" << endl << endl;

    string dec_input = enc_output;
    char * dec_output = Base64Decode((char *)dec_input.c_str(), dec_input.length(), with_new_line);
    cout << "Base64 Decoded:" << endl << "~" << dec_output << "~" << endl << endl;

    free(enc_output);
    free(dec_output);
}
