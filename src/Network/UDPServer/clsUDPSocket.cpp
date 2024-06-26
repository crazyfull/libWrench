#include "clsUDPSocket.h"
#include "log.h"


clsUDPSocket::clsUDPSocket()
{

}

clsUDPSocket::clsUDPSocket(int socket, sockaddr_in &Address)
{
    m_socket = socket;
    m_TargetAddress = Address;
}

void clsUDPSocket::Send(const char *Data, int length)
{
    int ret = sendto(m_socket, Data, length, MSG_NOSIGNAL, (struct sockaddr*)&m_TargetAddress, sizeof(m_TargetAddress));
    if(ret == ISINVALID) {
        DebugPrint("sendto errno[%d]", ERRNO);
    }

}

void clsUDPSocket::Send(const char *Data)
{
    Send(Data, strlen(Data));
}

const char *clsUDPSocket::getIPAddressDescription()
{
    inet_ntop(AF_INET, &m_TargetAddress.sin_addr, m_ipSource, sizeof(m_ipSource));
    return m_ipSource;
}

uint32_t clsUDPSocket::getIPAddress()
{
    return m_TargetAddress.sin_addr.s_addr;
}

int clsUDPSocket::getPort()
{
    return htons(m_TargetAddress.sin_port);
}
