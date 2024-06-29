#include "clsCString.h"
#include <string.h>
#include <stdlib.h>
#include <algorithm> //toLower
#include <cctype>   //toLower

#ifdef ZERO
#else
#define ZERO (0)
#endif

#ifdef ISINVALID
#else
#define ISINVALID (-1)
#endif

CString::CString()
{
    //printf("MyString()\n");
    ClearMembers();

    //pthread_mutex_init(&m_Mutex, nullptr);
}

CString::~CString()
{
    Free();

    // pthread_mutex_destroy(&m_Mutex);
}
/*
void CString::_MutexLock()
{
   pthread_mutex_lock(&m_Mutex);
}

void CString::_MutexUnlock()
{
   pthread_mutex_unlock(&m_Mutex);
}
*/

void CString::ClearMembers()
{
    m_ReserveSize = 0;
    Buffer = NULL;
    m_Lengh = 0;
}

bool CString::AddReserveSize(int reserveSize)
{
    if(reserveSize != 0)
    {
        if(m_Lengh > 0)
        {
            Buffer = (char*)realloc(Buffer, reserveSize+m_ReserveSize+1);
            memset(Buffer+m_Lengh, 0, reserveSize+1);
            //printf("realloc: %d\n", reserveSize+m_ReserveSize+1);
        }else{
            Buffer = (char*)malloc(reserveSize+1);
            memset(Buffer, 0, reserveSize+1);
        }
        if(!Buffer)
            return false;   //eror

        m_ReserveSize += reserveSize;

    }
    return true;
}

void CString::ReserveBlock(int16_t Lengh, bool beZero)
{
    if(m_ReserveSize+Lengh >= m_Lengh)
    {
        //if(beZero) Buffer[OutSize] = 0;
        if(beZero) memset(Buffer+m_Lengh, 0, Lengh);
        m_Lengh += Lengh;
    }
}

bool CString::isCompare(const char* dest, int destSize)
{
    if(Buffer == NULL || dest == NULL || m_Lengh <= 0 || destSize <= 0)
    {
        return false;
    }

    //strncmp()
    if(destSize != m_Lengh)
        return false;

    //Compare
    if (m_Lengh < destSize)
        return false;

    for(int i = 0; i < destSize; i++)
    {
        if(Buffer[i] != dest[i])
        {
            return false;
        }
    }
    return true;
}

bool CString::isCompare(const char *dest, int destSize) const
{
    if(Buffer == nullptr || dest == nullptr || m_Lengh <= 0 || destSize <= 0)
    {
        return false;
    }

    //strncmp()
    if(destSize != m_Lengh)
        return false;

    //Compare
    if (m_Lengh < destSize)
        return false;

    for(int i = 0; i < destSize; i++)
    {
        if(Buffer[i] != dest[i])
        {
            return false;
        }
    }

    return true;
}

bool CString::isCompare(const CString *dest)
{
    if(!dest)
        return false;
    return isCompare(dest->Data(), dest->length());
}

bool CString::isCompare(const CString *dest) const
{
    if(!dest)
        return false;
    return isCompare(dest->Data(), dest->length());
}

bool CString::isCompare(const char *dest)
{
    if(!dest)
        return false;
    return isCompare(this->Data(), dest);
}

bool CString::isCompare(const char *dest) const
{
    if(!dest)
        return false;
    return isCompare(this->Data(), dest);
}

const char *CString::at(uint p)
{
    if(p > 0 && m_Lengh > 0 && p <= m_Lengh){
        return &Buffer[p];
    }else{
        return nullptr;
    }
}

bool CString::itStartsWith(const char *Source, int SourceSize, const char *dest, int destSize)
{
    if(Source == NULL || dest == NULL || SourceSize <= 0 || destSize <= 0) {
        return false;
    }

    if (destSize > SourceSize)
        return false;

    return !strncmp(Source, dest, destSize);

}

bool CString::itStartsWith(const char *dest)
{
    return CString::itStartsWith(this->Data(), this->length(), dest, strlen(dest));
}

bool CString::itStartsWith(const char *dest) const
{
    return CString::itStartsWith(this->Data(), this->length(), dest, strlen(dest));
}

bool CString::itStartsWith(const CString *dest)
{
    if(!dest)
        return false;

    return CString::itStartsWith(this->Data(), this->length(), dest->Data(), dest->length());
}

