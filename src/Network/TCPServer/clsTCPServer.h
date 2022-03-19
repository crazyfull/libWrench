#ifndef CLSTCPSERVER_H
#define CLSTCPSERVER_H
#include "clsTCPListener.h"
#include "clsEpollManager.h"

class clsTCPServer
{
private:

    int SocketOptSendRecTimeout;
    int SocketOptReceiveBufferSize;
    int SocketOptSendBufferSize;
    int SocketOptLingerTime;
    bool SocketOptShare;
    bool SocketOptNoDelay;
    bool SocketKeepAlive;
    clsEpollManager* m_pEpoll;
    void AddThreadPool(int threadcount);

public:

    clsTCPServer(uint MaximumConnection = 1024);

    clsTCPListener *AddNewListener(uint16_t Port, const char *bindIP, void *p, AcceptCallbackType Acceptcallback);

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
};

#endif // CLSTCPSERVER_H
