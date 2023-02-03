#ifndef CLSTCPSERVER_H
#define CLSTCPSERVER_H
#include "clsTCPListener.h"
#include "clsEpollManager.h"
#include "clsSecureSocket.h"

class clsTCPServer
{
private:

    int m_SocketOptSendRecTimeout;
    int m_SocketOptReceiveBufferSize;
    int m_SocketOptSendBufferSize;
    int m_SocketOptLingerTime;
    bool m_SocketOptShare;
    bool m_SocketOptNoDelay;
    bool m_SocketKeepAlive;
    clsEpollManager* m_pEpoll;
    clsSecureSocket* m_pSSlSocket;

    void AddThreadPool(int threadcount);

public:

    clsTCPServer(uint MaximumConnection = 1024);
    ~clsTCPServer();

    clsTCPListener *AddNewListener(uint16_t Port, const char *bindIP, void *p, AcceptCallbackType Acceptcallback);
    void Start();

    bool AddSocketToEventsPoll(clsTCPSocket *pTCPSocket);
    bool RemoveSocketFromEventsPoll(clsTCPSocket *pTCPSocket);

    void SetSocketSendAndReceiveTimeout(int timeSec);
    void SetSocketReceiveBufferSize(int bufferSize);
    void SetSocketSendBufferSize(int bufferSize);
    void SetSocketLinger(int Timeout);
    void SetSocketShare(bool isEnable);
    void SetSocketNoDelay(bool isEnable);
    void SetSocketKeepAlive(bool isEnable);


    int getSocketOptSendRecTimeout() const;
    int getSocketOptReceiveBufferSize() const;
    int getSocketOptSendBufferSize() const;
    int getSocketOptLingerTime() const;
    bool getSocketOptShare() const;
    bool getSocketOptNoDelay() const;
    bool getSocketKeepAlive() const;
    clsSecureSocket *SSlSocket() const;
    bool SetCertificateSSL(const char *CertPath, const char *KeyPath);
    void SetSSLMethod(SSLMethod value);
};

#endif // CLSTCPSERVER_H