bool CString::itStartsWith(const CString *dest) const
{
    if(!dest)
        return false;

    return CString::itStartsWith(this->Data(), this->length(), dest->Data(), dest->length());
}

char *CString::toLower(char *str)
{
    if(!str)
        return nullptr;

    unsigned char *p = (unsigned char *)str;

    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return str;
}

char *CString::toUper(char *str)
{
    if(!str)
        return nullptr;

    unsigned char *p = (unsigned char *)str;

    while (*p) {
        *p = toupper((unsigned char)*p);
        p++;
    }

    return str;
}

CString CString::Replace(const char *Source, int SourceLen, const char *Match, int MatchLen, const char *text, int textLen)
{
    CString ret;
    if((Match == nullptr) || (Match == "") || (text == nullptr) || (Source == nullptr) ){
        return ret;
    }

    int EndAt = CString::FindPostion(Source, SourceLen, Match, MatchLen);
    int StartAt = (EndAt-MatchLen);
    if(EndAt > 0){
        ret.AddReserveSize(StartAt+textLen + (SourceLen - EndAt));
        ret.append(Source, StartAt);
        ret.append(text, textLen);
        ret.append(Source+EndAt, SourceLen - EndAt);
    }else{

    }

    //printf("len: (%d)\n", SourceLen - EndAt);
    //LOG("EnadAt[%d]", EnadAt);
    return  ret;
}

CString CString::Replace(CString *Source, const char *Match, int MatchLen, const char *text, int textLen)
{
    return Replace(Source->Data(), Source->length(), Match, MatchLen, text, textLen);
}

CString CString::Replace(CString *Source, const char *Match, const char *text)
{
    return Replace(Source->Data(), Source->length(), Match, strlen(Match), text, strlen(text));
}

void CString::Replace(CString *Source, char find, char replac)
{
    if(Source == NULL){
        return;
    }

    if(Source->length() > 0)
    {
        char *str = (char*)Source->Data();
        while (*str != '\0')
        {
            if(*str == find){
                *str = replac;
            }
            str++;
        }
    }
}

void CString::Replace(char find, char replac)
{
    Replace(this,find,  replac);
}

void CString::NumberToString(uint64_t source, char Dest[])
{
    int resSize = sprintf(Dest, "%lu", source);
    Dest[resSize] = 0;
    if(resSize > 0)
    {
        //ok
    }
}

void CString::NumberToString(double source, char Dest[])
{
    int resSize = sprintf(Dest, "%f", source);
    Dest[resSize] = 0;
    if(resSize > 0)
    {
        //ok
    }
}

CString CString::NumberToString(uint64_t source)
{
    CString ret;
    char Dest[64];
    int resSize = sprintf(Dest, "%lu", source);
    Dest[resSize] = 0;
    if(resSize > 0)
    {
        //ok
        ret.append(Dest, resSize);
    }

    return ret;
}

long CString::StringToNumber(const char *source)
{
    long ret = 0;
    if(source){
        sscanf(source, "%ld", &ret);
    }
    return ret;
}



long CString::toNumber()
{
    return CString::StringToNumber(Buffer);
}


/* Old
const char *CString::SplitString(const char *Source, const char *Match, bool AfterMatch)
{
    if(!Source || !Match)
        return nullptr;


    int pos = 0;

    while(1){
        pos = Find(Source, Match[0], pos);
        if(pos < 0){
            break;
        }


        char *str = (char*)Source+pos;
        char *strMath = (char*)Match;
        //LOG("mmmm: %s", str);
        //int i = 0;
        while (*str != '\0')
        {
            if(*strMath == '\0'){
                if(AfterMatch){
                    //LOG("bbbb: [%s]", str);
                    return str;
                }else{
                    str = (char*)Source;
                    //str[pos-1] = 0;
                    return str;
                }
            }

            if(*strMath != *str){
                break;
            }

            str++;
            strMath++;
            pos++;
        }
    }

    return NULL;
}

int CString::Find(const char *Source, char Match, int atPos)
{
    if(!Source)
        return -1;

    char *str = (char*)Source + atPos;



    int i = 0;
    while (*str != '\0')
    {
        if(*str == Match){
            return i+atPos;
        }
        str++;
        i++;
    }

    return -1;
}
*/


int FindAt(const char *Source, int SourceLen, char Ch, int PosAt){
    for(int i = 0; i < SourceLen;i++){
        if(Source[i] == Ch){
            return i+PosAt;
        }
    }
    return ISINVALID;
}

