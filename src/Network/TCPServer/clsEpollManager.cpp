#include "clsEpollManager.h"
#include "clsUDPServer.h"
#include "clsTCPServer.h"
#include "clsTCPSocket.h"
#include "log.h"

clsEpollManager::clsEpollManager(clsTCPServer *pTCPServer, uint MaxConnection)
{
    m_epollSocket = 0;
    m_MaxConnection = 0;
    m_Count = 0;
    //m_pTCPServer = pTCPServer;

    if (pthread_mutex_init(&m_Mutex, NULL) != 0)
    {
        DebugPrint("mutex init failed");
        return;
    }

    //init epool and set max connections
    _Create(MaxConnection);
}

clsEpollManager::clsEpollManager(uint MaxConnection)
{
    m_epollSocket = 0;
    m_MaxConnection = 0;
    m_Count = 0;

    if (pthread_mutex_init(&m_Mutex, NULL) != 0)
    {
        DebugPrint("mutex init failed");
        return;
    }

    //init epool and set max connections
    _Create(MaxConnection);
}

void clsEpollManager::_MutexLock()
{
    pthread_mutex_lock(&m_Mutex);
}

void clsEpollManager::_MutexUnlock()
{
    pthread_mutex_unlock(&m_Mutex);
}
uint clsEpollManager::MaxConnection() const
{
    return m_MaxConnection;
}

void clsEpollManager::_onStartWorker(clsThread *pTherad, void* pArg)
{
    clsEpollManager *This = static_cast<clsEpollManager*>(pArg);

    This->_OnWaitForNewEvenet(pTherad);
}

clsEpollManager::~clsEpollManager()
{
    //stop therad deleet epool
    pthread_mutex_destroy(&m_Mutex);
}

bool clsEpollManager::_Create(uint MaxConnection)
{
    //set max connection
    m_MaxConnection = MaxConnection;

    //create epoll
    m_epollSocket = epoll_create(m_MaxConnection);
    if (m_epollSocket == ISINVALID){
        DebugPrint("Could not create epoll, err:(%d)", ERRNO);
        return false;
    }

    /* */


   // AddWorker(1);
    return true;
}

void clsEpollManager::AddWorker(int workerCount)
{
    for(int i = 0; i < workerCount;i++)
    {
        clsThread *p = clsThread::CreateThread(_onStartWorker, this, 0);
        if(!p){
            DebugPrint("can not create thread!");
        }
    }
}


void clsEpollManager::CloseEpoll()
{
    closesocket(m_epollSocket);
}

void clsEpollManager::Start()
{
    //start at thread
    AddWorker(1);

    //start without thrad
    //_OnWaitForNewEvenet(nullptr);
}


void clsEpollManager::_OnWaitForNewEvenet(clsThread *pTherad)
{

    //DebugPrint("ther: %u", pthread_self());
    while (1)
    {
        struct epoll_event EpollEvent;
        int ress = epoll_wait(m_epollSocket, &EpollEvent, 1, 5000);

        //LOG("get events[%u]", EpollEvent.events);

        if(ress == ISINVALID){
            if(errno == EINTR){
                DebugPrint("Interrupted system call, err:(%d)", ERRNO);
                continue;
            }
            if(ERRNO == EAGAIN){
                //LOG("OnWaitForNewEvenet EAGAIN || EINTR");  //Interrupted system call
            }
            DebugPrint("epoll_wait, err:(%d)", ERRNO);
            continue;
        }

        if(ress == 0){
            //LOG("epoll timer; count:[%d] epooll ret:[%d], err:(%d)", m_Count, ress, ERRNO);
            continue;
        }

        //get pointer
        clsIDType *ID = static_cast<clsIDType*>(EpollEvent.data.ptr);
        if(!ID){
            DebugPrint("Receive null event[%d], err:(%d)", ress,ERRNO);
            continue;
        }

        if(ID->getIDType() == IS_UDP_LISTENER ){

            clsUDPListener *pUDPListener = ID->getUDPListenerPtr();
            if (EpollEvent.events & EPOLLIN){

                /*client*/
                pUDPListener->OnReceiveData();

                _MutexLock();
                ModifySocket(pUDPListener->GetSocket(), pUDPListener->Event());
                _MutexUnlock();
                continue;
            }

            LOG("IS_UDP_LISTENER unknown event[%u]", EpollEvent.events);
            continue;
        }

        //LOG("get events[%u]", EpollEvent.events);


        //for TCP Listener
        if(ID->isTCPListenerClass()){
            if (EpollEvent.events & EPOLLIN)
            {
                clsTCPListener *pTCPListener = ID->getTCPListenerPtr();
                if(!pTCPListener){
                    DebugPrint("pTCPListener is null");
                    continue;
                }

                //accept
                _MutexLock();
                pTCPListener->OnAccepting();
                ModifySocket(pTCPListener->GetSocket(), pTCPListener->Event());
                _MutexUnlock();

            }else{
                DebugPrint("Getlistiner unknown event! count:[%d] epooll ret:[%d], err:(%d)", m_Count, ress, ERRNO);
            }
            continue;
        }


        //for tcp sockets
        clsTCPSocket *pTCPSocket = ID->getTCPSocketPtr();
        if(!pTCPSocket){
            DebugPrint("pTCPSocket is null count:[%d] epooll ret:[%d], err:(%d)", m_Count, ress, ERRNO);
            continue;
        }

        // parse eventes
        _ParseEvents(pTCPSocket, &EpollEvent);


        //LOG("_ParseEvents %d", pTCPSocket->GetStatus());

        //modify event
        ModifySocket(pTCPSocket->GetSocket(), pTCPSocket->Event());



        //Close Event
        if(pTCPSocket->GetSocket() == ISINVALID)
            pTCPSocket->OnClosed();

    }
}


