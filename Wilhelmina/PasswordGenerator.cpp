#include "PasswordGenerator.h"
#include <openssl/rand.h>

unsigned int PasswordGenerator::randBetween(unsigned int min, unsigned int max) {
    uint32_t r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /*Create equal size buckets all in a row, then fire randomly towards
     *the buckets until you land in one of them. All buckets are equally
     *likely. If you land off the end of the line of buckets, try again.
     */
    do
    {
        if (RAND_bytes((unsigned char*)&r, sizeof r) == 0)
        {
            //TODO: Handle this, simply a messagebox?
        }

    } while (r >= limit);

    return min + (r / buckets);
}

QString PasswordGenerator::generatePassword(unsigned int len) {

    if (len < 1 || len > RAND_MAX)
        return nullptr;

    QString password;
    const char* alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?)(/%#!?)=";
    unsigned int max;
    unsigned int number;

    RAND_poll();

    max = strlen(alpha) - 1;

    for (int j = 0; j < len; j++)
    {
        number = randBetween(0, max);
        password = password + alpha[number];
    }    
    return password;
}