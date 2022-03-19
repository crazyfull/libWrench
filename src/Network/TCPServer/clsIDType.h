#ifndef CLSIDTYPE_H
#define CLSIDTYPE_H

class clsTCPListener;
class clsTCPSocket;
class clsIDType
{
    short IDType;
    void *m_pPtr;

public:
    clsIDType();
    int getIDType() const;
    void setIDType(short value);
    void setPtrID(void *value);
    bool isListenerClass();
    clsIDType *getClassIDPtr();
    clsTCPListener *getTCPListenerPtr();
    clsTCPSocket *getTCPSocketPtr();

};

#endif // CLSIDTYPE_H
