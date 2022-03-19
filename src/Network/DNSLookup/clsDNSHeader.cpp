#include "clsDNSHeader.h"
#include "SocketHeader.h"
#include <algorithm> //replace
#include "clsCString.h"

/*
 * resource Full parseh OK hast vali behine nist
 * https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
 * */

clsDNSHeader::clsDNSHeader(void)
{
}

clsDNSHeader::~clsDNSHeader(void)
{
}

string clsDNSHeader::LongIPToString(int IP)
{
    string ret;
    struct in_addr addr;
    memcpy(&addr, (char*)&IP, sizeof(struct in_addr));
    char* strIP = inet_ntoa(addr);

    ret = strIP;
    return ret;
}

void Debug(const char* Buff, int Size)
{

    string PCK;
    for(int i = 0; i < Size;i++)
    {
        uint64_t n = (unsigned char)Buff[i];
        //97 122 65 90 48 57

        if((n >= 48 && n <= 57) || (n >= 65 && n <= 90) || (n >= 97 && n <= 122)){

            PCK.append("[");
            PCK.append(&Buff[i],1);
            PCK.append("] ");
        }else{
            char bytes[10] = {0};
            CString::NumberToString(n, bytes);
            PCK.append("[");
            PCK.append(bytes);
            PCK.append("] ");
        }


    }

    /*
    string PCK(Buff, Size);
    std::replace( PCK.begin(), PCK.end(), '\0', '.');
*/
    printf("Debug: Buffer[%s] PacketLength[%d] \n", PCK.c_str(), PCK.length());
}

int GetDNameCount(const unsigned char* reader, const unsigned char* buffer)
{

    int count = 1;
    unsigned int jumped = 0, offset;

    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192){
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }

        reader = reader+1;

        if(jumped==0){
            count += 1; //if we havent jumped to another location then we can count up
        }
    }

    if(jumped==1){
        count += 1; //number of steps we actually moved forward in the packet
    }
    return count;
}

u_char* ReadName(const unsigned char* reader, const unsigned char* buffer, int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;

    *count = 1;
    name = (unsigned char*)malloc(256);

    name[0]='\0';

    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }else{
            name[p++]=*reader;
        }

        reader = reader+1;

        if(jumped==0)
        {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }

    name[p]='\0'; //string complete
    if(jumped==1)
    {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }

    //now convert 3www6google3com0 to www.google.com
    for(i=0;i<(int)strlen((const char*)name);i++)
    {
        p=name[i];
        for(j=0;j<(int)p;j++)
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0'; //remove the last dot
    return name;
}

uint32_t clsDNSHeader::ParseDNSPaket(int HostConvertedLen, const unsigned char* Buff, int BuffLength)
{

    //Debug((const char*)Buff , BuffLength);

    //set headet
    QUERY Query;
    memset(&Query,0 , sizeof(QUERY));
    memcpy((void*)&Query.Header, Buff, DNS_HEADER_SIZE);

    //printf("\nHeader: ID[%d], Answers Count[%d], ARCount[%d], Answers[%d], QR[%d], Questions[%d], Opcode: %d\n", ntohs(Query.Header.ID), ntohs(Query.Header.AnswerCount), ntohs(Query.Header.AdditionalCount), ntohs(Query.Header.AuthorityCount), (Query.Header.QueryFlag), ntohs(Query.Header.QuestionCount), (Query.Header.Opcode) );

    //unsigned char *reader = (unsigned char*)Buff + (DNS_HEADER_SIZE + (HostConvertedLen) + QUESTION_SIZE);

    //int stop = 0;
    int Read = (DNS_HEADER_SIZE + (HostConvertedLen) + QUESTION_SIZE);
    if (Read >= BuffLength)
        return 0;


    for(int i = 0; i < ntohs(Query.Header.AnswerCount);i++)
    {
        //Debug((const char*)Buff+Read, 100);
        //int stop;
        //u_char * dm= ReadName(Buff+Read, Buff, &stop);
        //printf("ReadName[%s][%d]\n", dm , stop);
        int count = GetDNameCount(Buff+Read, Buff);
        Read += count;
        if (Read >= BuffLength)
            return 0;

        //check buffer overflow
        if (Read+R_DATA_SIZE > BuffLength){
            return 0;
        }

        //set resurce data
        memcpy((unsigned char*)&Query.resource, Buff+Read, R_DATA_SIZE);
        Read += R_DATA_SIZE;

        //printf("type[%d], ttl[%d], class[%d], data_len[%d]\n", ntohs(Query.resource.type), ntohs(Query.resource.ttl), ntohs(Query.resource._class), ntohs(Query.resource.data_len));

/*
        memcpy((unsigned char*)&Query.Question, reader, QUESTION_SIZE);
        reader += QUESTION_SIZE;
        R += QUESTION_SIZE;
        //if(r >= BuffLength) return 0;

        //set resurce data
        memcpy((unsigned char*)&Query.resource, reader, R_DATA_SIZE);
        reader += R_DATA_SIZE;
        R += R_DATA_SIZE;
*/

        if(ntohs(Query.resource.type) == T_A){

            //check buffer overflow
            if (Read+IPv4_SIZE > BuffLength){
                return 0;
            }

            //set IPv4 Address
            memcpy((unsigned char*)&Query.IP, Buff+Read, IPv4_SIZE);
           // Read += IPv4_SIZE;
           // printf("IP[%s]\n", inet_ntoa(*(struct in_addr*)&Query.IP));


            Read += ntohs(Query.resource.data_len);
            //continue;
            break;
        }else{

            //ReadName(reader, Buff, &stop);
            Read += ntohs(Query.resource.data_len);
            continue;
        }
    }




    uint32_t intIP = Query.IP;
    return intIP;
}



