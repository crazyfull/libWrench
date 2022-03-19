#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

#define T_A 1       //Ipv4 address
#define T_NS 2      //Nameserver
#define T_CNAME 5   // canonical name
#define T_SOA 6     /* start of authority zone */
#define T_PTR 12    /* domain name pointer */
#define T_MX 15     //Mail server

#define R_DATA_SIZE 10
#define QUESTION_SIZE 4
#define DNS_HEADER_SIZE 12
#define IPv4_SIZE 4

struct DNS_HEADER
{
    unsigned short ID;                  // identification number	// 2 bytes
    unsigned char QueryFlag;            // query/response flag		// 0/1
    unsigned char Opcode;
    unsigned short QuestionCount;       // number of Question Count				// 2 bytes
    unsigned short AnswerCount;         // number of Answer Record Count		// 2 bytes
    unsigned short AuthorityCount;		// number of Authority Record Count		// 2 bytes
    unsigned short AdditionalCount;     // number of Additional Record Count	// 2 bytes

};

struct QUESTION
{
    unsigned short QuestionName;	// Question Name
    unsigned short QuestionClass;   // Question Class
};

#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    R_DATA resource;
    unsigned char *rdata;
};

struct QUERY
{
    DNS_HEADER Header;
    R_DATA resource;
    unsigned int IP;
};

class clsDNSHeader
{
public:
    clsDNSHeader(void);
    ~clsDNSHeader(void);

    static uint32_t ParseDNSPaket(int HostConvertedLen, const unsigned char *Buff, int BuffLength);
    string LongIPToString(int IP);
};
