#include "clsTCPSocket.h"
#include "clsTCPServer.h"
#include <math.h>       /* floor */
#include "log.h"

#ifdef USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

clsTCPSocket::clsTCPSocket(clsTCPServer *pServer)
{
    if(!pServer){
        DebugPrint("TCPServer not set");
        return;
    }

    m_pServer = pServer;
    Status = Closed;
    m_socket = 0;
    m_pClientSSlCtx = NULL;
    ClientIP.s_addr = 0;
    m_fdFile = 0;
    m_offest = 0;
    setIDType(IS_TCP_SOCKET);
    setPtrID(this);

}

clsTCPSocket::~clsTCPSocket()
{

}


void clsTCPSocket::OnConnectFailed(const char *msg, int errCode)
{
    //  LOG("OnConnectFailed(%d)", ERRNO);
}

void clsTCPSocket::OnConnecting()
{
    //   LOG("OnConnecting()");
}

void clsTCPSocket::OnConnected()
{
    //    LOG("OnConnected()");
}

void clsTCPSocket::OnAccepted()
{
}

void clsTCPSocket::OnReceiveData(const char *Buffer, int Length)
{
#ifdef libWrenchApp
    /*
    LOG("clsTCPSocket::OnReceiveData[%s][%d]", Buffer, Length);

    char szEchoMessage[] = "HTTP/1.1 200 OK\r\n"
                           "Content-Length: 9\r\n"
                           "\r\n"
                           "Chetori g";
    Send(szEchoMessage);

    LOG("clsTCPSocket::OnReceiveData[%s][%d]", Buffer, Length);

    int fdFile = open("/home/crow/Downloads/qt-everywhere-src-6.4.0.tar.xz", O_RDONLY);
    SendFile(fdFile);
 */
#endif
}

void clsTCPSocket::OnClosed()
{
    // LOG("clsTCPSocket::OnClosed()");
    //delete this;
}

void clsTCPSocket::OnSendFileStatus(uint64_t SizeSent)
{
    //
}

void clsTCPSocket::OnSendFileComplete()
{
    //
}


int clsTCPSocket::GetSocket() const
{
    return m_socket;
}

CString clsTCPSocket::GetClientIPString()
{
    CString ret;

    char buf[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &ClientIP, buf, sizeof(buf));
    ret.append(buf);
    //return inet_ntoa(ClientIP);   //localip
    return ret;
}

CString clsTCPSocket::GetSSLError() const
{
#ifdef USE_SSL
    return ERR_error_string(ERR_get_error(), NULL);
#endif

    return"";
}

uint32_t clsTCPSocket::GetClientIP()
{
    return ClientIP.s_addr;
}

clsTCPSocket *clsTCPSocket::GetTCPSocket() const
{
    return (clsTCPSocket *)this;
}

in_addr clsTCPSocket::GetIPFromHostAddress(const char *HostAddress)
{
    struct in_addr ResultAddr;
    memset(&ResultAddr, 0, sizeof(ResultAddr));

    //age HostAddress ipaddress bod resolve nemishe
    int IP = inet_addr(HostAddress);
    if(IP != ISINVALID)
    {
        ResultAddr.s_addr = IP;
        return ResultAddr;
    }

    //resolve
    struct addrinfo RequestAddress;
    struct addrinfo *ResponseAddress = NULL;


    memset(&RequestAddress, 0, sizeof(RequestAddress));
    RequestAddress.ai_family = AF_INET; // AF_INET or AF_INET6 to force version // AF_UNSPEC
    RequestAddress.ai_socktype = SOCK_STREAM;
    RequestAddress.ai_flags =  AI_NUMERICSERV;
    RequestAddress.ai_protocol = 0;

    //send request
    int isError = getaddrinfo(HostAddress, NULL, &RequestAddress, &ResponseAddress);
    if(isError != 0 || ResponseAddress == NULL)
    {

        //error //isError = EAI_NODATA(-5)
        return ResultAddr;
    }


    //parse
    //ResultAddress->ai_next;
    struct sockaddr_in *ipv4 = (sockaddr_in*)ResponseAddress->ai_addr;
    ResultAddr = ipv4->sin_addr;

    /* convert the IP to a string and print it: */
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(ResponseAddress->ai_family, &ResultAddr, ipstr, sizeof(ipstr));
    //LOG("GetIPFromHostAddress[%s]", ipstr);

    //free
    freeaddrinfo(ResponseAddress);

    return ResultAddr;
}

