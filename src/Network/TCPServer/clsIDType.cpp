#include "clsIDType.h"

clsIDType::clsIDType()
{
    setIDType(0);
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

bool clsIDType::isListenerClass()
{
    if(IDType == 1){
        return true;
    }
    return false;
}

clsIDType *clsIDType::getClassIDPtr()
{
    return this;
}

clsTCPListener *clsIDType::getTCPListenerPtr()
{
    return static_cast<clsTCPListener*>(m_pPtr);
}

clsTCPSocket *clsIDType::getTCPSocketPtr()
{
    return static_cast<clsTCPSocket*>(m_pPtr);
}

