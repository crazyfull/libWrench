#ifndef CLSIDTYPE_H
#define CLSIDTYPE_H

class clsUDPListener;
class clsTCPListener;
class clsTCPSocket;
enum IDTypes {
    IS_NOT_SET = 0,
    IS_TCP_LISTENER = 1,
    IS_TCP_SOCKET = 2,
    IS_UDP_LISTENER = 3,
    IS_UDP_SOCKET = 4
};

class clsIDType
{
    short IDType;
    void *m_pPtr;

public:
    clsIDType();
    int getIDType() const;
    void setIDType(short value);
    void setPtrID(void *value);
    bool isTCPListenerClass();
    clsIDType *getClassIDPtr();
    clsUDPListener *getUDPListenerPtr();
    clsTCPListener *getTCPListenerPtr();
    clsTCPSocket *getTCPSocketPtr();

};

#endif // CLSIDTYPE_H