int clsTCPSocket::GetSocketReceiveBuffer(int fd)
{
    int bufferSize = 0;
    socklen_t len = sizeof(int);
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufferSize, &len);
    return bufferSize;
}

int clsTCPSocket::GetSocketSendBuffer(int fd)
{
    int bufferSize = 0;
    socklen_t len = sizeof(int);
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufferSize, &len);
    return bufferSize;
}

void clsTCPSocket::SetSocketReceiveBuffer(int fd, int bufferSize)
{
    /*
     * For a client, the SO_RCVBUF socket option must be set before calling connect.
     * For a server, the SO_RCVBUF socket option must be set for the listening socket before calling listen.
    */

    int isErr = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_RCVBUF, error[%d]", ERRNO);
    }

}

void clsTCPSocket::SetSocketSendBuffer(int fd, int bufferSize)
{
    int isErr = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_RCVBUF, error[%d]", ERRNO);
    }
}

void clsTCPSocket::SetSocketResourceAddr(int fd, bool isEnable)
{
    //Allow local address reuse
    int iOption = 0;
    if(isEnable)
        iOption = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&iOption, sizeof(iOption));
}

void clsTCPSocket::SetSocketShared(int fd, bool isEnable)
{
    //Allow local port reuse
    int iOption = 0;
    if(isEnable)
        iOption = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char*)&iOption, sizeof(iOption));

}

int clsTCPSocket::GetSocketConnectTimeout(int fd)
{
    int retRyCount = 0;
    socklen_t len = sizeof(int);
    getsockopt(fd, IPPROTO_TCP, TCP_SYNCNT, &retRyCount, &len);
    return retRyCount;
}
void clsTCPSocket::SetSocketConnectTimeout(int fd, TCPConnectTimeout Timeout)
{

    /*
    socklen_t optlen = sizeof(synRetries);
    getsockopt(fd, IPPROTO_TCP, TCP_SYNCNT, &synRetries, &optlen);
    DebugPrint("synRetries[%d]", synRetries);
    */
    //5 66sec , 4 32sec, 3 15sec, 2 7sec, 1 3sec
    int synRetries = Timeout; // Send a total of 3 SYN packets => Timeout ~7s
    int isErr = setsockopt(fd, IPPROTO_TCP, TCP_SYNCNT, &synRetries, sizeof(synRetries));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag TCP_SYNCNT, error[%d]", ERRNO);
    }

    /*
    struct timeval timeout;
    timeout.tv_sec  = 1;  // after 7 seconds connect() will timeout
    timeout.tv_usec = 0;
    int isErr = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_LINGER, error[%d]", ERRNO);
    }
*/
}

void clsTCPSocket::SetSocketNonBlocking(int fd)
{
    // set non-blocking
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

void clsTCPSocket::SetSocketNoDelay(int fd, bool isEnable)
{
    //set socket option -linux
    int iOption = 0;
    if(isEnable)
        iOption = 1;

    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&iOption, sizeof(iOption));


    //TCP_CORK
    setsockopt(fd, IPPROTO_TCP, TCP_CORK, (char*)&iOption, sizeof(iOption));
}

void clsTCPSocket::SetSocketBlockingMode(int fd)
{
    //get flags
    int flags = 0;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
    {
        // Handle error
        DebugPrint("error on get flag, error[%d]", ERRNO);
        return;
    }

    /* */
    /* baray fahmdiane inke socket non blockinge ya na
    if(fcntl(m_socket, F_GETFL) & O_NONBLOCK) {
        // socket is non-blocking
    }
    */

    //Set socket to blocking
    if (fcntl(fd, F_SETFL, flags & (~O_NONBLOCK)) < 0)
    {
        /* Handle error */
        DebugPrint("error on set flag, error[%d]", ERRNO);
    }
}

