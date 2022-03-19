#include "clsMask.h"
#include <string.h>

namespace cryptography
{
    clsMask::clsMask()
    {
        memset(m_key, 0, sizeof(m_key));
    }

    void clsMask::Mask(char *Buffer, int BufferLength)
    {
        if(!Buffer)
        {
            return;
        }

        for(int i = 0; i < BufferLength;i++)
        {
            //mask
            Buffer[i] = Buffer[i] ^ m_key[i% sizeof(m_key) ];
        }
    }

    void clsMask::UnMask(char *Buffer, int BufferLength)
    {
        if(!Buffer)
        {
            return;
        }

        for(int i = 0; i < BufferLength;i++)
        {
            //unmask
            Buffer[i] = Buffer[i] ^ m_key[i% sizeof(m_key) ];
        }
    }

    void clsMask::Setey(const char *Key_4Len)
    {
        memcpy(m_key, Key_4Len, sizeof(m_key));
    }

    void clsMask::SetNewDataMaskKey(uint16_t Length)
    {
        for(int i = 0; i < sizeof(m_key);i++)
        {
            m_key[i] = m_key[i] + (i ^ (Length/4))-i;
            i=i+2;
        }
    }
}
