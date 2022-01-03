#include "rsacrypto.h"
#include <stdlib.h>
#include <string.h>

RsaCrypto::RsaCrypto()
{
    padding = RSA_PKCS1_PADDING;
    publicKey = "-----BEGIN PUBLIC KEY-----\n" \
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n" \
        "ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n" \
        "vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n" \
        "fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n" \
        "i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n" \
        "PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n" \
        "wQIDAQAB\n" \
        "-----END PUBLIC KEY-----\n";
    privateKey = "-----BEGIN RSA PRIVATE KEY-----\n"\
        "MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
        "vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
        "Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
        "yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
        "WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
        "gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
        "omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
        "N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
        "X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
        "gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
        "vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
        "1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
        "m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
        "uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
        "JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
        "4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
        "WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
        "nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
        "PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
        "SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
        "I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
        "ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
        "yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
        "w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
        "uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
        "-----END RSA PRIVATE KEY-----\n";
}

int RsaCrypto::GenerateRSAKey(string strKey[])
{
    size_t pri_len;          // Length of private key
    size_t pub_len;          // Length of public key
    char *pri_key = NULL;           // Private key
    char *pub_key = NULL;           // Public key

    RSA* keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    pri_len = BIO_pending(pri);
    pub_len = BIO_pending(pub);

    pri_key = new char[pri_len + 1];
    pub_key = new char[pub_len + 1];

    BIO_read(pri, pri_key, pri_len);
    BIO_read(pub, pub_key, pub_len);

    pri_key[pri_len] = '\0';
    pub_key[pub_len] = '\0';

    strKey[0] = pub_key;
    strKey[1] = pri_key;

    //printf("\n%s\n%s\n", pri_key, pub_key);

    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
    delete [] pri_key;
    delete [] pub_key;

    return 0;
}

RSA* RsaCrypto::createRSA(unsigned char* key, int flag)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);

    if (keybio==NULL) {
        printf( "Failed to create key BIO");
        return 0;
    }

    if(flag)
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    else
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    if(rsa == NULL)
        printf( "Failed to create RSA");

    return rsa;
}