void clsTCPSocket::SetSocketKeepAlive(int fd, bool isEnable)
{


    int optval = 0;
    socklen_t optlen = sizeof(optval);

    if(isEnable)
        optval = 1;

    if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        DebugPrint("setsockopt() SO_KEEPALIVE");
        return;
    }

    if(isEnable == false){
        return;
    }


    //Set time
    int keepcnt = 4;        //tedade kavosh keepalive ghabl az az marg  //The maximum number of keepalive probes TCP should send before dropping the connection. This option should not be used in code intended to be portable.
    int keepidle = 2*60;      //shoroe keepalive bad az in dore           //The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes, if the socket option SO_KEEPALIVE has been set on this socket. This option should not be used in code intended to be portable.
    int keepintvl = 1*60;     //fasele zaamni beyne keepalive             //The time (in seconds) between individual keepalive probes. This option should not be used in code intended to be portable.
    int isErr;

    isErr = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(int));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag TCP_KEEPCNT, error[%d]", ERRNO);
    }

    isErr = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag TCP_KEEPIDLE, error[%d]", ERRNO);
    }

    isErr = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(int));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag TCP_KEEPINTVL, error[%d]", ERRNO);
    }


    /*
    getsockopt(m_socket, IPPROTO_TCP, TCP_KEEPCNT,  &optval, &optlen);
    LOG("Socket TCP_KEEPCNT = %d", optval);

    getsockopt(m_socket, IPPROTO_TCP, TCP_KEEPIDLE,  &optval, &optlen);
    LOG("Socket TCP_KEEPIDLE = %d", optval);

    getsockopt(m_socket, IPPROTO_TCP, TCP_KEEPINTVL,  &optval, &optlen);
    LOG("Socket TCP_KEEPINTVL = %d", optval);
*/

}

void clsTCPSocket::SetSocketLinger(int fd, int Timeout = 0)
{
    //age ba socket packet ersal konim va belafasele close konim momkene packet ersal nashe, ba Linger ye time moshakhas mikonim ke packet bad az ersal shodan close mishe
    int iOption = 0;
    if(Timeout >= 0)
        iOption = 1;

    struct linger lo =
        {
            iOption,
            Timeout
        };

    int isErr = setsockopt(fd, SOL_SOCKET, SO_LINGER, &lo, sizeof(lo));
    if(isErr != 0)
    {
        DebugPrint("error setsockopt set flag SO_LINGER, error[%d]", ERRNO);
    }
}

void getSocketLinget(int fd){
    struct linger lo ={0, 0};
    socklen_t len = sizeof(lo);
    getsockopt(fd, SOL_SOCKET, SO_LINGER,  &lo, &len);
    LOG("getSocketLinget enable[%d], time:[%d]", lo.l_onoff, lo.l_linger);
}

bool clsTCPSocket::SetSocketSendAndReceiveTimeOut(int fd, int secTime)
{
    int ret = true;
    struct timeval timeout;
    timeout.tv_sec = secTime;
    timeout.tv_usec = 0;

    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        ret = false;
    }

    if (setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        ret = false;
    }

    return ret;
}

bool clsTCPSocket::isSocketHaveError()
{
    int ret;
    int code;
    socklen_t len = sizeof(int);
    ret = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &code, &len);
    if ((ret || code)!= 0){
        return true;
    }
    return false;
}

epoll_event *clsTCPSocket::Event()
{
    return &m_Event;
}

void clsTCPSocket::_SetStatus(const TCPSocketStatus &value)
{
    Status = value;
}

/*
void clsTCPSocket::MutexLock()
{
    pthread_mutex_lock(&m_Mutex);
}

void clsTCPSocket::MutexUnlock()
{
    pthread_mutex_unlock(&m_Mutex);
}
*/

//public funtions
clsTCPSocket::TCPSocketStatus clsTCPSocket::GetStatus() const
{
    return Status;
}

