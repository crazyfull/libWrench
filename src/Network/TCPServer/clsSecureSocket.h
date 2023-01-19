#ifndef CLSSECURESOCKET_H
#define CLSSECURESOCKET_H

#ifndef USE_SSL
class SSL_CTX;
class SSL;
#else
#include <openssl/types.h>
#endif

class clsSecureSocket
{
private:
    SSL_CTX *m_pServerSSlCtx;
    SSL_CTX *CreateSSlCtx();
    void FreeSSlCtx();
public:
    clsSecureSocket();
    ~clsSecureSocket();

    bool setSSLConfig(const char *CertPath, const char *KeyPath);
    SSL *newClientSSL(int fd);
};

#endif // CLSSECURESOCKET_H
