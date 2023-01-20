#ifndef CLSSECURESOCKET_H
#define CLSSECURESOCKET_H

#ifndef USE_SSL
class SSL_CTX;
class SSL;
#else
#include <openssl/types.h>
#endif

enum SSLMethod{
    SSLV1 = 1,
    SSLV1_1 = 2,
    SSLV1_2 = 3,
    SSLV2_3 = 4,
    SSLV3 = 5,
    SSLVAuto = 6,
};

class clsSecureSocket
{
private:
    SSLMethod m_SSLMethod;
    SSL_CTX *m_pServerSSlCtx;
    SSL_CTX *CreateSSlCtx();
    void FreeSSlCtx();
public:
    clsSecureSocket();
    ~clsSecureSocket();

    bool setSSLConfig(const char *CertPath, const char *KeyPath);
    SSL *newClientSSL(int fd);
    bool setSSLMethod(SSLMethod value);
};

#endif // CLSSECURESOCKET_H