bool Compare(const char *str, const char *Match1, int Match1Len)
{
    bool ret = true;
    for(int i = 0; i < Match1Len;i++){
        if(str[i] != Match1[i]){
            ret = false;
        }
    }
    return  ret;
}

int CString::FindPostion(const CString *Source, const char *Match1, int Match1Len)
{
    if(Source)
        return FindPostion(Source->Data(), Source->length(), Match1, Match1Len);

    return ZERO;
}

int CString::FindPostion(const char *Source, int SourceLen, const char *Match1, int Match1Len)
{
    int PosAt = 0;
    while(1){
        if((Match1Len+PosAt) > SourceLen){
            //match cant found
            //LOG("match not found 2");
            return ZERO;
        }

        PosAt = FindAt(Source+PosAt, SourceLen, Match1[0], PosAt);
        if(PosAt < ZERO){
            //cant found
            //LOG("match not found 1");
            return ZERO;
        }

        if((Match1Len+PosAt) > SourceLen){
            //match cant found
            //LOG("match not found 3");
            return ZERO;
        }

        //
        bool isMatch = Compare(Source+PosAt, Match1, Match1Len);
        if(isMatch){
            int StartAt = PosAt+Match1Len;
            return StartAt;
        }

        PosAt++;
    }

    return ZERO;
}

CString CString::SplitString(const char *Source, int SourceLen, const char *Match1, int Match1Len, const char *Match2, int Match2Len)
{
    CString ret;
    if((Match1 == nullptr && Match2 == nullptr) || (Match1 == "" && Match2 == "")){
        return ret;
    }

    int StartAt = CString::FindPostion(Source, SourceLen, Match1, Match1Len);
    int EnadAt = CString::FindPostion(Source+StartAt, SourceLen - StartAt, Match2, Match2Len);
    if(EnadAt > 0){
        EnadAt -= Match2Len;
    }

    if( StartAt > 0 && EnadAt > 0){
        ret.append(Source+StartAt, EnadAt);
        return ret;
    }

    if(StartAt > 0){
        if(EnadAt <=0){
            ret.append(Source+StartAt, SourceLen - StartAt);
        }
    }else if(StartAt == 0 && Match1Len == 0){
        if(EnadAt > 0){
            ret.append(Source, SourceLen - (SourceLen - EnadAt));
        }
    }
    //LOG("StartAt[%d]", StartAt);
    //LOG("EnadAt[%d]", EnadAt);
    return  ret;
}

CString CString::SplitString(const char *Source, int SourceLen, const char *Match1, const char *Match2)
{
    return  SplitString(Source, SourceLen, Match1, strlen(Match1), Match2, strlen(Match2));
}

CString CString::SplitString(const char *Source, const char *Match1, const char *Match2)
{
    return  SplitString(Source, strlen(Source), Match1, strlen(Match1), Match2, strlen(Match2));
}

CString CString::SplitString(const CString &Source, const char *Match1, const char *Match2)
{
    return  SplitString(Source.Data(), Source.length(), Match1, strlen(Match1), Match2, strlen(Match2));
}

bool CString::isExist(const char *text, const char *content)
{
    if(text == NULL || content == NULL)
    {
        return false;
    }

    if (strstr(content, text) != nullptr)   //strcasestr strstr
    {
        return true;
    }
    return false;
}

bool CString::isExistCase(const char *text, const char *content)
{
    if(text == NULL || content == NULL)
    {
        return false;
    }

    if (strcasestr(content, text) != nullptr)   //strcasestr strstr
    {
        return true;
    }
    return false;
}

bool CString::isExist(const char *text) const
{
    return isExist(text, Buffer);
}

bool CString::isExistCase(const char *text) const
{
    return isExistCase(text, Buffer);
}

bool CString::isEmpty()
{
    if(m_Lengh > 0){
        return false;
    }
    return true;
}

bool CString::isEmpty() const
{
    if(length() > 0){
        return  false;
    }
    return  true;
}


void CString::Free()
{
    if(Buffer != nullptr)
    {
        //printf("MyString::free[%s]\n", Buffer);
        free(Buffer);
        Buffer = nullptr;
    }
    ClearMembers();
}


void CString::Clear()
{
    Free();
    ClearMembers();
}