int RsaCrypto::public_encrypt(unsigned char* data, int data_len, unsigned char* key, unsigned char* encrypted)
{
    RSA * rsa = createRSA(key, 1);
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

int RsaCrypto::public_encrypt(unsigned char* data, int data_len)
{
    RSA * rsa = createRSA((unsigned char*)publicKey.c_str(), 1);
    unsigned char  encrypted[4098] = {};
    int len = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    b64.Base64Encode(encrypted, len, encryptedstr);
    return len;
}

int RsaCrypto::private_decrypt(unsigned char* enc_data, int data_len, unsigned char* key, unsigned char* decrypted)
{
    RSA * rsa = createRSA(key, 0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

int RsaCrypto::private_decrypt(string& data, unsigned char* decrypted)
{
    RSA * rsa = createRSA((unsigned char*)privateKey.c_str(), 0);
    unsigned char *basede;
    int basedelen;
    basede = b64.Base64Decode(data, basedelen);
    int  len = RSA_private_decrypt(basedelen, basede,decrypted,rsa,padding);
    free(basede);
    return len;
}

int RsaCrypto::private_encrypt(unsigned char* data, int data_len, unsigned char* key, unsigned char* encrypted)
{
    RSA * rsa = createRSA(key, 0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

int RsaCrypto::private_encrypt(string& data)
{
    RSA * rsa = createRSA((unsigned char*)privateKey.c_str(), 0);
    char  encrypted[4098] = {};
    int result = RSA_private_encrypt(data.size(), (unsigned char*)data.c_str(), (unsigned char*)encrypted, rsa, padding);
    encryptedstr = encrypted;
    return result;
}

int RsaCrypto::public_decrypt(unsigned char* enc_data, int data_len, unsigned char* key, unsigned char* decrypted)
{
    RSA * rsa = createRSA(key, 1);
    int  result = RSA_public_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

int RsaCrypto::public_decrypt(string& enc_data)
{
    RSA * rsa = createRSA((unsigned char*)publicKey.c_str(), 1);
    char decrypted[4098] = {};
    int  result = RSA_public_decrypt(enc_data.size(), (unsigned char*)enc_data.c_str(), (unsigned char*)decrypted, rsa, padding);
    decryptedstr = decrypted;
    return result;
}

void RsaCrypto::printLastError(const char *msg)
{
    char * err = (char*)malloc(130);;
    // ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n", msg, err);
    free(err);
}

void RsaCrypto::Rsa_Test()
{
    RsaCrypto rsa;
    unsigned char encrypted[4098] = {};
    unsigned char decrypted[4098] = {};

    string data = "hello world!";
    for (int i = 0; i < 10; i++) {
        rsa.public_encrypt((unsigned char*)data.c_str(), data.size());
        rsa.private_decrypt(rsa.encryptedstr, decrypted);
        cout << "decrypted:" << decrypted << endl;
        cout << "------------" << endl;
        cout << "------------" << endl;
        cout << "------------" << endl;
    }

//    for (int i = 0; i < 10; i++) {
//        int len = public_encrypt((unsigned char*)data.c_str(), data.size(), (unsigned char*)rsa.publicKey.c_str(), encrypted);
//        private_decrypt(encrypted, len, (unsigned char*)rsa.privateKey.c_str(), decrypted);
//        cout << "decrypted:" << decrypted << endl;
//        cout << "------------" << endl;
//        cout << "------------" << endl;
//        cout << "------------" << endl;
//    }
}

void RsaCrypto::RSA_test1(const string cleartext)
{
    //char plainText[2048/8] = "Hello this is Ravi"; //key length : 2048
    string plainText = cleartext;

    //下面的公钥和私钥是通过命令行方式获取的
    //char publicKey[]="-----BEGIN PUBLIC KEY-----\n"
    string publicKey = "-----BEGIN PUBLIC KEY-----\n" \
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n" \
        "ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n" \
        "vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n" \
        "fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n" \
        "i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n" \
        "PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n" \
        "wQIDAQAB\n" \
        "-----END PUBLIC KEY-----\n";

    //char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"
    string privateKey = "-----BEGIN RSA PRIVATE KEY-----\n"\
        "MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
        "vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
        "Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
        "yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
        "WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
        "gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
        "omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
        "N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
        "X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
        "gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
        "vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
        "1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
        "m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
        "uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
        "JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
        "4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
        "WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
        "nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
        "PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
        "SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
        "I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
        "ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
        "yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
        "w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
        "uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
        "-----END RSA PRIVATE KEY-----\n";

    unsigned char  encrypted[4098] = {};
    unsigned char decrypted[4098] = {};

    //int encrypted_length= public_encrypt((unsigned char*)plainText,strlen(plainText),(unsigned char*)publicKey,encrypted);
    int encrypted_length= public_encrypt((unsigned char*)plainText.c_str(), plainText.length(), (unsigned char*)publicKey.c_str(), encrypted);
    if (encrypted_length == -1) {
        printLastError("Public Encrypt failed ");
        exit(0);
    }
    printf("Encrypted length =%d\n", encrypted_length);

    int decrypted_length = private_decrypt(encrypted, encrypted_length, (unsigned char*)privateKey.c_str(), decrypted);
    if (decrypted_length == -1) {
        printLastError("Private Decrypt failed ");
        exit(0);
    }
    printf("Decrypted Text =%s\n", decrypted);
    printf("Decrypted Length =%d\n", decrypted_length);

    encrypted_length= private_encrypt((unsigned char*)plainText.c_str(), plainText.length(),(unsigned char*)privateKey.c_str(), encrypted);
    if(encrypted_length == -1) {
        printLastError("Private Encrypt failed");
        exit(0);
    }
    printf("Encrypted length =%d\n", encrypted_length);

    decrypted_length = public_decrypt(encrypted, encrypted_length, (unsigned char*)publicKey.c_str(), decrypted);
    if(decrypted_length == -1) {
        printLastError("Public Decrypt failed");
        exit(0);
    }
    printf("Decrypted Text =%s\n", decrypted);
    printf("Decrypted Length =%d\n", decrypted_length);
}

void RsaCrypto::RSA_test2(const string cleartext)
{
//     size_t pri_len;            // Length of private key
//     size_t pub_len;            // Length of public key
//     char   *pri_key;           // Private key
//     char   *pub_key;           // Public key
//     char   msg[KEY_LENGTH/8];  // Message to encrypt
//     char   *encrypt = NULL;    // Encrypted message
//     char   *decrypt = NULL;    // Decrypted message
//     char   *err;               // Buffer for any error messages
//     string str;

//     // Generate key pair
//     //printf("Generating RSA (%d bits) keypair...", KEY_LENGTH);
//     fflush(stdout);
//     RSA *keypair = RSA_generate_key(KEY_LENGTH, PUB_EXP, NULL, NULL);

//     // To get the C-string PEM form:
//     BIO *pri = BIO_new(BIO_s_mem());
//     BIO *pub = BIO_new(BIO_s_mem());

//     PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
//     PEM_write_bio_RSAPublicKey(pub, keypair);

//     pri_len = BIO_pending(pri);
//     pub_len = BIO_pending(pub);

//     pri_key = (char*)malloc(pri_len + 1);
//     pub_key = (char*)malloc(pub_len + 1);

//     BIO_read(pri, pri_key, pri_len);
//     BIO_read(pub, pub_key, pub_len);

//     pri_key[pri_len] = '\0';
//     pub_key[pub_len] = '\0';

// #ifdef PRINT_KEYS
//     printf("\n%s\n%s\n", pri_key, pub_key);
// #endif
//     //printf("done.\n");

//     // Get the message to encrypt
//     printf("Message to encrypt: ");
//     fgets(msg, KEY_LENGTH-1, stdin);
//     msg[strlen(msg)-1] = '\0';
//     //string msg = cleartext;
//     //cout<<"length:"<<strlen(msg)<<endl;

//     // Encrypt the message
//     encrypt = (char*)malloc(RSA_size(keypair));
//     int encrypt_len;
//     err = (char*)malloc(130);
//     if((encrypt_len = RSA_public_encrypt(strlen(msg)+1, (unsigned char*)msg, (unsigned char*)encrypt, keypair, RSA_PKCS1_OAEP_PADDING)) == -1) {
//     //if((encrypt_len = RSA_public_encrypt(msg.length(), (unsigned char*)msg.c_str(), (unsigned char*)encrypt, keypair, RSA_PKCS1_OAEP_PADDING)) == -1) {
//         ERR_load_crypto_strings();
//         ERR_error_string(ERR_get_error(), err);
//         fprintf(stderr, "Error encrypting message: %s\n", err);
//         goto free_stuff;
//     }

// #ifdef WRITE_TO_FILE
//     // Write the encrypted message to a file
//     FILE *out = fopen("out.bin", "w");
//     fwrite(encrypt, sizeof(*encrypt),  RSA_size(keypair), out);
//     fclose(out);
//     printf("Encrypted message written to file.\n");
//     free(encrypt);
//     encrypt = NULL;

//     // Read it back
//     printf("Reading back encrypted message and attempting decryption...\n");
//     encrypt = (char*)malloc(RSA_size(keypair));
//     out = fopen("out.bin", "r");
//     fread(encrypt, sizeof(*encrypt), RSA_size(keypair), out);
//     fclose(out);
// #endif

//     // Decrypt it
//     decrypt = (char*)malloc(encrypt_len);
//     //decrypt = new char[encrypt_len + 1];
//     //memset(decrypt, encrypt_len+1, 0);

//     if(RSA_private_decrypt(encrypt_len, (unsigned char*)encrypt, (unsigned char*)decrypt, keypair, RSA_PKCS1_OAEP_PADDING) == -1) {
//             // ERR_load_crypto_strings();
//             ERR_error_string(ERR_get_error(), err);
//             fprintf(stderr, "Error decrypting message: %s\n", err);
//             goto free_stuff;
//     }
//     printf("Decrypted message: %s\n", decrypt);

// free_stuff:
//     RSA_free(keypair);
//     BIO_free_all(pub);
//     BIO_free_all(pri);
//     free(pri_key);
//     free(pub_key);
//     free(encrypt);
//     free(decrypt);
//     free(err);
}
