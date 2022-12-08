#ifndef CLSHASH_H
#define CLSHASH_H
#include "clsCString.h"
#define SHA512_HEX_DIGEST_LENGTH (64*2)

namespace cryptography
{
    class clsHash
    {
    private:

    public:
        clsHash();
        static uint8_t *hex_decode(const char *in, size_t len, uint8_t *out);
        static uint32_t hex_encode(const void *buf, int size, char *wp);

        static uint32_t ToHEX(const char *source, int sourceLength, char *Output);
        static void FromHEX(const char *source, int sourceLength, uint8_t *Output);
        static CString ToHEX(const char* inData, int inDataLength);
        static CString ToHEX(const CString& inData);
        static CString FromHEX(const char* inData, int inDataLength);

        static CString ToMD5(const char* inData, int inDataLength, bool toHex = false);
        static CString ToSHA1(const char* inData, int inDataLength, bool toHex = false);
        static CString ToSHA256(const char* inData, int inDataLength, bool toHex = false);
        static CString ToSHA512(const char* inData, int inDataLength, bool toHex = false);
        static uint32_t ToSHA512(const char* source, int sourceLength, char *outPut, bool toHex = false);
        static uint32_t ToCRC32(const void *buffer, size_t bufferSize);

        static uint8_t GenerateRandomNumber(uint8_t Min, uint8_t Max);
        static uint32_t GenerateRandomLongNumber(uint32_t Min, uint32_t Max);
        static void GenerateRandomBytes(unsigned char *OutBuffer, int OutBufferSize);
        static CString GenerateRandomString(int len);

    };
}
#endif // CLSHASH_H
