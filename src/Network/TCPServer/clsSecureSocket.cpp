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

    // Initialize the SSL libraries
    OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS, NULL);

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

    //SSL_CTX_set_mode(pSSLCtx, SSL_MODE_ENABLE_PARTIAL_WRITE);

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

    if (SSL_CTX_use_certificate_file(m_pServerSSlCtx, CertPath, SSL_FILETYPE_PEM) <= 0) {
        DebugPrint("The certificate file [%s] does not exist or you do not have permission to read that file.", CertPath);
        return false;
    }

    if (SSL_CTX_use_PrivateKey_file(m_pServerSSlCtx, KeyPath, SSL_FILETYPE_PEM) <= 0) {
        DebugPrint("The private key file [%s] does not exist or you do not have permission to read that file.", KeyPath);
        return false;
    }

    if (SSL_CTX_check_private_key(m_pServerSSlCtx) <= 0)
    {
        DebugPrint("The private key file does not match the corresponding public key in the certificate.");
        return false;
    }


    //SSL_CTX_set_ciphersuites(m_pServerSSlCtx, "TLS_CHACHA20_POLY1305_SHA256");

    // the application will not request the certificate for the remote client application when the SSL session is started.
    // SSL_VERIFY_NONE

    //he application will request and verify the certificate for the remote client application when the SSL session is started
    SSL_CTX_set_verify(m_pServerSSlCtx, SSL_VERIFY_NONE, NULL);

    //need set for epoll
    SSL_CTX_set_mode(m_pServerSSlCtx,  SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

    //
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION | SSL_OP_NO_TICKET;
    SSL_CTX_set_options(m_pServerSSlCtx, flags);
    return true;
#else
    return false;
#endif
}

SSL* clsSecureSocket::newClientSSL(int fd) {
    SSL *pClientSSlCtx;
#ifdef USE_SSL
    //Create new ssl object
    pClientSSlCtx = SSL_new(m_pServerSSlCtx);
    if(pClientSSlCtx){
        //Bind the ssl object with the socket
        SSL_set_accept_state(pClientSSlCtx);
        SSL_set_fd(pClientSSlCtx, fd);


    }
#endif
    return pClientSSlCtx;
}