void CString::append(const uint8_t *data, uint16_t dataLength)
{
    if(data){
        append((const char *)data, dataLength);
    }
}

CString::CString(const CString &other) //Copy constructor
{
    /*
    Free();
    OutSize = other.OutSize;
    BufferLengh = other.BufferLengh;
    ResevSize = other.ResevSize;

    //Buffer = other.Buffer;
    Buffer = new char[BufferLengh+1];
    memcpy(Buffer, other.Buffer, other.BufferLengh);
    */

    ClearMembers();
    this->append(other.Data(), other.length());
}

CString::CString(const int Number)
{
    ClearMembers();
    this->append(Number);
}

CString::CString(const char *text)
{
    //printf("Copy constructor const char\n");
    /*
    if(OutSize != 0)
    {
        this->Free();
    }else{
        ClearMembers();
    }
    */
    ClearMembers();
    this->append(text);
}

CString::CString(const char* data, int dataLengh)
{
    if(!data)
        return;
    ClearMembers();
    this->append(data, dataLengh);
}

CString &CString::operator = (const CString &other) //assignment operator
{
    //printf("operator=\n");
    if (this != &other) {
        Free();
        m_Lengh = other.m_Lengh;
        m_ReserveSize = other.m_ReserveSize;

        //Buffer = other.Buffer;
        Buffer = new char[m_ReserveSize+1];
        Buffer[other.m_Lengh] = 0;
        memcpy(Buffer, other.Buffer, other.m_ReserveSize);
    }

    return *this;
}

CString &CString::operator = (const CString *other){
    if(other){
        if (this != other) {
            Free();
            this->append(other->Data(), other->length());
        }
    }
    return *this;
}

bool CString::operator == (const char *text)
{
    //printf("operator== %s\n", text);
    return isCompare(text, strlen(text));
}

bool CString::operator ==(const char *text) const
{
    return isCompare(text, strlen(text));
}


CString &CString::operator = (const char *text)
{
    if(text){
        //printf("operator= %s\n", text);
        this->Free();
        this->append(text);
        return *this;
    }
}

CString &CString::operator = (int Number)
{
    //printf("operator=\n");
    this->Free();
    this->append(Number);
    return *this;
}

CString &CString::operator +=(const char *text)
{
    if(text){
        append(text);
    }
}


CString &CString::operator +=(const CString &text)
{
    if(!text.isEmpty()){
        append(text);
    }
}

const char* CString::operator + (const char *text)
{
    if(text){
        //printf("operator+\n");
        this->append(text);
        return Buffer;
    }
}

void CString::Reset(bool setZeroBuffer)
{
    m_Lengh = 0;
    if(setZeroBuffer){
        memset(Buffer, 0, m_ReserveSize);
    }
}

CString & CString::operator + (const CString &other)
{
    //printf("operator+\n");
    this->append(other.Data());
    return *this;
}
CString & CString::operator + (const CString *other)
{
    //printf("operator+\n");
    this->append(other->Data());
    return *this;
}

CString &CString::operator << (const char *text)
{
    //printf("operator<<\n");
    this->append(text);
    return *this;
}

CString &CString::operator << (int Number)
{
    //printf("operator<< %d\n", Number);
    this->append(Number);
    return *this;
}



void CString::SetDataWithNewPointer(char* data, int dataLengh)
{
    if(!data || dataLengh == 0 || dataLengh == ISINVALID)
        return;

    if(m_Lengh > 0)
        Clear();

    m_Lengh = dataLengh;
    Buffer = data;
    //m_ReserveSize
}

void CString::append(const char* data, int dataLengh)
{
    if(!data || dataLengh == 0 || dataLengh == ISINVALID)
        return;

    if((m_Lengh+dataLengh) >= m_ReserveSize)
    {
        //new
        AddReserveSize(dataLengh);
    }

    //
    if(dataLengh == 1)
    {
        Buffer[m_Lengh] = *data;
    }else{
        memcpy(Buffer+m_Lengh, data, dataLengh);
    }

    m_Lengh += dataLengh;

    //printf("append: m_Lengh[%d],m_ReserveSize[%d], Buffer: [%s]\n\n\n", m_Lengh, m_ReserveSize, Buffer);
    //Buffer[m_Lengh] = 0;
}

void CString::append(const char* data)
{
    if(data){
        int dataLengh = strlen(data);
        this->append(data, dataLengh);
    }
}

