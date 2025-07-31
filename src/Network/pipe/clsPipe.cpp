#include "clsPipe.h"
#include "log.h"
#include "clsFileDirectory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>


clsPipe::clsPipe(): m_listenSock(0), m_bRunning(false)
{
    createDirectory();
}

clsPipe::~clsPipe()
{
    killThread();
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


void clsPipe::setSocketPath(struct sockaddr_un *pSockAddr, const char *socketName) {
    std::string fullPath = std::string(SOCKET_BASE_DIR) + socketName;
    if (fullPath.length() >= sizeof(pSockAddr->sun_path)) {
        DebugPrint("Socket path too long!");
        throw std::runtime_error("Socket path too long");
    }
    strncpy(pSockAddr->sun_path, fullPath.c_str(), sizeof(pSockAddr->sun_path) - 1);
}

/*
void clsPipe::setSocketPath(struct sockaddr_un *pSockAddr, const char *socketName) {
    memset(pSockAddr->sun_path, 0, sizeof(pSockAddr->sun_path));
    pSockAddr->sun_path[0] = '\0'; // abstract namespace
    strncpy(&pSockAddr->sun_path[1], socketName, sizeof(pSockAddr->sun_path) - 2);
}
*/

bool clsPipe::isRunning() const
{
    return m_bRunning;
}

void clsPipe::createDirectory(){

    if (FileDirectory::isDirectory(SOCKET_BASE_DIR) == false) {
        if (FileDirectory::CreateDirectory(SOCKET_BASE_DIR, 0700) == false) {
            DebugPrint("failed to create socket directory: [%s]", strerror(errno));
        }else{
            //
            if (chown(SOCKET_BASE_DIR, getuid(), getgid()) < 0) {
                DebugPrint("chown failed");
                throw std::runtime_error("chown failed");

            }
        }
    }
}

void clsPipe::Listen(const char *socketName)
{
    if (m_bRunning) {
        DebugPrint("Listener is already running. Please stop it before restarting.");
        return;
    }

    if (m_listenSock != 0){
        DebugPrint("socket is not closed\n");
        return;
    }

    if (m_Thread.joinable())
        killThread();

    m_socketName = socketName;
    m_Thread = std::thread([this]() {
        this->_onListenerThread();
    });
    m_bRunning = true;
    sleep(1);
}

void clsPipe::_onListenerThread() {
    while (m_bRunning) {
        sleep(1);

        struct sockaddr_un addr;
        // Bind the socket to a path
        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;  //AF_LOCAL; //AF_UNIX;
        setSocketPath(&addr, this->m_socketName.c_str());

        //set to varialbe
        //m_socketPath = addr.sun_path;

        // Create a socket
        m_listenSock = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0); //| SOCK_CLOEXEC
        if (m_listenSock == -1) {
            DebugPrint("can not create socket");
            continue;
        }

        SetSocketResourceAddr(m_listenSock, true);
        SetSocketShared(m_listenSock, true);

        if (unlink(addr.sun_path) != 0 && errno != ENOENT) {
            DebugPrint("can't unlink socket: [%s]", strerror(errno));
            close(m_listenSock);
            m_listenSock = 0;
            continue;
        }


        if (bind(m_listenSock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
            DebugPrint("can not bind");
            //StopListen();
            continue;
        }

        fd_set readfds;
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[PIPE_BUFFER_SIZE + 1];

        const int timeout_seconds = 10;  // set timeout

        while (m_bRunning) {
            FD_ZERO(&readfds);
            FD_SET(m_listenSock, &readfds);

            struct timeval tv;
            tv.tv_sec = timeout_seconds;
            tv.tv_usec = 0;

            int ret = select(m_listenSock + 1, &readfds, NULL, NULL, &tv);
            if (ret == -1) {
                if (errno == EINTR) {
                    // سیگنالی باعث قطع select شده، قابل چشم‌پوشیه
                    continue;
                }

                // خطای select
                perror("select error");
                DebugPrint("select error: [%s]", strerror(errno));
                close(m_listenSock);
                m_listenSock = -1;
                break; // یا continue، بسته به شرایط برنامه
            } else if (ret == 0) {
                // timeout: هیچ داده‌ای برای خواندن نبود
                // اینجا می‌تونی کاری بکنی مثل چک کردن وضعیت یا صرفاً حلقه رو ادامه بدی
                continue;
            }

            if (FD_ISSET(m_listenSock, &readfds)) {
                ssize_t bytes_read = recvfrom(m_listenSock, buffer, PIPE_BUFFER_SIZE, 0,  (struct sockaddr*)&client_addr, &client_len);
                if (bytes_read < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // داده‌ای نیست، دوباره ادامه بده
                        continue;
                    }
                    perror("recvfrom error");
                    DebugPrint("recvfrom error: [%s]", strerror(errno));
                    break;
                } else if (bytes_read == 0) {
                    // اتصال بسته شده (برای سوکت دایرکتوری احتمال کمتر)
                    DebugPrint("recvfrom returned 0, connection closed? error: [%s]", strerror(errno));
                    break;
                }

                buffer[bytes_read] = '\0';

                // اگر نیاز به محافظت همزمانی هست:
                {
                    std::lock_guard<std::mutex> lock(m_onReadingMutex);
                    OnPipeReceiveMessage(buffer, bytes_read);
                }
            }
        }

    }

}

