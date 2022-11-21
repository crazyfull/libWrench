#include "clsTCPListener.h"
#include "clsEpollManager.h"
#include "clsTCPSocket.h"
#include "clsTCPServer.h"
#include "log.h"

uint16_t clsTCPListener::GetListenPort() const
{
    return m_Port;
}

epoll_event *clsTCPListener::Event()
{
    return &m_ev;
}

clsTCPListener::clsTCPListener(clsTCPServer *pServer)
{
    m_pTCPServer = pServer;
    isStarted = false;
    m_Socket = 0;
    m_Port = 0;
    m_pEpollManage = NULL;
    m_Acceptcallback = NULL;
    m_AcceptcallbackPointer = NULL;
    setIDType(1);
    setPtrID(this);
}

clsTCPListener::~clsTCPListener()
{

}

void clsTCPListener::OnAccepting()
{
    //accept socket
    struct sockaddr_in ClientAddr;
    socklen_t addrLen = sizeof(ClientAddr);
    int new_socket = accept(m_Socket, (struct sockaddr *)&ClientAddr, &addrLen);

    //Check Soocket
    if(new_socket != ISINVALID ){

        //go to accept callback
        m_Acceptcallback(this, m_AcceptcallbackPointer, new_socket);

        //
        clsTCPSocket::SetSocketNonBlocking(new_socket);

        //clsTCPSocket::SetSocketSendBuffer(new_socket, 65*1000);

        clsTCPSocket::SetSocketNoDelay(new_socket, true);

        //close new socket if not accepted
        int retRyCount = clsTCPSocket::GetSocketConnectTimeout(new_socket);
        if(retRyCount != 0){
            if(retRyCount != TIMEOUT_7Sec){
                LOG("deny closed");
                close(new_socket);
            }
        }

    }else{
        DebugPrint("accept(), can't accept new socket, error[%d]", errno);
    }

}

bool clsTCPListener::StartListen(uint16_t Port, const char* bindIP, clsEpollManager *pEpollManager)
{

    if(pEpollManager == nullptr){
        DebugPrint("null pEpollManager");
        return false;
    }

    m_Port = Port;

    //set epoll manager
    m_pEpollManage = pEpollManager;

    //Create TCP Socket
    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Socket <= 0)
    {
        DebugPrint("Failed create socket():");
        close(m_Socket);
        return false;
    }

    //***********************************Socket options*********************************
    /**/
    // set enable socket SO_REUSEADDR
    clsTCPSocket::SetSocketResourceAddr(m_Socket, true);
    clsTCPSocket::SetSocketNonBlocking(m_Socket);

    //set timeout
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


    //bind port
    struct sockaddr_in ServerAddr;
    int intErr;
    if(bindIP){
        ServerAddr.sin_addr.s_addr = inet_addr(bindIP);     //htonl(INADDR_LOOPBACK); //htonl(INADDR_LOOPBACK); //htonl(INADDR_LOOPBACK);
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

    memset(&ServerAddr, 0, sizeof(ServerAddr));

    //Listen
    intErr = listen(m_Socket, 1000);//m_pEpollManage->MaxConnection()
    if(intErr == ISINVALID)
    {
        DebugPrint("cannot listen port: %hu, err:(%d)", m_Port, ERRNO);
        close(m_Socket);
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

void clsTCPListener::StopListen()
{
    m_pEpollManage->RemoveSocket(m_Socket);
    close(m_Socket);
    LOG("StopListen [%d]", m_Port)
            delete this;
}

int clsTCPListener::GetSocket() const
{
    return m_Socket;
}

void clsTCPListener::_setAcceptCallback(void *p, AcceptCallbackType Acceptcallback)
{
    m_Acceptcallback = Acceptcallback;
    m_AcceptcallbackPointer = p;
}

