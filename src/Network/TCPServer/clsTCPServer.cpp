#include "clsTCPServer.h"
#include "clsFileDirectory.h"
#include "clsTCPSocket.h"
#include "log.h"
#include <ifaddrs.h>
#include <net/if.h>


clsTCPServer::clsTCPServer(uint MaximumConnection)
{
    m_pEpoll = new clsEpollManager(MaximumConnection);
    m_pSSlSocket = new clsSecureSocket;

    //
    m_SocketOptSendRecTimeout = 0;
    m_SocketOptReceiveBufferSize = 0;
    m_SocketOptSendBufferSize = 0;
    m_SocketOptShare = false;
    m_SocketOptNoDelay = false;
    m_SocketKeepAlive = false;
    m_SocketOptLingerTime = 0;


    //set maximum connection
    //FileDirectory::SetMaximumFileDiscriptor(MaximumConnection*2);
}

clsTCPServer::~clsTCPServer()
{
    //
}

int clsTCPServer::getSocketOptSendRecTimeout() const
{
    return m_SocketOptSendRecTimeout;
}

int clsTCPServer::getSocketOptReceiveBufferSize() const
{
    return m_SocketOptReceiveBufferSize;
}

int clsTCPServer::getSocketOptSendBufferSize() const
{
    return m_SocketOptSendBufferSize;
}

int clsTCPServer::getSocketOptLingerTime() const
{
    return m_SocketOptLingerTime;
}

bool clsTCPServer::getSocketOptShare() const
{
    return m_SocketOptShare;
}

bool clsTCPServer::getSocketOptNoDelay() const
{
    return m_SocketOptNoDelay;
}

bool clsTCPServer::getSocketKeepAlive() const
{
    return m_SocketKeepAlive;
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

void clsTCPServer::Start()
{
    m_pEpoll->Start();
}

clsSecureSocket *clsTCPServer::SSlSocket() const
{
    return m_pSSlSocket;
}

bool clsTCPServer::SetCertificateSSL(const char *CertPath, const char *KeyPath)
{
    return m_pSSlSocket->setSSLConfig(CertPath, KeyPath);
}

void clsTCPServer::SetSSLMethod(SSLMethod value){
    m_pSSlSocket->setSSLMethod(value);
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
    m_SocketOptSendRecTimeout = timeSec;
}

void clsTCPServer::SetSocketReceiveBufferSize(int bufferSize)
{
    m_SocketOptReceiveBufferSize = bufferSize;
}

void clsTCPServer::SetSocketSendBufferSize(int bufferSize)
{
    m_SocketOptSendBufferSize = bufferSize;
}

void clsTCPServer::SetSocketShare(bool isEnable)
{
    m_SocketOptShare = isEnable;
}

void clsTCPServer::SetSocketNoDelay(bool isEnable)
{
    m_SocketOptNoDelay = isEnable;
}

void clsTCPServer::SetSocketKeepAlive(bool isEnable)
{
    m_SocketKeepAlive = isEnable;
}

void clsTCPServer::SetSocketLinger(int Timeout)
{
    m_SocketOptLingerTime = Timeout;
}


const CString clsTCPServer::getPrimaryIPAddress()  {
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *temp_addr = nullptr;
    CString ipAddress = "0.0.0.0";

    // Retrieve the current interfaces - returns 0 on success
    if (getifaddrs(&interfaces) == 0) {
        temp_addr = interfaces;
        while (temp_addr != nullptr) {

            // Check if it is a valid IP4 Address
            if(temp_addr->ifa_addr){
                if (temp_addr->ifa_addr->sa_family == AF_INET) {
                    // Skip the loopback interface
                    if (std::strcmp(temp_addr->ifa_name, "lo") != 0) {

                        if (temp_addr->ifa_flags & IFF_RUNNING && !(temp_addr->ifa_flags & IFF_LOOPBACK)) {
                            ipAddress = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                            break;
                        }
                        //ipAddress = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                        //break;
                    }
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free the memory allocated by getifaddrs
    freeifaddrs(interfaces);
    return ipAddress;
}

