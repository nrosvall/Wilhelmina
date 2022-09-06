# Wilhelmina - Offline Password Manager

Wilhelmina is a free software (GPLv3+) password manager written in C++. Wilhelmina is meant to be easy
to use, yet very secure. At the moment it's still under heavy development and there are no official releases 
available. First release (1.0) will follow on early October 2022.

Wilhelmina is developed on and for Windows but should be easily portable for other operating systems. Android version
will follow later this year.

## Encryption

Wilhelmina uses OpenSSL crypto library implementation of AES GCM algorithm with 256 bit keys. Key is derived using
PBKDF2 algorithm with 200 000 iterations.

Decrypted data never touches the hard drive.

## SFTP support

Wilhelmina has support for data synchronization using SFTP. Only encrypted data is synchronized so remote servers
will not know anything about the data.