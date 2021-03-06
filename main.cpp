

#ifdef libWrenchApp

#include <stdio.h>
#include "clsTCPServer.h"
#include "clsTCPSocket.h"
#include <string>
#include <clsCString.h>
#include <clsDNSLookup.h>
#include <log.h>
#include "Base64/clsBase64.h"
#include "Files/clsFileDirectory.h"

//get sockets
//netstat -n | awk '/^tcp/ {t[$NF]++}END{for(state in t){print state, t[state]} }'
using namespace std;
clsTCPServer Server;
bool enableLinger = false;

static void onAccepClient(clsTCPListener *pListener, void *pParent, int NewSocket)
{
    LOG("NewSocket(%d)", NewSocket);

    clsTCPSocket *newConnection = new clsTCPSocket(&Server);
    newConnection->Accept(NewSocket);

    if(enableLinger)
        clsTCPSocket::SetSocketLinger(NewSocket, 2);   //mohem baraye http

    /*
    newConnection->Send("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: Closed\n\nlibTCPSERVER");
    /*
    for(int i = 0; i < 1000;i++){
        newConnection->Send((char*)newConnection->GetClientIP());
        newConnection->Send("<br>");
    }

    newConnection->Send("end");
    newConnection->Close();
    */
}

#include "clsHash.h"

void CString_Test(){
    CString A;
    for(int i = 0;i < 10000;i++){

        A.append(i);
        A.append("aaa", i);

        CString B;
        B.AddReserveSize(5);
        B.append("goozaaaaaaaaaaaaa");
    }


}

static void onThread1(clsThread *pTherad, void* pArg){
    CString_Test();
}



int main(int ac, char **av)
{
/*
    FileDirectory::ReadFile("/gooz");

    LOG("aaa");
    CString a = FileDirectory::GetCurrentDir();

    LOG("GetCurrentDir1: [%s]", a.Data());
    LOG("GetCurrentDir2: [%s]", av[0]);


    for(;;){
        getchar();
        const char *path = "./www/img/AlGBeo.jpg";

        LOG("GetCurrentDir: [%s]", FileDirectory::GetCurrentDir().Data());
        LOG("GetFullProgramPath: [%s]", FileDirectory::GetFullProgramPath().Data());

        LOG("isDirectory[%s][%d]", path , FileDirectory::isDirectory(path));
        LOG("isExistDirOrFile[%s][%d]", path , FileDirectory::isExistDirOrFile(path));

        chdir("..");

    }


    return 0;
*/


/*
    for(int i = 0; i < 100;i++){
        //clsThread::CreateThread(onThread1, nullptr, 0);
        CString_Test();
        //uint32_t RandomNum = cryptography::clsHash::GenerateRandomNumber(1, 10);   //5 sec ta 15 min
        //LOG("random Time sleep : [%d]", RandomNum);
    }
    LOG("Done");
    getchar();
    return 0;
*/

    /*
    for(int i = 0; i < 255;i++){
        unsigned char A = i;
        uint64_t sor = A ;
        char Dest[10] = {0};
        int resSize = sprintf(Dest, "%lu", sor);
        LOG("sprintf(%s)", Dest);

    }
 */

    /*
    CString PoolAdr = "google.com:443";
    if( PoolAdr.isExist(":") ){
        CString port = CString::SplitString(PoolAdr, ":", "");
        LOG("port! [%s]", port.Data());
    }

    const char* Buffer = "{\"id\":1, \"result\": [[[\"mining.set_difficulty\", \"87c466ed54713c0a\"], [\"mining.notify\", \"87c466ed54713c0a\"]], \"a74ad2a1\", 4], \"error\": null}";
    if(CString::isExist("\"id\": 1,", Buffer) == true || CString::isExist("\"id\":1,", Buffer) == true){
        LOG("isExist! [%s]", Buffer);

    }

*/




    //ret = FileDirectory::CopyFile("/etc/passwd", "/home/crow/Desktop/test.txt");
    //ret =  FileDirectory::SetExecutePermission("/etc/shadow-");
    //LOG("CopyFile(%d)", ret);


    getchar();

    clsTCPSocket *TCP = new clsTCPSocket(&Server);

    /*
    for(int i = 0; i < 100;i++){

        TCP->ConnectToHost("client.openwrt-upgrade.net", 3333, TIMEOUT_66Sec);
        getchar();
        TCP->Close(false);
    }

    getchar();

    //TCP->Send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    getchar();
    //PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n
    return 0;




    getchar();
*/
    Server.AddNewListener(8888, "127.0.0.1", nullptr, onAccepClient);
    Server.AddNewListener(8080, nullptr, nullptr, onAccepClient);

    getchar();
    enableLinger = true;
    getchar();
    return 0;
}

#endif
