#ifndef CLSPIPE_H
#define CLSPIPE_H

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

#define PIPE_BUFFER_SIZE (1 * 1024)
//#define SOCKET_PATH ("/tmp/")
#define SOCKET_BASE_DIR ("/tmp/.libwrench/")


class clsPipe
{
    int m_listenSock;
    std::thread m_Thread;
    std::atomic<bool> m_bRunning;
    std::string m_socketName;
    std::mutex m_onReadingMutex;

private:
    void createDirectory();
protected:
    void _onListenerThread();
    static void setSocketPath(struct sockaddr_un *pSockAddr, const char *socketName);

public:
    clsPipe();
    virtual ~clsPipe();
    void Listen(const char* socketName);
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

    bool isRunning() const;
};

#endif // CLSPIPE_H