in_addr clsTCPSocket::GetSockaddrFromSocket(int sock){
    /*
    //char buf[INET_ADDRSTRLEN] = "";
    struct sockaddr_in name;
    //socklen_t len = sizeof(name);

    if (getpeername(sock, (struct sockaddr *)&name, &len) != 0) {
        perror("getpeername");
    } else {
        inet_ntop(AF_INET, &name.sin_addr, buf, sizeof buf);
    }

    DebugPrint("iGetSockaddrFromSocket [%s]", buf);

*/
    // assume s is a connected socket

    in_addr ret = {0};
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int isErr = getpeername(sock, (struct sockaddr*)&addr, &len);
    if(isErr == 0){
        // IPv4:
        if (addr.sin_family == AF_INET) {
            //struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            ret = addr.sin_addr;
            //memcpy(&ret, &s->sin_addr, sizeof(ret));
        }
    }
    return ret;
}

bool clsTCPSocket::Accept(int new_socket, bool useSSL)
{
    if(Status != Closed || new_socket == 0 || new_socket == ISINVALID)
    {
        return false;
    }

    m_socket = new_socket;

    //change status...
    _SetStatus(Accepting);

    //Save ClientIP
    ClientIP = GetSockaddrFromSocket(m_socket);

    //if use ssl
    if(useSSL){
#ifdef USE_SSL
        clsSecureSocket *pSecureSocket = m_pServer->SSlSocket();
        if(pSecureSocket){
            m_pClientSSlCtx = pSecureSocket->newClientSSL(m_socket);
        }else{
            DebugPrint("certificate is not found");
            Close();
            return false;
        }

        //Do the SSL Handshake
        int ret = SSL_accept(m_pClientSSlCtx);

        //Check for error in handshake
        if (ret < 1) {
            //DebugPrint("SSL_accept failed: %d", SSL_get_error(m_pClientSSlCtx, ret));
            Close();
            return false;
        }

        /*
        X509 *cert = nullptr;
        if (cert == SSL_get_peer_certificate(m_pClientSSlCtx))
        {
            DebugPrint("SSL_get_peer_certificate error");
            Close();
            return false;
        }*/

        // Check for Client authentication error
        if (SSL_get_verify_result(m_pClientSSlCtx) != X509_V_OK) {
            DebugPrint("SSL Client Authentication error");
            SSL_free(m_pClientSSlCtx);
            Close();
            return false;
        }

        /*Print connection details
        LOG("SSL connection on socket [%x],Version: [%s], Cipher: [%s], Hostname [%s]\n",
               m_socket,
               SSL_get_version(m_pClientSSlCtx),
               SSL_get_cipher(m_pClientSSlCtx),
               SSL_get_servername(m_pClientSSlCtx, TLSEXT_NAMETYPE_host_name));
        */
#else
        DebugPrint("not suported ssl socket because ssl library is not linked");
        Close();
        return false;
#endif
    }

    /*
    struct sockaddr_storage *ClientAddrstorage;
    ClientAddrstorage = (struct sockaddr_storage *)&ClientAddr;
    if (ClientAddrstorage->ss_family != AF_INET) {
        DebugPrint("is not IPv4 Family address fd: %d, err(%d) [%s]", ERRNO, m_socket, strerror(ERRNO));
        Close();
        return false;
    }
    */

    //Set Blocking mode
    //SetBlockingSocket();
    //SetNonBlockingSocket();
    //SetNonBlockingSocket(m_socket);

    //
    //SetEnableNoDelay();

    //SetSocketLinger(m_socket, true, 1);

    //Alive Socket
    //SetKeepAlive(m_socket, true);
    //getSocketLinget(m_socket);

    //change status...
    _SetStatus(Connected);

    //int set = 1;
    //setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));

    //epoll Linux
    m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING;
    m_Event.data.u32 = 0;
    m_Event.data.u64 = 0;
    m_Event.data.ptr = getClassIDPtr();

    //
    OnAccepted();

    if(GetStatus() == Connected){
        //add too epoll manager
        int isAdd = m_pServer->AddSocketToEventsPoll(this);
        if(isAdd != 0)
        {
            Close(false);
            return false;
        }
    }


    //set coonect timeout bi estefadast baraye accept ama baraye inke befahmim socket accept shode mark mikonim socket ro
    SetSocketConnectTimeout(m_socket, TIMEOUT_7Sec);
    return true;
}

