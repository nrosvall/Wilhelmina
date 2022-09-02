#pragma once

#include <qstring.h>

class PasswordGenerator
{
private:
	unsigned int randBetween(unsigned int min, unsigned int max);
public:
	QString generatePassword(unsigned int len);
};
