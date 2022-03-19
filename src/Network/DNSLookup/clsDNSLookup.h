#ifndef CLSDNSLOOKUP_H
#define CLSDNSLOOKUP_H

#include "clsCString.h"
#define DNSLOOKUP_TIMEOUT (6)

class clsDNSLookup
{
public:
    clsDNSLookup();
    static CString DNSLookup(const char *TargetHost, int Timeout = DNSLOOKUP_TIMEOUT, const char *DNSServerIP = "8.8.8.8", int DNSServerPort = 53);
};

#endif // CLSDNSLOOKUP_H