void clsTCPSocket::ConnectToHost(const char *HostAddress, uint16_t Port, TCPConnectTimeout Timeout)
{
    if(HostAddress == NULL || Port == 0 )
    {
        DebugPrint("HostAddress Or PortNumber is invalid");
        return;
    }

    if(Status != Closed)
    {
        DebugPrint("Status is not Closed");
        //Close();
        return;
    }

    //change status...
    _SetStatus(Connecting);
    OnConnecting();


    //Create TCP Socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == ISINVALID)
    {
        DebugPrint("cant create socket! errno[%d]", ERRNO);
        Close(false);
        return;
    }
    //***********************************Socket options*********************************
    //set enable resource addr
    //SetEnableResourceAddr();

    //Set non-blocking socket
    SetSocketNonBlocking(m_socket);

    //set coonect timeout
    SetSocketConnectTimeout(m_socket, Timeout);


    // set enable socket SO_REUSEADDR
    clsTCPSocket::SetSocketResourceAddr(m_socket, true);

    //set timeout
    if(m_pServer->getSocketOptSendRecTimeout() > 0)
        clsTCPSocket::SetSocketSendAndReceiveTimeOut(m_socket, m_pServer->getSocketOptSendRecTimeout());

    if(m_pServer->getSocketOptReceiveBufferSize() > 0)
        clsTCPSocket::SetSocketReceiveBuffer(m_socket, m_pServer->getSocketOptReceiveBufferSize());

    if(m_pServer->getSocketOptSendBufferSize() > 0)
        clsTCPSocket::SetSocketSendBuffer(m_socket, m_pServer->getSocketOptSendBufferSize());

    if(m_pServer->getSocketOptLingerTime() >= 0)
        clsTCPSocket::SetSocketLinger(m_socket, m_pServer->getSocketOptLingerTime());

    if(m_pServer->getSocketOptShare())
        clsTCPSocket::SetSocketShared(m_socket, m_pServer->getSocketOptShare());

    if(m_pServer->getSocketOptNoDelay())
        clsTCPSocket::SetSocketNoDelay(m_socket, m_pServer->getSocketOptNoDelay());

    if(m_pServer->getSocketKeepAlive())
        clsTCPSocket::SetSocketKeepAlive(m_socket, m_pServer->getSocketKeepAlive());


    //***********************************************************************************


    //get ip address -- Blocking method
    in_addr IPAddress = GetIPFromHostAddress(HostAddress);
    if (IPAddress.s_addr == 0)
    {
        //DebugPrint("cant resolve host! errno[%d]", ERRNO);
        OnConnectFailed("can't resolve host!", ERRNO);
        //Close(false);
        CloseFinal();
        return;
    }


    //addr
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    addr.sin_addr = IPAddress;

    int isConnect = connect(m_socket, (sockaddr*)&addr, sizeof(addr));

    //connecting...
    if(isConnect == 0 || errno == EINPROGRESS)
    {
        //Socket Event Linux
        m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING | EPOLLOUT;
        m_Event.data.u32 = 0;
        m_Event.data.u64 = 0;
        m_Event.data.ptr = getClassIDPtr();


        //add too epoll manager
        int isAdd = m_pServer->AddSocketToEventsPoll(this);
        if(isAdd != 0)
        {
            OnConnectFailed("can't AddSocketToEventsPoll!", ERRNO);
            Close(false);
            return;
        }

        //wait to get response in epoll....
        return;
    }

    //connect failed
    if(isConnect == ISINVALID)
    {
        //DebugPrint("cant connect to ip! errno[%d]", ERRNO);
        if (errno == EAGAIN)
        {
            //perror("connect is EAGAIN\n");
        }

        OnConnectFailed("can't connect to IP!", ERRNO);
        CloseFinal();
        //Close(false);
        return;
    }
}

