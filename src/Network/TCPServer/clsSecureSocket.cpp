#include "clsSecureSocket.h"
#include "log.h"
#ifdef USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

clsSecureSocket::clsSecureSocket()
{
    m_pServerSSlCtx = CreateSSlCtx();
    if(!m_pServerSSlCtx){

    }

    /*
    SSL_load_error_strings();
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    */
}

clsSecureSocket::~clsSecureSocket()
{
    FreeSSlCtx();
}

bool clsSecureSocket::setSSLMethod(SSLMethod value)
{
    bool ret =false;
    m_SSLMethod = value;
    const SSL_METHOD * method = NULL;
    switch (m_SSLMethod) {
    case SSLV1:
        method = TLSv1_server_method();
        break;
    case SSLV1_1:
        method = TLSv1_1_server_method();
        break;
    case SSLV1_2:
        method = TLSv1_2_server_method();
        break;
    case SSLV2_3:
        method = SSLv23_server_method();
        break;
    case SSLV3:
#ifdef OPENSSL_NO_SSL3_METHOD
        //method = SSLv3_server_method2();
#endif
        break;
    case SSLVAuto:
        method = TLS_server_method();  //highest available SSL/TLS version
        break;

    default:
        break;
    }

    if(m_pServerSSlCtx && method){
        ret = SSL_CTX_set_ssl_version(m_pServerSSlCtx, method);

        //SSL_CTX_set_verify_depth(m_pServerSSlCtx, 50);
        //SSL_CTX_set_options(m_pServerSSlCtx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3); // Use only TLS v1 or later.

        //LOG("SSL_CTX_set_ssl_version() %s", SSL_get_version(m_pServerSSlCtx));
    }
    return ret;
}

SSL_CTX* clsSecureSocket::CreateSSlCtx() {
    SSL_CTX* pSSLCtx = nullptr;
#ifdef USE_SSL
    pSSLCtx = SSL_CTX_new(TLS_server_method());

#endif
    return pSSLCtx;
}

void clsSecureSocket::FreeSSlCtx() {
#ifdef USE_SSL
    if (m_pServerSSlCtx) {
        SSL_CTX_free(m_pServerSSlCtx);
        m_pServerSSlCtx = NULL;
    }
#endif
}

bool clsSecureSocket::setSSLConfig(const char* CertPath, const char* KeyPath) {
#ifdef USE_SSL
    if (SSL_CTX_use_certificate_file(m_pServerSSlCtx, CertPath, SSL_FILETYPE_PEM) == -1) {
        DebugPrint("SSL_CTX_use_certificate_file() failed");
        return false;
    }

    if (SSL_CTX_use_PrivateKey_file(m_pServerSSlCtx, KeyPath, SSL_FILETYPE_PEM) == -1) {
        DebugPrint("SSL_CTX_use_PrivateKey_file() failed");
        return false;
    }

    if (SSL_CTX_check_private_key(m_pServerSSlCtx) == ISINVALID)
    {
        DebugPrint("SSL_CTX_check_private_key() failed");
        return false;
    }


    //SSL_CTX_set_ciphersuites(m_pServerSSlCtx, "TLS_CHACHA20_POLY1305_SHA256");

    // Enable server certificate verification. Enable before accepting connections.
    // SSL_CTX_set_verify(m_pServerSSlCtx, SSL_VERIFY_PEER, NULL);

    return true;
#else
    return false;
#endif
}

SSL* clsSecureSocket::newClientSSL(int fd) {
    SSL *pClientSSlCtx;
#ifdef USE_SSL
    pClientSSlCtx = SSL_new(m_pServerSSlCtx);
    if(pClientSSlCtx)
        SSL_set_fd(pClientSSlCtx, fd);
#endif
    return pClientSSlCtx;
}
