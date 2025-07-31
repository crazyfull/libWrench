//#include "Network/UDPClient/UDPClient.h"
#include <openssl/crypto.h>
#include <random>
#ifdef libWrenchApp

#include <stdio.h>
#include "clsTCPServer.h"
#include "clsTCPSocket.h"
#include "clsUDPSocket.h"
#include "clsUDPServer.h"
#include "Network/pipe/clsPipe.h"
#include <string>
#include <clsCString.h>
#include <clsDNSLookup.h>
#include <log.h>
#include "clsHash.h"
#include "UDPClient.h"
#include "clsFileDirectory.h"

//get sockets
//netstat -n | awk '/^tcp/ {t[$NF]++}END{for(state in t){print state, t[state]} }'
using namespace std;
//clsTCPServer Server(800);
clsUDPServer UDPserver(1000);
clsUDPSocket m_UDPClient;
bool enableLinger = false;


static void onAccepClient(clsTCPListener *pListener, void *pParent, int NewSocket)
{
    LOG("NewSocket(%d)", NewSocket);
    /*
    clsTCPSocket *newConnection = new clsTCPSocket(&Server);
    if(newConnection->Accept(NewSocket, true) == false){

        LOG("Accept(%s)", newConnection->GetSSLError().Data());
    }
    return;

    if(enableLinger)
        clsTCPSocket::SetSocketLinger(NewSocket, 2);   //mohem baraye http




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

// typedef void (*UDPCallbackType)(clsUDPListener*, void*, clsUDPClient&, const char*Data, int DataLength);
static void onReceiveUDPData(clsUDPListener* pListener, void*p, clsUDPSocket& UDPClient, const char*Data, int DataLength){
    LOG("buffer[%s] udpclient[%s:%d]", Data, UDPClient.getIPAddress(), UDPClient.getPort());

    //replay
    UDPClient.Send("echo from wrench");
    m_UDPClient = UDPClient;
}

void udp_main(){


    //https://www.ipvoid.com/udp-port-scan/
    UDPserver.AddNewListener(1813, nullptr, nullptr, onReceiveUDPData);
    UDPserver.Start();
    getchar();
    for(;;){
        m_UDPClient.Send("hi from wrench");
        getchar();
    }

    exit(0);
}



int main1(int ac, char **av)
{
    CString ip = clsTCPServer::getPrimaryIPAddress() ;

    LOG("ip: [%s]", ip.Data());
    return 0;

    // udp_main();
    UDPClient udpClient;
    udpClient.setHost("198.244.189.153");
    udpClient.setPort(40001);
    udpClient.setTimeOut(3600);
    string buff = udpClient.SendReceive("gooz");



    //test compare

    //strncmp(a,b);
    LOG("udpClient: [%s]", buff.c_str());
    return 0;

    /*
    if(FileDirectory::CreateDirectory("gooz")){
        LOG("CreateDirectory OK");

    }else{
        LOG("CreateDirectory failed");
    }

    return 0;


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


    //getchar();
    /*
    Server.SetSocketLinger(3);                   //3 sec
    Server.SetSocketKeepAlive(true);             //3 min
    Server.SetSocketSendAndReceiveTimeout(60);    //6 sec
    */

    LOG("openssl version: (%lu), %s", SSLeay(), SSLeay_version(SSLEAY_VERSION));


    /*
    clsTCPSocket *TCP = new clsTCPSocket(&Server);
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
    /*
    //Server.AddNewListener(8888, "127.0.0.1", nullptr, onAccepClient);
    Server.AddNewListener(8080, nullptr, nullptr, onAccepClient);
    Server.SetCertificateSSL("/home/crow/Downloads/cert/fullchain.cer",
                             "/home/crow/Downloads/cert/sv2.mojz.ir.key");


    Server.SetSSLMethod(SSLV2_3);
    Server.Start();
*/
    getchar();
    enableLinger = true;
    getchar();
    return 0;
}

//example to use
//pipe main
#include <sys/wait.h>

int rrandom(){
    std::random_device rd;
    std::mt19937 gen(rd());

    // تعریف محدوده توزیع
    std::uniform_int_distribution<> dist(1000, 10000);

    // تولید عدد تصادفی
    int random_num = dist(gen);
    return random_num;
}

void SendReceive(){
    //client
    LOG("sending message to server...");
    //create uniq socket name
    CString requestID = cryptography::clsHash::GenerateRandomString(16);

    //create requestID
    int ReceiverFD = clsPipe::CreateReceiver(requestID.Data());
    LOG("ReceiverFD: %d", ReceiverFD);


    //cretae packet
    string pck = requestID.Data();
    //send packet with pipe to pipe server
    clsPipe::SendMessage("RadiuSSHParent", pck.c_str(), pck.length());

    //receive answer message from pipe server
    string msg = clsPipe::ReceiveMessage(requestID.Data(), ReceiverFD, 2);  // 5secound
    LOG("the message server [%s]", msg.c_str());
}

int main(int ac, char **av)
{
    pid_t pid = fork();
    if (pid == -1) {
        DebugPrint("Failed to fork: %s", strerror(errno));
        return 1;
    }

    //server
    if (pid == 0) {
        sleep(1);

        for(int i = 0; i < 3; i++){
            //LOG("loop()");
            if(fork() == 0){

                while(1) {
                    LOG("Child process created (PID: %d, Parent PID: %d)", getpid(), getppid());
                    //usleep(rrandom());
                    SendReceive();
                    //usleep(100);
                }
                //sleep(1);
                exit(0);

            }else{

                //
                //SendReceive();
                wait(NULL);
                exit(0);
            }

        }

    }else{

        clsPipe srver;
        for(;;){
            srver.Listen("RadiuSSHParent");
            sleep(2);
            srver.StopListen();
            //sleep(1);
            //srver.Listen("RadiuSSHParent");

        }

        getchar();
        wait(NULL);
        LOG("end listener...");
    }


}


#endif
