#ifndef CLSTCPLISTENER_H
#define CLSTCPLISTENER_H
#include "clsIDType.h"
#include "SocketHeader.h"

class clsTCPListener;
class clsTCPServer;
typedef void (*AcceptCallbackType)(clsTCPListener*, void*, int);

class clsEpollManager;
class clsTCPListener: public clsIDType
{
private:
    //socket
    int m_Socket;
    uint16_t m_Port;

    //events
    clsEpollManager *m_pEpollManage;
    struct epoll_event m_ev;
    bool isStarted;

    //callbacks variables
    clsTCPServer *m_pTCPServer;
    AcceptCallbackType m_Acceptcallback;
    void *m_AcceptcallbackPointer;

public:
    clsTCPListener(clsTCPServer *pServer);
    ~clsTCPListener();

    //public functions
    bool StartListen(uint16_t Port, const char *bindIP, clsEpollManager *pEpollManager);
    void StopListen();
    int GetSocket() const;
    void _setAcceptCallback(void* p, AcceptCallbackType Acceptcallback);

    //overides
    void OnAccepting();
    uint16_t GetListenPort() const;
    epoll_event *Event();
};

#endif // CLSTCPLISTENER_H
