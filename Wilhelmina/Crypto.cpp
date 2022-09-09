/*
* Wilhelmina
*
* Copyright (C) 2022  Niko Rosvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
//#include <vcruntime_string.h>
#include <string.h>

const int KEY_SIZE = 32;
const int SALT_SIZE = 64;
const int IV_SIZE = 12;
const int TAG_SIZE = 16;

int Crypto::ivSize() {
    return IV_SIZE;
}

int Crypto::keySize() {
    return KEY_SIZE;
}

int Crypto::saltSize() {
    return SALT_SIZE;
}

int Crypto::tagSize() {
    return TAG_SIZE;
}

//Generate key from passphrase. If oldsalt is NULL, new salt is created.
//ok is set to true on success, false on failure

Key Crypto::generate_key(const char* passphrase, char* old_salt, bool* ok) {
    unsigned char* salt = NULL;
    int iterations = 200000;
    Key key = { {0} };
    int success;
    char* resultbytes[KEY_SIZE];

    if (old_salt == NULL) {
        salt = (unsigned char*)malloc(SALT_SIZE * sizeof(char));

        if (!salt) {
            *ok = false;
            return key;
        }

        RAND_bytes(salt, SALT_SIZE);
    }
    else
    {
        salt = (unsigned char *)malloc(SALT_SIZE * sizeof(char));

        if (!salt) {
            *ok = false;
            return key;
        }

        memmove(salt, old_salt, SALT_SIZE);
    }

    if (!salt)
    {
        *ok = false;
        return key;
    }

    success = PKCS5_PBKDF2_HMAC(passphrase, strlen(passphrase), (unsigned char*)salt,
        SALT_SIZE, iterations, EVP_sha256(),
        KEY_SIZE, (unsigned char*)resultbytes);

    if (success == 0)
    {
        free(salt);
        *ok = false;
        return key;
    }

    memmove(key.data, resultbytes, KEY_SIZE);
    memmove(key.salt, salt, SALT_SIZE);

    free(salt);
    *ok = true;

    return key;
}

void Crypto::genData(unsigned char* in, int len) {
    RAND_bytes(in, len);
}

int Crypto::encryptData( unsigned char* plaintext,
                         int plaintext_len,
                         unsigned char* iv,
                         unsigned char* key,
                         unsigned char* ciphertext,
                         unsigned char* tag) {

    EVP_CIPHER_CTX* ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    ciphertext_len += len;

    //Get our tag
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int Crypto::decryptData(unsigned char* ciphertext,
                        int ciphertext_len,
                        unsigned char* tag,
                        unsigned char* key,
                        unsigned char* iv,
                        int iv_len,
                        unsigned char* plaintext) {

    EVP_CIPHER_CTX* ctx;
    int len;
    int plaintext_len;
    int ret;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    plaintext_len = len;

    //Set our tag
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    //Finalise the decryption. A positive return value indicates success,
    //anything else is a failure - the plaintext is not trustworthy.
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        plaintext_len += len;
        return plaintext_len;
    }
    else {
        return -1;
    }
}
