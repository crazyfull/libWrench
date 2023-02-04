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

static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
{
    preverify_ok = 1;
    return 1;
    /*
    char    buf[256];
    X509   *err_cert;
    int     err, depth;
    SSL    *ssl;
    mydata_t *mydata;

    err_cert = X509_STORE_CTX_get_current_cert(ctx);
    err = X509_STORE_CTX_get_error(ctx);
    depth = X509_STORE_CTX_get_error_depth(ctx);

    /*
     * Retrieve the pointer to the SSL of the connection currently treated
     * and the application specific data stored into the SSL object.
     * /
    ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
    mydata = SSL_get_ex_data(ssl, mydata_index);

    X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);

    /*
     * Catch a too long certificate chain. The depth limit set using
     * SSL_CTX_set_verify_depth() is by purpose set to "limit+1" so
     * that whenever the "depth>verify_depth" condition is met, we
     * have violated the limit and want to log this error condition.
     * We must do it here, because the CHAIN_TOO_LONG error would not
     * be found explicitly; only errors introduced by cutting off the
     * additional certificates would be logged.
     * /
    if (depth > mydata->verify_depth) {
        preverify_ok = 0;
        err = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        X509_STORE_CTX_set_error(ctx, err);
    }
    if (!preverify_ok) {
        printf("verify error:num=%d:%s:depth=%d:%s\n", err,  X509_verify_cert_error_string(err), depth, buf);
    }
    else if (mydata->verbose_mode)
    {
        printf("depth=%d:%s\n", depth, buf);
    }

    // At this point, err contains the last verification error. We can use it for something special
    if (!preverify_ok && (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT))
    {
      X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
      printf("issuer= %s\n", buf);
    }

    if (mydata->always_continue)
      return 1;
    else
      return preverify_ok;
      */
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
    SSL_CTX_set_verify_depth(m_pServerSSlCtx, 3);

    SSL_CTX_set_verify(m_pServerSSlCtx, SSL_VERIFY_NONE, NULL);
    //SSL_CTX_set_verify(m_pServerSSlCtx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    // Load certificates of trusted CAs based on file provided
    if (SSL_CTX_load_verify_locations(m_pServerSSlCtx, CertPath, NULL) < 1) {
        DebugPrint("Error setting the verify locations.\n");
        return false;
    }

    //need set for epoll
    SSL_CTX_set_mode(m_pServerSSlCtx,  SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

    //
    SSL_CTX_set_options(m_pServerSSlCtx, SSL_OP_NO_COMPRESSION | SSL_OP_NO_TICKET); //SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
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
