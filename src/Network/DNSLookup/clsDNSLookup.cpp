#include "clsDNSLookup.h"
#include "clsDNSHeader.h"
#include "clsDNSPackets.h"
#include "SocketHeader.h"
#include "log.h"
#include <string.h>
#include <iostream>

using namespace std;

clsDNSLookup::clsDNSLookup()
{

}

string SendPacket(string pck, const char* ServerHost, int ServerPort, int TimeOutSec)
{
    string ret;
    const char* Data = pck.c_str();
    int DataLen = pck.length();

    //send packet
    struct sockaddr_in ServerAddr;		/* server address */


    // Create UDP socket
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == ISINVALID){
        DebugPrint("create socket() failed\n");
        return ret;
    }

    /* Construct the server address structure */
    memset(&ServerAddr, 0, sizeof(ServerAddr));             /* Zero out structure */
    ServerAddr.sin_family = AF_INET;                        /* Internet address family */
    ServerAddr.sin_addr.s_addr = inet_addr(ServerHost);		/* Server IP address */
    ServerAddr.sin_port = htons(ServerPort);                /* Server port */


    //set Timeouts
    struct timeval tv;
    tv.tv_sec = TimeOutSec;
    tv.tv_usec = 500000;

    int isErr = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_RCVTIMEO, error[%d]", ERRNO);
    }

    isErr =  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_SNDTIMEO, error[%d]", ERRNO);
    }


    //Send Buffer
    int sndSize = sendto(sock, Data, DataLen, MSG_NOSIGNAL, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));
    if (sndSize != DataLen)
    {
        DebugPrint("sendto() sent a different number of bytes than expected\n");
        close(sock);
        return ret;
    }

    //Receive Buffer
    unsigned char Buffer[BUFFER_SIZE+1] = {0};
    memset(Buffer, 0, sizeof(Buffer));
    //ssize_t recvSize = recvfrom(m_socket, (char*)Buffer, BUFFER_SIZE, 0, (struct sockaddr *) &fromAddr, &fromSize );
    ssize_t recvSize = recvfrom(sock, (char*)Buffer, BUFFER_SIZE, MSG_WAITALL, nullptr, nullptr);

    if (recvSize == ISINVALID)
    {
        //DebugPrint("Error: WSARecvFrom () %d\n");
        close(sock);
        return ret;
    }

    if(recvSize > 0){
        ret.append((char*)Buffer, recvSize);
    }

    close(sock);
    return ret;
}

CString clsDNSLookup::DNSLookup(const char *TargetHost, int Timeout, const char *DNSServerIP, int DNSServerPort)
{
    CString ret;

    //age HostAddress ipaddress bod resolve nemishe
    int IP = inet_addr(TargetHost);
    if(IP != ISINVALID){
        ret = inet_ntoa(*(struct in_addr*)&IP);
        return ret;
    }

    int HostLen = strlen(TargetHost);

    //create pck
    string Pck = clsDNSPackets::PckRequestA(TargetHost, HostLen);

    //send
    string response = SendPacket(Pck, DNSServerIP, DNSServerPort, Timeout);
    if(response.length() > 0){
        //parse
        uint32_t intIP  = clsDNSHeader::ParseDNSPaket(HostLen+2, (unsigned char*)response.data(), response.length());
        if(intIP > 0){
            ret = inet_ntoa(*(struct in_addr*)&intIP);
        }
        //LOG("Target: [%s], IP: [%s](%lu)", TargetHost, ret.Data(), response.length());
    }

    return ret;
}