void clsTCPSocket::onConnected()
{

    //Alive Socket
    //SetKeepAlive();

    //Set Blocking mode
    //SetSocketBlockingMode(m_socket);

    //change modify for remove EPOLLOUT
    m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING;


    //for ssl
    //SSL_set_connect_state

    //change status...
    _SetStatus(Connected);

    //
    OnConnected();
}



///* orginal
void clsTCPSocket::onReceiving()
{
    if(Status == Closing){
        //LOG("Close Soceket, err:(%d), m_socket:(%d)", ERRNO, m_socket);
        m_pServer->RemoveSocketFromEventsPoll(this);
        return;
    }

    if(Status != Connected)
    {
        DebugPrint("Status != Connected, m_socket:[%d], status:[%d], errno[%1d]", m_socket, Status, ERRNO);
        return;
    }

    if(isSocketHaveError() == true)
    {
        //DebugPrint("EPOLL IN"); //closed
        DebugPrint("EPOLLIN! isSocketHaveError Status[%d], errno[%1d]", Status, ERRNO);
        Close(false);
        return;
    }

    char recBuffer[BUFFER_SIZE+1];
    ssize_t bytesRec;
    if(m_pClientSSlCtx){
#ifdef USE_SSL
        bytesRec = SSL_read(m_pClientSSlCtx, recBuffer, BUFFER_SIZE);

        if(bytesRec < 1) {
            int err = SSL_get_error(m_pClientSSlCtx, bytesRec);
            if(err == SSL_ERROR_ZERO_RETURN)
                SSL_shutdown(m_pClientSSlCtx);
        }
#endif
    }else{
        bytesRec = recv(m_socket, recBuffer, BUFFER_SIZE, 0); //read(m_socket, recBuffer, BUFFER_SIZE);
    }

    if(Status != Connected){
        return;
    }

    //Socket is closed
    if(bytesRec == 0)
    {
        Close(false);
        return;
    }


    //receive data
    if(bytesRec > 0)
    {
        //
        recBuffer[bytesRec] = '\0';
        OnReceiveData(recBuffer, bytesRec);
        return;
    }


    //socket error
    if(bytesRec == ISINVALID)
    {
        if(errno == EAGAIN || errno == EINTR){
            //LOG("OnWaitForNewEvenet EAGAIN || EINTR");
            // goto A;
        }
        //DebugPrint("epoll_wait, err:(%d)", ERRNO);
        //continue;

        DebugPrint("recv failed socket: %d, err(%d)", m_socket, ERRNO);
        Close(false);
    }

}

ssize_t clsTCPSocket::Send(const char *Buffer)
{
    return Send(Buffer, strlen(Buffer));
}

ssize_t clsTCPSocket::Send(const char *Buffer, size_t Length)
{
    if (Status != Connected || Buffer == nullptr){
        return ISINVALID;
    }


    ssize_t bytesSent = 0;
    char *pBuffer = (char*)Buffer;
    int pBufferLength = Length;
    while(pBufferLength > 0){

        if(m_pClientSSlCtx){
#ifdef USE_SSL
            bytesSent = SSL_write(m_pClientSSlCtx, pBuffer, pBufferLength);
            if(bytesSent < 1) {
                int err = SSL_get_error(m_pClientSSlCtx, bytesSent);
                DebugPrint("SSL_write error [%d]", err);
                if(err == SSL_ERROR_ZERO_RETURN){
                    SSL_shutdown(m_pClientSSlCtx);
                }else{
                }

            }
#endif
        }else{
            bytesSent = send(m_socket, pBuffer , pBufferLength, MSG_NOSIGNAL);
        }


        if(bytesSent == ISINVALID)
        {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000);
                continue;
            }

            if(pBufferLength != Length){
                DebugPrint("Send not complite send [%d] of [%d], socket[%d], err:(%d) \n", pBufferLength, Length, m_socket, ERRNO);
            }
            break;
        }

        pBuffer += bytesSent;
        pBufferLength -= bytesSent;
    }


    return bytesSent;
}