void clsEpollManager::_ParseEvents(clsTCPSocket *pTCPSocket, epoll_event *pTheEvent)
{
    if (pTheEvent->events & (EPOLLHUP | EPOLLPRI | EPOLLERR) ){
        if(pTCPSocket->GetStatus() == clsTCPSocket::Connecting)
        {
            //be host connect nashod
            pTCPSocket->OnConnectFailed("(EPOLLHUP | EPOLLPRI | EPOLLERR)!", ERRNO);
            pTCPSocket->Close();
            return;
        }

        if(pTCPSocket->GetStatus() == clsTCPSocket::Closing){
            RemoveSocket(pTCPSocket);
            return;
        }

        //DebugPrint("connection broken! Status:[%d], socket: %d, errno[%1d]", pTCPSocket->GetStatus(), pTCPSocket->GetSocket(), ERRNO);
        pTCPSocket->Close(false);
        return;
    }

    if (pTheEvent->events & EPOLLHUP) {

        DebugPrint("EPOLLHUP, GetSocket[%d], errno[%1d]", pTCPSocket->GetSocket(), ERRNO);
        pTCPSocket->Close(false);
        return;
    }

    //Send() Or Connected
    if (pTheEvent->events & EPOLLOUT)
    {
        if(pTCPSocket->GetStatus() == clsTCPSocket::Connecting)
        {
            if(pTCPSocket->isSocketHaveError() == true)
            {
                //be host connect nashod
                pTCPSocket->OnConnectFailed("isSocketHaveError!", ERRNO);
                pTCPSocket->Close(false);
            }else{
                //connected
                pTCPSocket->onConnected();
            }
        }else{
            //DebugPrint("WTF?");
            pTCPSocket->ResumeSendFile();
        }
        return;
    }

    //Receive() Or GetRequestConnect Or ClientClose
    if (pTheEvent->events & EPOLLIN)
    {
        pTCPSocket->onReceiving();
        return;
    }

    if (pTheEvent->events & EPOLLERR)
    {
        DebugPrint("EPOLLERR");
        return;
    }
}

int clsEpollManager::AddSocket(int Socket, struct epoll_event *event)
{
    int ret = epoll_ctl(m_epollSocket, EPOLL_CTL_ADD, Socket, event);
    if (ret != 0){
        DebugPrint("Couldn't add socket[%d], return: %d, err:(%d)", Socket, ret, ERRNO);
    }else
        m_Count++;

    return ret;
}

int clsEpollManager::RemoveSocket(clsTCPSocket *pTCPSocket)
{
    _MutexLock();

    int ret = RemoveSocket(pTCPSocket->GetSocket());
    if(ret == 0)
        pTCPSocket->CloseFinal();

    //LOG("CloseFinal, err:(%d)", ERRNO);

    _MutexUnlock();
    return ret;
}

int clsEpollManager::RemoveSocket(int Socket)
{
    int ret = epoll_ctl(m_epollSocket, EPOLL_CTL_DEL, Socket, nullptr);
    if (ret != 0){
        //EBADF
        DebugPrint("Couldn't remove client socket[%d], return: %d, err:(%d)", Socket, ret, ERRNO);
    }else{
        m_Count--;
    }

    return ret;
}

int clsEpollManager::ModifySocket(int Socket, struct epoll_event *event)
{
    if(Socket == ISINVALID || Socket == 0)
        return 0;

    int ret = epoll_ctl(m_epollSocket, EPOLL_CTL_MOD, Socket, event);
    if (ret != 0){
        DebugPrint("Couldn't modify socket: %d, return: %d, err:(%d)", Socket, ret,  ERRNO);
    }
    return ret;
}



