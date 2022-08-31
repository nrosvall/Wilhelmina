#pragma once

struct Key
{
    char data[32];
    char salt[64];

};

class Crypto
{
private:


public:
    int encryptData(unsigned char* plaintext,
                    int plaintext_len,
                    unsigned char* key,
                    unsigned char* ciphertext,
                    unsigned char* tag);


    int decryptData(unsigned char* ciphertext,
                    int ciphertext_len,
                    unsigned char* tag,
                    unsigned char* key,
                    unsigned char* iv,
                    int iv_len,
                    unsigned char* plaintext);

    Key generate_key(const char* passphrase, char* old_salt, bool *ok);
};

