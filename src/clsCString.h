#ifndef CSTRING_H
#define CSTRING_H
#include <stdio.h>
#include <stdint.h>
#include <pthread.h> //thread
#include <sys/types.h>


class CString
{

    char *Buffer;
    uint32_t m_ReserveSize;
    uint32_t m_Lengh;

    void Free();
    void ClearMembers();
public:

    CString();
    ~CString();
    CString(const char* data, int dataLengh);
    CString(const CString &other);
    CString(const int Number);
    CString(const char *text);
    CString &operator = (const CString &other);
    CString &operator = (const CString *other);
    CString &operator = (const char *text);
    CString &operator = (int Number);
    CString &operator += (const CString &text);
    CString &operator += (const char *text);
    bool operator == (const char *text);
    bool operator == (const char *text) const;

    CString &operator << (const char *text);
    CString &operator << (int Number);

    CString &operator +(const CString &other);
    CString &operator +(const CString *other);
    const char* operator + (const char *text);

    void Reset(bool setZeroBuffer = false);
    bool AddReserveSize(int reserveSize);
    void ReserveBlock(int16_t Lengh, bool beZero = false);
    void Clear();
    void append(const uint8_t* data, uint16_t dataLength);
    void append(const char* data, int dataLengh);
    void append(const char* data);
    void append(uint32_t Number);
    void append(const CString &data);
    void append(const CString* data);
    bool Copy(int32_t index, const void *Source, int32_t SourceSize);

    const char* Data();
    const char* Data() const;

    int32_t length();
    int32_t length() const;

    CString *GetThis();


    static bool isCompare(const CString *Source, const char *dest);
    static bool isCompare(const char *Source, int SourceSize, const char *dest, int destSize);
    static bool isCompare(const char *Source, const char *dest);
    bool isCompare(const char *dest, int destSize);
    bool isCompare(const char *dest, int destSize) const;
    bool isCompare(const CString *dest);
    bool isCompare(const CString *dest) const;
    bool isCompare(const char *dest);
    bool isCompare(const char *dest) const;
    const char *at(uint p);

    static bool itStartsWith(const char *Source, int SourceSize, const char *dest, int destSize);
    bool itStartsWith(const char *dest);
    bool itStartsWith(const char *dest) const;
    bool itStartsWith(const CString *dest);
    bool itStartsWith(const CString *dest) const;

    static char *toLower(char *str);
    static char *toUper(char *str);
    static CString Replace(const char *Source, int SourceLen, const char *Match, int MatchLen, const char *text, int textLen);
    static CString Replace(CString *Source, const char *Match, int MatchLen, const char *text, int textLen);
    static CString Replace(CString *Source, const char *Match, const char *text);
    static void Replace(CString *Source, char find, char replac);
    void Replace(char find, char replac);
    static void NumberToString(uint64_t source, char Dest[]);
    static void NumberToString(double source, char Dest[]);
    static CString NumberToString(uint64_t source);
    static long StringToNumber(const char* source);
    long toNumber();

    static int FindPostion(const CString *Source, const char *Match1, int Match1Len);
    static int FindPostion(const char *Source, int SourceLen, const char *Match1, int Match1Len);
    static CString SplitString(const char *Source, int SourceLen, const char *Match1, int Match1Len, const char *Match2, int Match2Len);
    static CString SplitString(const char *Source, int SourceLen, const char *Match1, const char *Match2);
    static CString SplitString(const char *Source, const char *Match1, const char *Match2);
    static CString SplitString(const CString &Source, const char *Match1, const char *Match2);

    bool isEmpty();
    bool isEmpty() const;
    bool ReadFile(const char *FilePath);
    bool SaveFile(const char *FilePath);
    static bool isExistCase(const char *text, const char *content);
    static bool isExist(const char *text, const char *content);
    bool isExist(const char *text) const;
    bool isExistCase(const char *text) const;


    void SetDataWithNewPointer(char *data, int dataLengh);

    CString replaceAll(const CString &buff, const CString &target, const CString &to);
};


#endif // CSTRING_H
