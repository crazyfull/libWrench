#ifndef CLSEPOLLMANAGER_H
#define CLSEPOLLMANAGER_H

#include "clsThread.h"


class clsTCPServer;
class clsClientPointer;
class clsTCPSocket;

class clsEpollManager
{
public:


private:
    int m_epollSocket;
    uint m_MaxConnection;
    uint m_Count;
    pthread_mutex_t m_Mutex;
    clsTCPServer *m_pTCPServer;


    bool _Create(uint MaxConnection);
    static void _onStartWorker(clsThread *pTherad, void* pArg);
    void _OnWaitForNewEvenet(clsThread *pTherad);
    void _ParseEvents(clsTCPSocket *pTCPSocket,  epoll_event *pTheEvent);

    void _MutexLock();
    void _MutexUnlock();
    int ModifySocket(int Socket, epoll_event *event);


public:

    //Functions
    clsEpollManager(clsTCPServer *pTCPServer, uint MaxConnection);
    ~clsEpollManager();

    void AddWorker(int workerCount);
    int AddSocket(int Socket, struct epoll_event *event);
    int RemoveSocket(clsTCPSocket *pTCPSocket);
    int RemoveSocket(int Socket);

    void CloseEpoll();

    //GetOverides
    uint MaxConnection() const;
};

#endif // CLSEPOLLMANAGER_H
