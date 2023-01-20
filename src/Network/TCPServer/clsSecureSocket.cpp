#include "clsSecureSocket.h"
#include "log.h"
#ifdef USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

clsSecureSocket::clsSecureSocket()
{
    m_pServerSSlCtx = CreateSSlCtx();

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


SSL_CTX* clsSecureSocket::CreateSSlCtx() {
    SSL_CTX* pSSLCtx = nullptr;
#ifdef USE_SSL
    //highest available SSL/TLS version
    const SSL_METHOD * method =  TLS_server_method();
    pSSLCtx = SSL_CTX_new(method);
    //SSL_CTX_set_ssl_version(pSSLCtx, TLSv1_2_server_method());
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
