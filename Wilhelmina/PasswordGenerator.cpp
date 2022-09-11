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

#include "PasswordGenerator.h"
#include <openssl/rand.h>
#include <qmessagebox.h>

PasswordGenerator::PasswordGenerator(QWidget* parentWindow) {
    this->m_Parent = parentWindow;
}

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
            QMessageBox::critical(m_Parent, "Wilhelmina", "Failure generating random bytes. Abort.", QMessageBox::Ok);
            return max;
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

    max = strlen(alpha) - 1;

    for (int j = 0; j < len; j++)
    {
        number = randBetween(0, max);
        password = password + alpha[number];
    }    
    return password;
}