ssize_t clsTCPSocket::SendFile(int fileFD )
{
    if (Status != Connected)
        return ISINVALID;


    //read file
    if(m_fdFile != 0){
        DebugPrint("sendfile is busy");
        return ISINVALID;
    }

    //
    m_fdFile = fileFD;

    //
    m_offest = 0;
    ResumeSendFile();

    return 0;
}

void clsTCPSocket::CloseFile(){
    if(m_fdFile > 0){
        close(m_fdFile);
        m_fdFile = 0;
    }
}


/*
void clsTCPSocket::ResumeSendFile2()
{
    if (Status != Connected)
        return;

    LOG("call clsTCPSocket::ResumeSendFile(%lu)", pthread_self());


    //read file
    if(m_fdFile <= 0){
        DebugPrint("invalid fd file!");
        Close(false);
        return;
    }

    //back to normal event
    m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING;

    ssize_t sent_bytes = 0;
    int sslErr = 0;
    //long remain_data = FileSize;

    // Sending file data

    while (1)
    {
        if (Status != Connected){
            break;
        }

        if(m_pClientSSlCtx){


            // sent_bytes = SSL_sendfile(m_pClientSSlCtx, m_fdFile, m_offest, BUFSIZ, 0);

            char bufferFile[BUFSIZ+1];
            memset(bufferFile, 0, BUFSIZ);
            ssize_t readFileSize = 0;


            readFileSize = pread(m_fdFile, bufferFile, BUFSIZ, m_offest);
            if(readFileSize == ISINVALID){
                DebugPrint("can not read the file");
            }

            if(readFileSize == 0){
                //end read
                DebugPrint(" --- readFileSize = 0");
                Close(false);
                break;
            }

            if(readFileSize < 0){
                DebugPrint(" --- readFileSize = -1");
                Close(false);
                break;
            }



            if(readFileSize > 0){
                sent_bytes = SSL_write(m_pClientSSlCtx, bufferFile, readFileSize);

                LOG("SSL_write sent_bytes: [%zd] m_offest [%zd] readFileSize[%zd] SSL_pending [%d]", sent_bytes, m_offest, readFileSize, SSL_has_pending(m_pClientSSlCtx));
                if(sent_bytes < 0) {
                    //usleep(1000000);
                    int err = SSL_get_error(m_pClientSSlCtx, sent_bytes);
                    DebugPrint("SSL_write error [%d]", err);

                    if(err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SSL){
                        SSL_shutdown(m_pClientSSlCtx);
                        DebugPrint("SSL_shutdown");
                        break;
                    }

                    if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_X509_LOOKUP) {
                        LOG("Write could not complete. Will be invoked later.");
                        LOG("sendfile errno[%d]", errno);
                        if(errno == EAGAIN || errno == EWOULDBLOCK){

                        }

                        //SSL_has_pending(m_pClientSSlCtx);

                        //SSL_set_wfd(m_fdFile);
                        m_Event.events = EPOLLOUT;
                        return;
                    }

                    //if(err == SSL_ERROR_WANT_WRITE) continue;
                    //Close(false);
                    // break;
                }

                if(sent_bytes > 0){
                    m_offest += sent_bytes;
                    continue;
                }
            }


        }
    }
}
*/