void CString::append(uint32_t Number)
{
    char res[12] = {0};
    memset(res, 0, sizeof(res));
    int resSize = sprintf(res, "%u", Number);
    //printf("append: %s , %u\n", res2, tt);
    if(resSize > 0)
    {
        this->append(res, resSize);
    }
}

void CString::append(const CString &data)
{
    if(data.length() > 0){
        this->append(data.Data(), data.length());
    }

}

void CString::append(const CString *data)
{
    if(data)
        this->append(data->Data(), data->length());
}

bool CString::Copy(int32_t index, const void *Source, int32_t SourceSize)
{
    if(index > m_ReserveSize)
    {
        return false;
    }

    memcpy(Buffer+index, Source, SourceSize);
    return true;
}

const char* CString::Data()
{

    return Buffer;
}

const char* CString::Data() const {
    return Buffer;
}

int32_t CString::length()
{
    return m_Lengh;
}

int32_t CString::length() const {
    return m_Lengh;
}

CString *CString::GetThis()
{
    return this;
}

bool CString::isCompare(const CString *Source, const char *dest)
{
    if(Source == NULL || dest == NULL){

        return false;
    }

    return isCompare(Source->Data(), dest);
}

bool CString::isCompare(const char *Source, int SourceSize, const char *dest, int destSize)
{
    if(Source == NULL || dest == NULL || SourceSize <= 0 || destSize <= 0)
    {
        return false;
    }
    //strncmp()
    if(SourceSize != destSize)
        return false;

    //Compare
    if (SourceSize < destSize)
        return false;

    for(int i = 0; i < destSize; i++)
    {
        if(Source[i] != dest[i])
        {
            return false;
        }
    }

    return true;
}

bool CString::isCompare(const char *Source, const char *dest)
{
    if(Source == NULL || dest == NULL)
        return false;

    char *strSource = (char*)Source;
    char *strDest = (char*)dest;
    while (*strSource != '\0')
    {
        if(*dest == '\0')
            return false;

        if(*strSource != *strDest){
            return false;
        }
        strSource++;
        strDest++;
    }

    if(*strSource != *strDest){
        return false;
    }

    return true;
}


bool CString::ReadFile(const char *FilePath) {
    FILE *pfile = NULL;
    long length = 0;
    bool ret = false;
    size_t read_chars = 0;

    /* open in read binary mode */
    pfile = fopen(FilePath, "rb");
    if (pfile == NULL){
        //cannot open file
        return false;
    }

    /* get the length */
    if (fseek(pfile, 0, SEEK_END) == 0)
    {
        length = ftell(pfile);
        if (length > 0)
        {
            if (fseek(pfile, 0, SEEK_SET) == 0)
            {
                if (AddReserveSize(length) == true)
                {
                    /* read the file into memory */
                    read_chars = fread(Buffer+m_Lengh, sizeof(char), (size_t)length, pfile);
                    if ((long)read_chars == length)
                    {
                        ret = true;
                        m_Lengh += read_chars;
                        //Buffer[m_Lengh] = '\0';
                    }else{
                        Clear();
                    }
                }
            }
        }
    }

    fclose(pfile);
    return ret;
}

bool CString::SaveFile(const char *FilePath){
    FILE *pfile;
    pfile = fopen(FilePath, "wb");
    //a aapend
    //b binary
    //w write
    //a+
    //https://www.programiz.com/c-programming/c-file-input-output

    if(pfile == NULL){
        //cannot open file
        return false;
    }

    fwrite(Buffer, m_Lengh, sizeof(char), pfile);
    fclose(pfile);
    return true;
}

CString CString::replaceAll(const CString &buff, const CString &target, const CString &to){

    CString ret;
    ret.AddReserveSize(buff.length());

    int index = 0;
    int size = 0;


    for(;;){
        //LOG("data[%d]", buff.length() - index);
        int position = CString::FindPostion(buff.Data() + index, buff.length() - index, target.Data(), target.length()) ;
        if(position > 0){
            size = (position - target.length());
        }else{
            ret.append(buff.Data() + index, buff.length() - index);
            //LOG("end");
            break;
        }

        ret.append(buff.Data()+index, size);
        ret.append(to);
        index = index + position;

        /*
        LOG("position[%d]", position);
        LOG("size[%d]", size);
        LOG("index[%d]", index);
        */

    }

    //int position = CString::FindPostion(source, name, nameLength);
    return ret;
}

