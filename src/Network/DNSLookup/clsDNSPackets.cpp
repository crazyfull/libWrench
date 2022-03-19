#include "clsDNSPackets.h"
#include "clsDNSHeader.h"
#include "SocketHeader.h"
//#include "cryptography/clsHash.h"

clsDNSPackets::clsDNSPackets(void)
{
}

clsDNSPackets::~clsDNSPackets(void)
{
}

void ChangetoDnsNameFormat(unsigned char* Qutput, const char* HostName, int HostLentgh)
{
	int lock = 0;
    int Lentgh = HostLentgh+1;
	unsigned char host[127] = {0};
    memcpy(host, HostName, Lentgh);
    host[HostLentgh] = '.';
    //strcat((char*)host, ".");

    for(int i = 0 ; i < Lentgh ; i++)
	{
		if(host[i] == '.')
		{
			//*dns ++= i-lock;
            *Qutput ++=  i-lock ;
			for(;lock < i; lock++)
			{
                *Qutput ++= host[lock];
			}
			lock++; //or lock=i+1;
		}
	}
    *Qutput++='\0';
}

string clsDNSPackets::PckRequestA(const char* HostName, int HostLentgh)
{
	//change format host
    //int HostLentgh = strlen(HostName)+1;
	unsigned char OuHost[127] = {0};

    ChangetoDnsNameFormat(OuHost, HostName, HostLentgh);

	//
	DNS_HEADER Header;
    Header.ID = htons(5001);            //cryptography::clsHash::GenerateRandomLongNumber(1, 65530);// htons(5000);	// random
	Header.QueryFlag = 1;				// request
    Header.Opcode = 0;
    Header.QuestionCount = htons(1);	//request count
    Header.AnswerCount = 0;
	Header.AuthorityCount = 0;
	Header.AdditionalCount = 0;

	//
	QUESTION Question;
    Question.QuestionClass = htons(1);		// ipv4 address version
    Question.QuestionName = htons(1);		// inetrnet

	string Pck;
	Pck.append((char*)&Header, sizeof(Header));
    Pck.append((char*)&OuHost, HostLentgh+2);
	Pck.append((char*)&Question, sizeof(Question));

	return Pck;
}
