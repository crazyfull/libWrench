#include "clsUDPServer.h"
#include "clsFileDirectory.h"
//#include "clsTCPSocket.h"
#include "log.h"

clsUDPServer::clsUDPServer(uint MaximumConnection)
{
    m_pEpoll = new clsEpollManager(MaximumConnection);
}

clsUDPListener *clsUDPServer::AddNewListener(uint16_t Port, const char *bindIP, void *p, UDPCallbackType UDPCallback)
{
    clsUDPListener *pListener = new clsUDPListener(this);
    pListener->_setCallback(p, UDPCallback);

    bool ret = pListener->StartListen(Port, bindIP, m_pEpoll);
    if(ret){
        if(!bindIP)
            bindIP = "0.0.0.0";
        LOG("StartListen(UDP): (%s:%d)", bindIP, Port);


        return pListener;
    }else{
        delete pListener;
    }
    return nullptr;

}

void clsUDPServer::Start()
{
    m_pEpoll->Start();
}
