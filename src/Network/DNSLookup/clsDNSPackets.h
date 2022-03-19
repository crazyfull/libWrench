#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

class clsDNSPackets
{
public:
	clsDNSPackets(void);
	~clsDNSPackets(void);

    static string PckRequestA(const char* HostName, int HostLentgh);
};
