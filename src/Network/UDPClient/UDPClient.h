#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include "SocketHeader.h"
#include <string>

using namespace std;
class UDPClient
{
    int m_timeOut = 0;
    int m_socket;
    //struct sockaddr_in m_TargetAddress;
    in_addr m_TargeIPAddress;
    int m_TargetPort;
public:
    UDPClient();

    string SendReceive(const char *Data, int length);
    string SendReceive(const char *Data);
    void setTimeOut(int milisecTimeout);
    void setHost(const string &HostAddress);
    void setPort(int newPort);
};

#endif // UDPCLIENT_H
