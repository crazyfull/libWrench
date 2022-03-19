#ifndef CLSMASK_H
#define CLSMASK_H
#include <stdint.h>

namespace cryptography
{
    class clsMask
    {
    private:
        char m_key[4];

    public:
        clsMask();

        void Mask(char* Buffer, int BufferLength);
        void UnMask(char* Buffer, int BufferLength);
        void Setey(const char *Key_4Len);
        void SetNewDataMaskKey(uint16_t Length);
    };
}
#endif // CLSMASK_H