void clsTCPSocket::ResumeSendFile()
{
    if (Status != Connected)
        return;

    //read file
    if(m_fdFile <= 0){
        DebugPrint("invalid fd file!");
        Close(false);
        return;
    }

    //back to normal event
    m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING;

    ssize_t sent_bytes = 0;
    int sslErr = 0;

    // Sending file data
    while (1)
    {
        if (Status != Connected){
            break;
        }

        if(m_pClientSSlCtx)
        {
#ifdef USE_SSL

            char bufferFile[BUFSIZ+1];
            memset(bufferFile, 0, BUFSIZ);
            ssize_t readFileSize = 0;

            readFileSize = pread(m_fdFile, bufferFile, BUFSIZ, m_offest);
            if(readFileSize == ISINVALID){
                DebugPrint("can not read the file");
            }

            //end of reading
            if(readFileSize == 0){
                //LOG("ssl_sendfile don");
                //Close(true);
                CloseFile();
                OnSendFileComplete();
                break;
            }

            //error on reading
            if(readFileSize < 0){
                DebugPrint(" --- readFileSize = -1");
                Close(false);
                break;
            }

            if(readFileSize > 0){
                sent_bytes = SSL_write(m_pClientSSlCtx, bufferFile, readFileSize);
                if(sent_bytes < 0) {
                    //usleep(100000);
                    int err = SSL_get_error(m_pClientSSlCtx, sent_bytes);

                    if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_X509_LOOKUP) {
                        //LOG("ssl_Write err: [%d]", err);
                        m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING | EPOLLOUT;
                        return;
                    }

                    DebugPrint("SSL_write error [%d]", err);
                    Close(false);
                    return;
                }

                if(sent_bytes > 0){
                    m_offest += sent_bytes;
                    continue;
                }
            }


#endif
        }else{
            sent_bytes = sendfile64(m_socket, m_fdFile, &m_offest, BUFSIZ);
        }

        if(sent_bytes > 0){
            //LOG("sent_bytes [%ld] m_offest[%ld]", sent_bytes, m_offest);
            OnSendFileStatus(m_offest);
            continue;
        }

        //send complate
        if(sent_bytes == 0){
            //LOG("sendfile don");
            CloseFile();
            OnSendFileComplete();
            break;
        }

        if(sent_bytes < 0){

            if (Status != Connected)
                break;

            // change event
            m_Event.events = EPOLL_EVENTS_TCP_MULTITHREAD_NONBLOCKING | EPOLLOUT;

            //LOG("sendfile errno[%d]", errno); //EAGAIN
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                //set time for last send
                //usleep(100000);
                break;
            }else if (errno == EBADF || errno == EPIPE || errno == ECONNRESET){
                LOG("sendfile2 errno[%d]", errno);
                break;
            }else{
                //other errors
                if(errno != EPIPE)
                    LOG("sendfile errno[%d]", errno); //EAGAIN
                break;
            }

        }

    }
}


void clsTCPSocket::Close(bool isShutdown)
{
    if (Status == Closed || Status == Closing){
        return;
    }

    if (Status == Connected)
        isShutdown = true;

//isShutdown = false;
//clean ssl socket
#ifdef USE_SSL
    if(m_pClientSSlCtx){
        if(isShutdown == true){
            SSL_set_shutdown(m_pClientSSlCtx, SSL_RECEIVED_SHUTDOWN | SSL_SENT_SHUTDOWN);
            SSL_shutdown(m_pClientSSlCtx);
        }
        SSL_free(m_pClientSSlCtx);
        m_pClientSSlCtx = NULL;
    }
#endif

    if (Status == Accepting){
        CloseFinal();
    }

    //LOG("Close() errno[%d]\n", errno);
    if(m_socket == ISINVALID || m_socket == 0)
    {
        //change status...
        _SetStatus(Closed);
        return;
    }

    //change status...
    _SetStatus(Closing);

    //
    CloseFile();

    // change event
    //m_Event.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
    //ModifyEvent();

    //
    //m_Event.data.ptr = nullptr;
    //ModifyEvent();

    if(isShutdown == true)
    {
        //shutdown(m_socket, SHUT_RDWR);  //SHUT_RDWR
        //LOG("shutdown, sock:[%d]", m_socket);
        CloseFinal();
    }else{
        //remove of epoll manager
        if(m_socket != ISINVALID || m_socket != 0){
            m_pServer->RemoveSocketFromEventsPoll(this);
        }
    }

}

void clsTCPSocket::CloseFinal(){

    close(m_socket);

    //change status...
    _SetStatus(Closed);

    //LOG("close, sock:[%d]", m_socket);

    m_socket = ISINVALID;

    //Close Event
    //OnClosed();
}



bool clsTCPSocket::isReadyForDelete() const
{
    if(Status == Closed && m_socket == ISINVALID)
    {
        return true;
    }

    return false;
}

bool clsTCPSocket::isReadyForClose() const
{
    if(Status == Connected)
    {
        if(m_fdFile == 0)
        {
            return true;
        }
    }

    return false;
}
