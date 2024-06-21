#ifndef CLSUDPSOCKET_H
#define CLSUDPSOCKET_H
#include "SocketHeader.h"


class clsUDPSocket
{
    char m_ipSource[INET_ADDRSTRLEN];
    struct sockaddr_in m_TargetAddress;
    int m_socket;


public:
    clsUDPSocket();
    clsUDPSocket(int socket, sockaddr_in &Address);
    void Send(const char *Data, int length);
    void Send(const char *Data);
    const char *getIPAddressDescription();
    uint32_t getIPAddress();
    int getPort();
};

#endif // CLSUDPSOCKET_H
