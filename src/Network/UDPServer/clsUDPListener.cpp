#include "clsUDPListener.h"
#include "clsUDPServer.h"
#include "clsEpollManager.h"
#include "clsTCPSocket.h"
//#include "clsTCPServer.h"
#include "clsUDPSocket.h"

#include "log.h"

clsUDPListener::clsUDPListener(clsUDPServer *pServer)
{
    isStarted = false;
    m_Socket = 0;
    m_Port = 0;
    m_pEpollManage = NULL;
    m_UDPCallback = NULL;
    m_callbackPointer = NULL;
    m_pUDPServer = pServer;
    setIDType(IS_UDP_LISTENER);
    setPtrID(this);
}

bool clsUDPListener::StartListen(uint16_t Port, const char *bindIP, clsEpollManager *pEpollManager)
{

    if(pEpollManager == nullptr){
        DebugPrint("null pEpollManager");
        return false;
    }

    m_Port = Port;

    //set epoll manager
    m_pEpollManage = pEpollManager;

    //Create TCP Socket
    m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_Socket <= 0) {
        DebugPrint("Failed create socket():");
        close(m_Socket);
        return false;
    }

    //***********************************Socket options*********************************
    /**/
    // set enable socket SO_REUSEADDR
    clsTCPSocket::SetSocketResourceAddr(m_Socket, true);
    clsTCPSocket::SetSocketNonBlocking(m_Socket);
    //clsTCPSocket::SetSocketSendAndReceiveTimeOut(m_Socket, 3);

    //set timeout
/*
    if(m_pTCPServer->getSocketOptSendRecTimeout() > 0)
        clsTCPSocket::SetSocketSendAndReceiveTimeOut(m_Socket, m_pTCPServer->getSocketOptSendRecTimeout());

    if(m_pTCPServer->getSocketOptReceiveBufferSize() > 0)
        clsTCPSocket::SetSocketReceiveBuffer(m_Socket, m_pTCPServer->getSocketOptReceiveBufferSize());

    if(m_pTCPServer->getSocketOptSendBufferSize() > 0)
        clsTCPSocket::SetSocketSendBuffer(m_Socket, m_pTCPServer->getSocketOptSendBufferSize());

    if(m_pTCPServer->getSocketOptLingerTime() >= 0)
        clsTCPSocket::SetSocketLinger(m_Socket, m_pTCPServer->getSocketOptLingerTime());

    clsTCPSocket::SetSocketShared(m_Socket, m_pTCPServer->getSocketOptShare());
    clsTCPSocket::SetSocketKeepAlive(m_Socket, m_pTCPServer->getSocketKeepAlive());
*/

    //bind port
    struct sockaddr_in ServerAddr;
    memset(&ServerAddr, 0, sizeof(ServerAddr));

    int intErr;
    if(bindIP){
        ServerAddr.sin_addr.s_addr = inet_addr(bindIP);         //htonl(INADDR_LOOPBACK); //htonl(INADDR_LOOPBACK); //htonl(INADDR_LOOPBACK);
    }else{
        ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);         //inet_addr("127.0.0.1"); //htonl(INADDR_ANY); //;
    }
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(m_Port);

    intErr = bind(m_Socket, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr));
    if(intErr == ISINVALID)
    {
        DebugPrint("cannot bind to the port: %d, err:(%d)",m_Port, ERRNO);
        return false;
    }

    //create epoll
    m_ev.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING;
    m_ev.data.u32 = 0;
    m_ev.data.u64 = 0;
    m_ev.data.ptr = getClassIDPtr();

    //add to epoll
    intErr = m_pEpollManage->AddSocket(m_Socket, &m_ev);
    if(intErr != 0){
        DebugPrint("Could not add socket %d to epoll, err:(%d)", m_Socket, ERRNO);
        close(m_Socket);
        return false;
    }

    return true;
}

void clsUDPListener::StopListen()
{
    m_pEpollManage->RemoveSocket(m_Socket);
    close(m_Socket);
    LOG("StopListen [%d]", m_Port)
    delete this;
}

int clsUDPListener::GetSocket() const
{
    return m_Socket;
}

void clsUDPListener::_setCallback(void *p, UDPCallbackType callback)
{
    m_UDPCallback = callback;
    m_callbackPointer = p;
}

void clsUDPListener::OnReceiveData()
{
    struct sockaddr_in clientSource;

    /*on receive data*/
    char buffer[BUFFER_SIZE+1];
    socklen_t client_address_length;
    memset(&clientSource, 0, sizeof(clientSource));
    int bytes_received = recvfrom(m_Socket, buffer, sizeof(buffer), MSG_NOSIGNAL, (struct sockaddr *)&clientSource, &client_address_length);
    if (bytes_received < 0) {
        DebugPrint("Error receiving datagram");
    }

    buffer[bytes_received] = 0;

    if(m_UDPCallback){
        //create client
        clsUDPSocket udpclient(m_Socket, clientSource);


        m_UDPCallback(this, m_callbackPointer, udpclient, buffer, bytes_received);
    }
}

uint16_t clsUDPListener::GetListenPort() const
{
    return m_Port;
}

epoll_event *clsUDPListener::Event()
{
    return &m_ev;
}