void clsPipe::StopListen() {
    if (!m_bRunning)
        return;

    DebugPrint("StopListen()");
    m_bRunning = false;

    if (m_listenSock > 0) {
        close(m_listenSock);
        m_listenSock = 0;
    }

    if (m_Thread.joinable())
        m_Thread.join();

    sleep(1);
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

    if (access(addr.sun_path, F_OK) != 0) {
        DebugPrint("sendto skipped: socket path does not exist [%s]", addr.sun_path);
        close(sockfd);
        return;
    }

    // send msg
    ssize_t bytes_written = sendto(sockfd, msg, msgLength, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_written == -1) {
        DebugPrint("sendto error: socketName[%s] path[%s] errno[%s]", socketName, addr.sun_path ,strerror(errno));
    } else if (bytes_written != msgLength) {
        DebugPrint("partial message sent, expected: [%d], sent: [%zd]", msgLength, bytes_written);
    }

    //DebugPrint("sendto debug: socketName[%s] path[%s] errno[%s]", socketName, addr.sun_path ,strerror(errno));

    //
    if (close(sockfd) != 0) {
        DebugPrint("socket: [%d] failed to close socket: [%s]", sockfd, strerror(errno));
    }
}


int clsPipe::CreateReceiver(const char *socketName) {
    int recSock = 0;
    struct sockaddr_un addr;

    //init
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    setSocketPath(&addr, socketName);

    //create socket
    recSock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (recSock == -1) {
        DebugPrint("can not create socket: [%s]", strerror(errno));
        return -1;
    }

    SetSocketResourceAddr(recSock, true);
    SetSocketShared(recSock, true);

    //unlink socket file
    if (unlink(addr.sun_path) != 0 && errno != ENOENT) {
        DebugPrint("can't unlink socket: %s", strerror(errno));
        close(recSock);
        return -1;
    }

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
    char buffer[PIPE_BUFFER_SIZE + 1] = {0};

    ssize_t bytes_read = recvfrom(sock, buffer, PIPE_BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

    if (bytes_read == -1) {
        DebugPrint("recvfrom error: sockd:[%d] socketName[%s%s] error: [%s]", sock, SOCKET_BASE_DIR, socketName,  strerror(errno));
    } else if (bytes_read == 0) {
        DebugPrint("sockd: [%d] recvfrom returned 0 (connection closed)", sock);
    } else {
        buffer[bytes_read] = 0;
        ret.append(buffer, bytes_read);
    }

    std::string sockpath = SOCKET_BASE_DIR;
    sockpath.append(socketName);
    //DebugPrint("recvfrom ddd: sockd:[%d] socketName[%s%s] error: [%s]", sock, SOCKET_BASE_DIR, socketName,  strerror(errno));


    if (unlink(sockpath.c_str()) != 0) {
        DebugPrint("sockd: [%d] failed to unlink socket file: [%s]", sock, strerror(errno));
    }


    shutdown(sock, SHUT_RDWR);
    if (close(sock) != 0) {
        DebugPrint("sockd: [%d] failed to close socket: [%s]", sock, strerror(errno));
    }

    return ret;
}

void clsPipe::OnPipeReceiveMessage(const char *msg, int msgLenth)
{
    //
    DebugPrint("OnReceiveMessage: [%s]\n", msg);
    //replay
    SendMessage(msg, "hi from server");
}


void clsPipe::killThread() {
    m_bRunning = false;
    if (m_Thread.joinable()) {
        m_Thread.join();
    }
    StopListen();
}
