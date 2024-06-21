#include "UDPClient.h"
#include "log.h"
#include "clsTCPSocket.h"

UDPClient::UDPClient() {
    //m_socket = socket;
    //m_TargetAddress = Address;
    m_TargetPort = 0;
}

void UDPClient::setHost(const string &HostAddress)
{
    if(HostAddress.length() > 1){
        //get ip address -- Blocking method
        in_addr IPAddress = clsTCPSocket::GetIPFromHostAddress(HostAddress.c_str());
        if (IPAddress.s_addr == 0){
            DebugPrint("can't resolve host! [%d]", ERRNO);
            return;
        }else{
            m_TargeIPAddress = IPAddress;
        }
    }
}

void UDPClient::setPort(int newPort)
{
    m_TargetPort = newPort;
}

void UDPClient::setTimeOut(int milisecTimeout) {
    m_timeOut = milisecTimeout;
}

string UDPClient::SendReceive(const char *Data, int length) {
    string retBuffer;

    if(m_TargetPort <= 0 ){
        DebugPrint("port can not be empty\n");
        return retBuffer;
    }

    // Create UDP socket
    m_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == ISINVALID){
        DebugPrint("create socket() failed\n");
        return retBuffer;
    }

    //set Timeouts
    struct timeval tv;
    tv.tv_sec = m_timeOut / 1000;
    tv.tv_usec = (m_timeOut % 1000) * 1000;

    int isErr = setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0) {
        DebugPrint("error setsockopt set flag SO_RCVTIMEO, error[%d]", ERRNO);
        close(m_socket);
        return retBuffer;
    }

    isErr = setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0) {
        DebugPrint("error setsockopt set flag SO_SNDTIMEO, error[%d]", ERRNO);
        close(m_socket);
        return retBuffer;
    }

    /* Construct the server address structure */
    struct sockaddr_in targetHostAddr;
    memset(&targetHostAddr, 0, sizeof(targetHostAddr));                     /* Zero out structure */
    targetHostAddr.sin_family = AF_INET;                                    /* Internet address family */
    //targetHostAddr.sin_addr.s_addr = inet_addr(m_TargetHost.c_str());		/* Server IP address */
    targetHostAddr.sin_addr = m_TargeIPAddress;
    targetHostAddr.sin_port = htons(m_TargetPort);                          /* Server port */

    int sndSize = sendto(m_socket, Data, length, MSG_NOSIGNAL, (struct sockaddr*)&targetHostAddr, sizeof(targetHostAddr));
    //int sndSize = sendto(m_socket, Data, length, MSG_NOSIGNAL, (struct sockaddr*)&m_TargetAddress, sizeof(m_TargetAddress));
    if(sndSize == ISINVALID) {
        DebugPrint("sendto errno[%d]", ERRNO);
        close(m_socket);
        return retBuffer;
    }


    /*on receive data*/
    struct sockaddr_in clientSource;
    char buffer[BUFFER_SIZE+1];
    socklen_t client_address_length = sizeof(clientSource);
    memset(&clientSource, 0, sizeof(clientSource));
    int bytes_received = recvfrom(m_socket, buffer, sizeof(buffer), MSG_NOSIGNAL, (struct sockaddr *)&clientSource, &client_address_length);


    if (bytes_received < 0) {
        DebugPrint("Error receiving datagram");
        close(m_socket);
        return retBuffer;
    }

    buffer[bytes_received] = 0;
    retBuffer.append((char*)buffer, bytes_received);

    /*
    //Receive Buffer
    unsigned char Buffer[BUFFER_SIZE+1] = {0};
    memset(Buffer, 0, sizeof(Buffer));
    //ssize_t recvSize = recvfrom(m_socket, (char*)Buffer, BUFFER_SIZE, 0, (struct sockaddr *) &fromAddr, &fromSize );
    ssize_t recvSize = recvfrom(m_socket, (char*)Buffer, BUFFER_SIZE, MSG_WAITALL, nullptr, nullptr);

    if (recvSize == ISINVALID) {
        //DebugPrint("Error: WSARecvFrom () %d\n");
        close(sock);
        return ret;
    }

    if(recvSize > 0){
        ret.append((char*)Buffer, recvSize);
    }
*/

    close(m_socket);
    return retBuffer;
}

string UDPClient::SendReceive(const char *Data)
{
    return SendReceive(Data, strlen(Data));
}
