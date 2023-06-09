#ifndef CLSPIPE_H
#define CLSPIPE_H

#include <iostream>
#include <thread>

class clsPipe
{
    int m_svSock;
    std::thread m_Thread;

protected:
        void _onReading();
        static void setSocketPath(struct sockaddr_un *pSockAddr, const char *socketName);

public:
    clsPipe();
    virtual ~clsPipe();
    bool Listen(const char* socketName);
    void StopListen();
    static int CreateReceiver(const char *socketName);

    static void SetSocketResourceAddr(int fd, bool isEnable);
    static void SetSocketShared(int fd, bool isEnable);
    static void SetTimeout(int fd, int TimeOutSec);

    static void onThread(void *p);
    void killThread();

    static void SendMessage(const char *socketName, const char*msg);
    static void SendMessage(const char *socketName, const char*msg, int msgLenth);
    static std::string ReceiveMessage(const char *socketName,int sock, int Timeout = 17);
    //
    virtual void OnPipeReceiveMessage(const char*msg, int msgLenth);

};

#endif // CLSPIPE_H
