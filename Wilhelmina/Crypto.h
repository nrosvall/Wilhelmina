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
                    unsigned char* iv,
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
    void genData(unsigned char* in, int len);
    int ivSize();
    int keySize();
    int saltSize();
    int tagSize();
};

