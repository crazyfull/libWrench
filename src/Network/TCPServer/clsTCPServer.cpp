#include "clsTCPServer.h"
#include "clsFileDirectory.h"
#include "clsTCPSocket.h"
#include "log.h"

int clsTCPServer::getSocketOptSendRecTimeout() const
{
    return SocketOptSendRecTimeout;
}

int clsTCPServer::getSocketOptReceiveBufferSize() const
{
    return SocketOptReceiveBufferSize;
}

int clsTCPServer::getSocketOptSendBufferSize() const
{
    return SocketOptSendBufferSize;
}

int clsTCPServer::getSocketOptLingerTime() const
{
    return SocketOptLingerTime;
}

bool clsTCPServer::getSocketOptShare() const
{
    return SocketOptShare;
}

bool clsTCPServer::getSocketOptNoDelay() const
{
    return SocketOptNoDelay;
}

bool clsTCPServer::getSocketKeepAlive() const
{
    return SocketKeepAlive;
}

clsTCPServer::clsTCPServer(uint MaximumConnection)
{
    m_pEpoll = new clsEpollManager(this, MaximumConnection);

    //
    SocketOptSendRecTimeout = 0;
    SocketOptReceiveBufferSize = 0;
    SocketOptSendBufferSize = 0;
    SocketOptShare = false;
    SocketOptNoDelay = false;
    SocketKeepAlive = false;
    SocketOptLingerTime = 0;


    //set maximum connection
    //FileDirectory::SetMaximumFileDiscriptor(MaximumConnection*2);
}


clsTCPListener *clsTCPServer::AddNewListener(uint16_t Port, const char* bindIP, void *p, AcceptCallbackType Acceptcallback)
{
    clsTCPListener *pListener = new clsTCPListener(this);
    pListener->_setAcceptCallback(p, Acceptcallback);
    //pListener->GetSocketLimit();
    //pListener->SetSocketLimit(10000);

    bool ret = pListener->StartListen(Port, bindIP, m_pEpoll);
    if(ret){
        if(!bindIP)
            bindIP = "0.0.0.0";
        LOG("StartListen: (%s:%d)", bindIP, Port);
        return pListener;
    }else{
        delete pListener;
    }
    return nullptr;

}

void clsTCPServer::AddThreadPool(int threadcount)
{
    m_pEpoll->AddWorker(threadcount);
}

bool clsTCPServer::AddSocketToEventsPoll(clsTCPSocket *pTCPSocket)
{
    //
    return m_pEpoll->AddSocket(pTCPSocket->GetSocket(), pTCPSocket->Event());
}

bool clsTCPServer::RemoveSocketFromEventsPoll(clsTCPSocket *pTCPSocket)
{
    //
    return m_pEpoll->RemoveSocket(pTCPSocket);
}

/*
clsTCPServer *clsTCPServer::GetTCPServerPointer()
{
    return g_pServer;
}
*/

void clsTCPServer::SetSocketSendAndReceiveTimeout(int timeSec)
{
    SocketOptSendRecTimeout = timeSec;
}

void clsTCPServer::SetSocketReceiveBufferSize(int bufferSize)
{
    SocketOptReceiveBufferSize = bufferSize;
}

void clsTCPServer::SetSocketSendBufferSize(int bufferSize)
{
    SocketOptSendBufferSize = bufferSize;
}

void clsTCPServer::SetSocketShare(bool isEnable)
{
    SocketOptShare = isEnable;
}

void clsTCPServer::SetSocketNoDelay(bool isEnable)
{
    SocketOptNoDelay = isEnable;
}

void clsTCPServer::SetSocketKeepAlive(bool isEnable)
{
    SocketKeepAlive = isEnable;
}

void clsTCPServer::SetSocketLinger(int Timeout)
{
    SocketOptLingerTime = Timeout;
}

