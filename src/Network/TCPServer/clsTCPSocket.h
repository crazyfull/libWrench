#ifndef CLSTCPSOCKET_H
#define CLSTCPSOCKET_H
#include "SocketHeader.h"
#include "clsIDType.h"
#include "clsCString.h"

//5 66sec , 4 32sec, 3 15sec, 2 7sec, 1 3sec

//****************************************************************
//  size khoob baraye tozihat socket optiona
//  https://notes.shichao.io/unp/ch7/
//****************************************************************

enum TCPConnectTimeout
{
    TIMEOUT_3Sec = 1,
    TIMEOUT_7Sec = 2,
    TIMEOUT_15Sec = 3,
    TIMEOUT_32Sec = 4,
    TIMEOUT_66Sec = 5,
    TIMEOUT_132Sec = 6
};

class clsTCPServer;
class clsTCPSocket: public clsIDType
{
public:
    //variable
    enum TCPSocketStatus {Closed = 1
                          , Closing = 2
                          , Connecting = 3
                          , Accepting = 4
                          , Connected = 5
                         };

private:
    //variable

    int m_socket;
    TCPSocketStatus Status;
    in_addr ClientIP;
    epoll_event m_Event;
    clsTCPServer *m_pServer;

    //send files var
    int m_fdFile;
    off64_t m_offest = 0;      //mybe need offext64

    //functions
    void _SetStatus(const TCPSocketStatus &value);
    bool ModifyEvent();
    void CloseFile();

public:
    clsTCPSocket(clsTCPServer *pServer);
    virtual ~clsTCPSocket();

    //public functions
    //functions
    static in_addr GetIPFromHostAddress(const char *HostAddress);
    static int GetSocketReceiveBuffer(int fd);
    static int GetSocketSendBuffer(int fd);
    static int GetSocketConnectTimeout(int fd);
    static void SetSocketReceiveBuffer(int fd, int bufferSize);
    static void SetSocketSendBuffer(int fd, int bufferSize);
    static void SetSocketConnectTimeout(int fd, TCPConnectTimeout Timeout);
    static void SetSocketResourceAddr(int fd, bool isEnable);
    static void SetSocketShared(int fd, bool isEnable);
    static void SetSocketNonBlocking(int fd);
    static void SetSocketBlockingMode(int fd);
    static void SetSocketNoDelay(int fd, bool isEnable);
    static void SetSocketKeepAlive(int fd, bool isEnable);
    static void SetSocketLinger(int fd, int Timeout);
    static bool SetSocketSendAndReceiveTimeOut(int fd, int secTime);
    static in_addr GetSockaddrFromSocket(int sock);

    bool Accept(int new_socket);
    void ConnectToHost(const char* HostAddress, uint16_t Port, TCPConnectTimeout Timeout = TIMEOUT_66Sec);
    ssize_t Send(const char* Buffer, size_t Length);
    ssize_t Send(const char* Buffer);
    ssize_t SendFile(int fileFD);
    void ResumeSendFile();
    void Close(bool isShutdown = false);

    TCPSocketStatus GetStatus() const;
    int GetSocket() const;

    CString GetClientIPString();
    uint32_t GetClientIP();
    clsTCPSocket* GetTCPSocket() const;
    bool isReadyForDelete() const;
    bool isSocketHaveError();
    bool isReadyForClose() const;

    //method
    void onConnected();
    void onReceiving();

    //overides
    virtual void OnConnectFailed(const char* msg, int errCode);
    virtual void OnConnecting();
    virtual void OnConnected();
    virtual void OnAccepted();
    virtual void OnReceiveData(const char* Buffer, int Length);
    virtual void OnClosed();
    virtual void OnSendFileStatus(ssize_t size);
    virtual void OnSendFileComplete();


    void CloseFinal();
    epoll_event *Event();


};

#endif // CLSTCPSOCKET_H
