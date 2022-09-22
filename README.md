# Wilhelmina - Offline Password Manager

Wilhelmina is a free software (GPLv3+) password manager written in C++. Wilhelmina is meant to be easy
to use, yet very secure. 

Wilhelmina is developed on and for Windows but should be easily portable for other operating systems.

Wilhelmina is for people who want to keep their passwords locally but also to be able to seamlessly access 
them from every computer they use. SSH syncing works automatically once it's configured and even if you loose 
your local copy of the data, Wilhelmina is clever enough to try to find it from the SSH server.

Wilhelmina tries to keep it simple without unnecessary feature bloat.

## Encryption

Wilhelmina uses OpenSSL crypto library implementation of AES GCM algorithm with 256 bit keys. Key is derived using
PBKDF2 algorithm with 200 000 iterations.

Decrypted data never touches the hard drive.

## SFTP support

Wilhelmina has support for data synchronization using SFTP. Only encrypted data is synchronized so remote servers
will not know anything about the data.

## Homepage

For more information, documentation and downloads see <https://www.byteptr.com/wilhelmina>.