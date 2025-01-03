#include "clsPipe.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>

#define BUFFER_SIZE (1 * 1024)
#define SOCKET_PATH ("/tmp/")

clsPipe::clsPipe()
{
    m_svSock = 0;
}

clsPipe::~clsPipe()
{
    killThread();
}

void clsPipe::OnPipeReceiveMessage(const char *msg, int msgLenth)
{
    //
    //DebugPrint("OnReceiveMessage: [%s]\n", msg);
    //SendMessage("pipebache", "hi from server");
}


void clsPipe::SetSocketResourceAddr(int fd, bool isEnable)
{
    //Allow local address reuse
    int iOption = 0;
    if(isEnable)
        iOption = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&iOption, sizeof(iOption));
}

void clsPipe::SetTimeout(int fd, int TimeOutSec){
    //set Timeouts
    struct timeval tv;
    tv.tv_sec = TimeOutSec;
    tv.tv_usec = 0;

    int isErr = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0) {
        DebugPrint("error setsockopt set flag SO_RCVTIMEO, error[%d]\n", errno);
    }

    isErr =  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(timeval));
    if(isErr != 0) {
        DebugPrint("error setsockopt set flag SO_SNDTIMEO, error[%d]\n", errno);
    }
}

void clsPipe::SetSocketShared(int fd, bool isEnable)
{
    //Allow local port reuse
    int iOption = 0;
    if(isEnable)
        iOption = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char*)&iOption, sizeof(iOption));

}

void clsPipe::setSocketPath(sockaddr_un *pSockAddr, const char *socketName)
{
    memcpy(pSockAddr->sun_path, SOCKET_PATH, strlen(SOCKET_PATH));
    memcpy(pSockAddr->sun_path + strlen(SOCKET_PATH), socketName, strlen(socketName));
}


bool clsPipe::Listen(const char *socketName)
{
    if(m_svSock!=0){
        DebugPrint("socket is not close\n");
        return false;
    }


    if(m_Thread.native_handle() != 0)
        killThread();

    struct sockaddr_un addr;


    // Bind the socket to a path
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;  //AF_LOCAL; //AF_UNIX;
    setSocketPath(&addr, socketName);

    // Create a socket
    m_svSock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (m_svSock == -1) {
        DebugPrint("can no create socket");
        return false;
    }

    SetSocketResourceAddr(m_svSock, true);
    SetSocketShared(m_svSock, true);


    remove(addr.sun_path);
    if (bind(m_svSock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        DebugPrint("cann not bind");
        StopListen();
        return false;
    }

    // Read data from the client
    m_Thread = std::thread(onThread, this);
    //onThread(this);
    return true;
}

void clsPipe::StopListen()
{
    close(m_svSock);
    m_svSock = 0;
}


void clsPipe::onThread(void *p)
{
    clsPipe *pThis = static_cast<clsPipe*>(p);
    pThis->_onReading();

}

void clsPipe::killThread(){
    std::thread::native_handle_type thID = m_Thread.native_handle();

#ifdef _WIN32
    //in win32
    if(thID){
        TerminateThread(thID, 1);
        m_Thread.join();
    }
#else
    if(thID > 0){
        pthread_cancel(thID);
        m_Thread.join();
    }
#endif
}

void clsPipe::SendMessage(const char *socketName, const char *msg)
{
    SendMessage(socketName, msg, strlen(msg));
}

void clsPipe::SendMessage(const char *socketName, const char *msg, int msgLength) {
    int sockfd;
    struct sockaddr_un addr;

    // create socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        DebugPrint("can not create socket: [%s]", strerror(errno));
        return;
    }

    SetSocketResourceAddr(sockfd, true);
    SetSocketShared(sockfd, true);

    //
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    setSocketPath(&addr, socketName);

    // send msg
    ssize_t bytes_written = sendto(sockfd, msg, msgLength, MSG_CONFIRM, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_written == -1) {
        DebugPrint("sendto error: [%s]", strerror(errno));
    } else if (bytes_written != msgLength) {
        DebugPrint("partial message sent, expected: [%d], sent: [%zd]", msgLength, bytes_written);
    }

    //
    if (close(sockfd) != 0) {
        DebugPrint("socket: [%d] failed to close socket: [%s]", sockfd, strerror(errno));
    }
}


int clsPipe::CreateReceiver(const char *socketName) {
    int recSock = 0;
    struct sockaddr_un addr;

    //
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    setSocketPath(&addr, socketName);

    //
    recSock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (recSock == -1) {
        DebugPrint("can not create socket: [%s]", strerror(errno));
        return -1;
    }

    SetSocketResourceAddr(recSock, true);
    SetSocketShared(recSock, true);

    //
    remove(addr.sun_path);

    //
    if (bind(recSock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        DebugPrint("can not bind: [%s]", strerror(errno));
        if (close(recSock) != 0) {
            DebugPrint("socket: [%d] failed to close socket: [%s]", recSock, strerror(errno));
        }
        return -1;
    }

    return recSock;
}

std::string clsPipe::ReceiveMessage(const char *socketName, int sock, int Timeout) {
    std::string ret;
    SetTimeout(sock, Timeout);

    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE + 1] = {0};

    ssize_t bytes_read = recvfrom(sock, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&client_addr, &client_len);

    if (bytes_read == -1) {
        DebugPrint("sockd: [%d] recvfrom error: [%s]", sock, strerror(errno));
    } else if (bytes_read == 0) {
        DebugPrint("sockd: [%d] recvfrom returned 0 (connection closed)", sock);
    } else {
        buffer[bytes_read] = 0;
        ret.append(buffer, bytes_read);
    }

    std::string sockpath = SOCKET_PATH;
    sockpath.append(socketName);

    if (remove(sockpath.c_str()) != 0) {
        DebugPrint("sockd: [%d] failed to remove socket file: [%s]", sock, strerror(errno));
    }

    if (close(sock) != 0) {
        DebugPrint("sockd: [%d] failed to close socket: [%s]", sock, strerror(errno));
    }

    return ret;
}



void clsPipe::_onReading()
{
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);

    for(;;){
        char buffer[BUFFER_SIZE+1] = {0};
        ssize_t bytes_read = recvfrom(m_svSock, buffer, BUFFER_SIZE, MSG_WAITALL,  (struct sockaddr *)&client_addr, &client_len);
        if (bytes_read == -1 || bytes_read == 0) {
            DebugPrint("sockd: [%d] recvfrom error: [%s]", m_svSock, strerror(errno));
            break;
        }else{
            buffer[bytes_read] = 0;
            OnPipeReceiveMessage(buffer, bytes_read);
        }
    }

    // Close the server socket
    StopListen();
}





