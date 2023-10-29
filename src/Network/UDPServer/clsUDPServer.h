#ifndef CLSUDPSERVER_H
#define CLSUDPSERVER_H

#include "clsUDPListener.h"
#include "clsEpollManager.h"

class clsUDPServer
{
private:
    clsEpollManager* m_pEpoll;

public:
    clsUDPServer(uint MaximumConnection);
    clsUDPListener *AddNewListener(uint16_t Port, const char *bindIP, void *p, UDPCallbackType UDPCallback);
    void Start();

};

#endif // CLSUDPSERVER_H
