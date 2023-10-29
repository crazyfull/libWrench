#ifndef CLSUDPLISTENER_H
#define CLSUDPLISTENER_H
#include "clsIDType.h"
#include "SocketHeader.h"

//class clsTCPListener;

class clsUDPListener;
class clsUDPServer;
class clsEpollManager;
class clsUDPSocket;
typedef void (*UDPCallbackType)(clsUDPListener*, void*, clsUDPSocket&, const char*Data, int DataLength);


class clsUDPListener: public clsIDType
{
private:
    //event
    clsEpollManager *m_pEpollManage;
    struct epoll_event m_ev;
    bool isStarted;

    //socket
    int m_Socket;
    uint16_t m_Port;

    //callbacks variables
    clsUDPServer *m_pUDPServer;
    UDPCallbackType m_UDPCallback;
    void *m_callbackPointer;

public:
    clsUDPListener(clsUDPServer *pServer);

    //public functions
    bool StartListen(uint16_t Port, const char *bindIP, clsEpollManager *pEpollManager);
    void StopListen();
    int GetSocket() const;
    void _setCallback(void* p, UDPCallbackType callback);

    //overides
    void OnReceiveData();
    uint16_t GetListenPort() const;
    epoll_event *Event();
};

#endif // CLSUDPLISTENER_H
