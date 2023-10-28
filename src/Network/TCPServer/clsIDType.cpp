#include "clsIDType.h"

clsIDType::clsIDType()
{
    setIDType(IS_NOT_SET);
    setPtrID(nullptr);
}

int clsIDType::getIDType() const
{
    return IDType;
}

void clsIDType::setIDType(short value)
{
    IDType = value;
}

void clsIDType::setPtrID(void *value)
{
    m_pPtr = value;
}

bool clsIDType::isTCPListenerClass()
{
    if(IDType == IS_TCP_LISTENER){
        return true;
    }
    return false;
}

clsIDType *clsIDType::getClassIDPtr()
{
    return this;
}

clsUDPListener *clsIDType::getUDPListenerPtr()
{
    return static_cast<clsUDPListener*>(m_pPtr);
}

clsTCPListener *clsIDType::getTCPListenerPtr()
{
    return static_cast<clsTCPListener*>(m_pPtr);
}

clsTCPSocket *clsIDType::getTCPSocketPtr()
{
    return static_cast<clsTCPSocket*>(m_pPtr);
}

