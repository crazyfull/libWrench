#ifndef SOCKHEADER_H
#define SOCKHEADER_H

#ifdef _WIN32
    //Socket in windows
    #include <WinSock2.h>
    #include <ws2tcpip.h>
    #include <Mstcpip.h>

    #pragma comment(lib, "ws2_32.lib")

#else //#ifdef __unix__ //__linux__
    //Socket in unix
    #include <sys/socket.h>
    #include <netinet/tcp.h> //keefpalivetimer
    #include <netdb.h>	//gethostbyname
    #include <unistd.h> // for close()
    #include <errno.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <pthread.h> //thread
    #include <fcntl.h>

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <sys/epoll.h>
    #include <sys/sendfile.h>
#endif

#ifdef _WIN32
    #define SOCKET_ERRNO    WSAGetLastError()
    #define ERRNO       GetLastError()
    #define MSG_NOSIGNAL (0)
#else
    #define SOCKET_ERRNO    errno
    #define ERRNO       errno
    #define closesocket close
    #define WSACleanup() (void)0
#endif

#ifdef ISINVALID
#else
    #define ISINVALID (-1)
#endif


/*Variable public*/
#define MAX_THREAD (65536)
#define BUFFER_SIZE (8 * 1024)	//8 KB
#define MAX_EPOLL_EVENT_COUNT (MAX_EPOLL_COUNT / THREAD_WORKERS_COUNT)  //max 5000

//#define EPOLL_EVENTS_MULTITHREAD (EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLONESHOT) //| EPOLLHUP
//#define EPOLL_EVENTS_MULTITHREAD (EPOLLIN | EPOLLONESHOT)
//#define EPOLL_EVENTS_MULTITHREAD_NONBLOCKING (EPOLLIN | EPOLLHUP | EPOLLPRI | EPOLLERR | EPOLLONESHOT | EPOLLET ) //test EPOLLHUP EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLWAKEUP
//#define EPOLL_EVENTS_MULTITHREAD_NONBLOCKING (EPOLLIN | EPOLLONESHOT | EPOLLRDHUP |EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP  )

#define EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING (EPOLLIN | EPOLLET | EPOLLONESHOT) //     //(EPOLLIN | EPOLLONESHOT | EPOLLET) //Asli

#define EPOLL_EVENTS_LISTENER_NONBLOCKING (EPOLLIN | EPOLLET)
#define EPOLL_EVENTS_TCP_NONBLOCKING (EPOLL_EVENTS_LISTENER_NONBLOCKING | EPOLLOUT)



//

#endif
