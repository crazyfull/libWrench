#ifndef CLSUDPSOCKET_H
#define CLSUDPSOCKET_H
#include "SocketHeader.h"


class clsUDPSocket
{
    struct sockaddr_in m_TargetAddress;
    char m_ipSource[INET_ADDRSTRLEN];
    int m_socket;

public:
    clsUDPSocket();
    clsUDPSocket(int socket, sockaddr_in &Address);
    void Send(const char *Data, int length);
    void Send(const char *Data);
    const char *getIPAddress();
    int getPort();
};

#endif // CLSUDPSOCKET_H
