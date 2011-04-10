
#include "ecore_config.h"
#include <Ws2tcpip.h>
#include "networking.h"
#include "ports.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef  __MINGW32__

/*
 * "QueryInterface" versions of the above APIs.
 */

typedef
BOOL
(PASCAL FAR * LPFN_TRANSMITFILE)(
     SOCKET hSocket,
     HANDLE hFile,
     DWORD nNumberOfBytesToWrite,
     DWORD nNumberOfBytesPerSend,
     LPOVERLAPPED lpOverlapped,
     LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
     DWORD dwReserved
    );

#define WSAID_TRANSMITFILE \
        {0xb5367df0,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

typedef
BOOL
(PASCAL FAR * LPFN_ACCEPTEX)(
     SOCKET sListenSocket,
     SOCKET sAcceptSocket,
     PVOID lpOutputBuffer,
     DWORD dwReceiveDataLength,
     DWORD dwLocalAddressLength,
     DWORD dwRemoteAddressLength,
     LPDWORD lpdwBytesReceived,
     LPOVERLAPPED lpOverlapped
    );

#define WSAID_ACCEPTEX \
        {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

typedef
VOID
(PASCAL FAR * LPFN_GETACCEPTEXSOCKADDRS)(
     PVOID lpOutputBuffer,
     DWORD dwReceiveDataLength,
     DWORD dwLocalAddressLength,
     DWORD dwRemoteAddressLength,
     struct sockaddr **LocalSockaddr,
     LPINT LocalSockaddrLength,
     struct sockaddr **RemoteSockaddr,
     LPINT RemoteSockaddrLength
    );

#define WSAID_GETACCEPTEXSOCKADDRS \
        {0xb5367df2,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}


typedef
BOOL
(PASCAL FAR * LPFN_TRANSMITPACKETS) (
     SOCKET hSocket,
     TRANSMIT_PACKETS_ELEMENT* lpPacketArray,
     DWORD nElementCount,
     DWORD nSendSize,
     LPOVERLAPPED lpOverlapped,
     DWORD dwFlags
    );

#define WSAID_TRANSMITPACKETS \
    {0xd9689da0,0x1f90,0x11d3,{0x99,0x71,0x00,0xc0,0x4f,0x68,0xc8,0x76}}

typedef
BOOL
(PASCAL FAR * LPFN_CONNECTEX) (
     SOCKET s,
     const struct sockaddr FAR *name,
     int namelen,
     PVOID lpSendBuffer,
     DWORD dwSendDataLength,
     LPDWORD lpdwBytesSent,
     LPOVERLAPPED lpOverlapped
    );

#define WSAID_CONNECTEX \
    {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}

typedef
BOOL
(PASCAL FAR * LPFN_DISCONNECTEX) (
     SOCKET s,
     LPOVERLAPPED lpOverlapped,
     DWORD  dwFlags,
     DWORD  dwReserved
    );

#define WSAID_DISCONNECTEX \
    {0x7fda2e11,0x8630,0x436f,{0xa0, 0x31, 0xf5, 0x36, 0xa6, 0xee, 0xc1, 0x57}}

#endif

static LPFN_TRANSMITFILE __transmitfile = NULL;
static LPFN_ACCEPTEX __acceptex = NULL;
static LPFN_TRANSMITPACKETS __transmitpackets = NULL;
static LPFN_CONNECTEX __connectex = NULL;
static LPFN_DISCONNECTEX __disconnectex = NULL;
static LPFN_GETACCEPTEXSOCKADDRS __getacceptexsockaddrs = NULL;


#define sys_call(func)   (SOCKET_ERROR != (func))?ECORE_RC_OK:ECORE_RC_ERROR

#define sys_call_with_ecore_rcean(func)   (TRUE == (func))?ECORE_RC_OK:ECORE_RC_ERROR


int poll(SOCKET sock, const struct timeval* time_val, int mode)
{
    fd_set socket_set;
    FD_ZERO(&socket_set);
    FD_SET(sock, &socket_set);

    return (1 == select(0, (mode & select_read) ? &socket_set : NULL
                          , (mode & select_write) ? &socket_set : NULL
                          , (mode & select_error) ? &socket_set : NULL
                          , time_val))?1:0;
}

int isReadable(SOCKET sock)
{
    struct timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 0;
    return poll(sock, &time_val, select_read);
}

int isWritable(SOCKET sock)
{
    struct timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 0;
    return poll(sock, &time_val, select_write);
}

ecore_rc setNonblocking(SOCKET sock)
{
    u_long nonblock = 1;
    return sys_call(ioctlsocket(sock,
                              FIONBIO,
                              &nonblock));
}
//
//ecore_rc send_n(SOCKET sock, const char* buf, size_t length)
//{
//    do
//    {
//#pragma warning(disable: 4267)
//        int n = send(sock, buf, length, 0);
//#pragma warning(default: 4267)
//        if (0 >= n)
//            return ECORE_RC_ERROR;
//
//        length -= n;
//        buf += n;
//    }
//    while (0 < length);
//
//    return ECORE_RC_OK;
//}
//
//ecore_rc recv_n(SOCKET sock, char* buf, size_t length)
//{
//    do
//    {
//#pragma warning(disable: 4267)
//        int n = ::recv(sock, buf, length, 0);
//#pragma warning(default: 4267)
//
//        if (0 >= n)
//            return ECORE_RC_ERROR;
//
//        length -= n;
//        buf += n;
//    }
//    while (0 < length);
//
//    return ECORE_RC_OK;
//}
//
//ecore_rc sendv_n(SOCKET sock, const io_mem_buf* wsaBuf, size_t size)
//{
//    std::vector<io_mem_buf> buf(wsaBuf, wsaBuf + size);
//    io_mem_buf* p = &buf[0];
//
//    do
//    {
//        DWORD numberOfBytesSent = 0;
//#pragma warning(disable: 4267)
//        if (SOCKET_ERROR == ::WSASend(sock, p, size, &numberOfBytesSent, 0, 0 , 0))
//#pragma warning(default: 4267)
//            return ECORE_RC_ERROR;
//
//        do
//        {
//            if (numberOfBytesSent < p->len)
//            {
//                p->len -= numberOfBytesSent;
//                p->buf = p->buf + numberOfBytesSent;
//                break;
//            }
//            numberOfBytesSent -= p->len;
//            ++ p;
//            -- size;
//        }
//        while (0 < numberOfBytesSent);
//    }
//    while (0 < size);
//
//    return ECORE_RC_OK;
//}
//
//ecore_rc recvv_n(SOCKET sock, io_mem_buf* wsaBuf, size_t size)
//{
//    io_mem_buf* p = wsaBuf;
//
//    do
//    {
//        DWORD numberOfBytesRecvd = 0;
//#pragma warning(disable: 4267)
//        if (SOCKET_ERROR == ::WSARecv(sock, p, size, &numberOfBytesRecvd, 0, 0 , 0))
//#pragma warning(default: 4267)
//            return ECORE_RC_ERROR;
//
//        do
//        {
//            if (numberOfBytesRecvd < p->len)
//            {
//                p->len -= numberOfBytesRecvd;
//                p->buf = p->buf + numberOfBytesRecvd;
//                break;
//            }
//            numberOfBytesRecvd -= p->len;
//            ++ p;
//            -- size;
//        }
//        while (0 < numberOfBytesRecvd);
//    }
//    while (0 < size);
//
//    return ECORE_RC_OK;
//}
ecore_rc  initializeScket()
{
    GUID GuidConnectEx = WSAID_CONNECTEX;
    GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    GUID GuidTransmitFile = WSAID_TRANSMITFILE;
    GUID GuidTransmitPackets = WSAID_TRANSMITPACKETS;

	SOCKET cliSock;
    DWORD dwBytes = 0;

    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
        return ECORE_RC_ERROR;

    if (LOBYTE(wsaData.wVersion) != 2 ||
            HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return ECORE_RC_ERROR;
    }

    cliSock = socket(AF_INET , SOCK_STREAM, IPPROTO_TCP);

    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidConnectEx,
                                 sizeof(GuidConnectEx),
                                 &__connectex,
                                 sizeof(__connectex),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __connectex = NULL;
    }


    dwBytes = 0;
    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidDisconnectEx,
                                 sizeof(GuidDisconnectEx),
                                 &__disconnectex,
                                 sizeof(__disconnectex),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __disconnectex = NULL;
    }

    dwBytes = 0;
    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidTransmitFile,
                                 sizeof(GuidTransmitFile),
                                 &__transmitfile,
                                 sizeof(__transmitfile),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __transmitfile = NULL;
    }

    dwBytes = 0;
    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidAcceptEx,
                                 sizeof(GuidAcceptEx),
                                 &__acceptex,
                                 sizeof(__acceptex),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __acceptex = NULL;
    }

    dwBytes = 0;
    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidTransmitPackets,
                                 sizeof(GuidTransmitPackets),
                                 &__transmitpackets,
                                 sizeof(__transmitpackets),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __transmitpackets = NULL;
    }

    dwBytes = 0;
    if (SOCKET_ERROR == WSAIoctl(cliSock,
                                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                                 &GuidGetAcceptExSockAddrs,
                                 sizeof(GuidGetAcceptExSockAddrs),
                                 &__getacceptexsockaddrs,
                                 sizeof(__getacceptexsockaddrs),
                                 &dwBytes,
                                 NULL,
                                 NULL))
    {
        __getacceptexsockaddrs = NULL;
    }

    closesocket(cliSock);

    return ECORE_RC_OK;
}

void shutdownSocket()
{
    WSACleanup();
}

ecore_rc transmitFile(SOCKET hSocket,
                  HANDLE hFile,
                  DWORD nNumberOfBytesToWrite,
                  DWORD nNumberOfBytesPerSend,
                  LPOVERLAPPED lpOverlapped,
                  LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
                  DWORD dwFlags)
{

    return sys_call_with_ecore_rcean(__transmitfile(hSocket
                                  , hFile
                                  , nNumberOfBytesToWrite
                                  , nNumberOfBytesPerSend
                                  , lpOverlapped
                                  , lpTransmitBuffers
                                  , dwFlags));
}

ecore_rc acceptEx(SOCKET sListenSocket,
              SOCKET sAcceptSocket,
              PVOID lpOutputBuffer,
              DWORD dwReceiveDataLength,
              DWORD dwLocalAddressLength,
              DWORD dwRemoteAddressLength,
              LPDWORD lpdwBytesReceived,
              LPOVERLAPPED lpOverlapped)
{
    return sys_call_with_ecore_rcean(__acceptex(sListenSocket,
                              sAcceptSocket,
                              lpOutputBuffer,
                              dwReceiveDataLength,
                              dwLocalAddressLength,
                              dwRemoteAddressLength,
                              lpdwBytesReceived,
                              lpOverlapped));
}

ecore_rc transmitPackets(SOCKET hSocket,
		             TRANSMIT_PACKETS_ELEMENT* lpPacketArray,
                     DWORD nElementCount,
                     DWORD nSendSize,
                     LPOVERLAPPED lpOverlapped,
                     DWORD dwFlags)
{
    return sys_call_with_ecore_rcean(__transmitpackets(hSocket,
                                     lpPacketArray,
                                     nElementCount,
                                     nSendSize,
                                     lpOverlapped,
                                     dwFlags));
}

ecore_rc connectEx(SOCKET s,
               const struct sockaddr* name,
               int namelen,
               PVOID lpSendBuffer,
               DWORD dwSendDataLength,
               LPDWORD lpdwBytesSent,
               LPOVERLAPPED lpOverlapped)
{
    return sys_call_with_ecore_rcean(__connectex(s,
                               name,
                               namelen,
                               lpSendBuffer,
                               dwSendDataLength,
                               lpdwBytesSent,
                               lpOverlapped));
}

ecore_rc disconnectEx(SOCKET hSocket,
                  LPOVERLAPPED lpOverlapped,
                  DWORD dwFlags,
                  DWORD reserved)
{
    return sys_call_with_ecore_rcean(__disconnectex(hSocket,
                                  lpOverlapped,
                                  dwFlags,
                                  reserved));
}

void getAcceptExSockaddrs(PVOID lpOutputBuffer,
                          DWORD dwReceiveDataLength,
                          DWORD dwLocalAddressLength,
                          DWORD dwRemoteAddressLength,
                          LPSOCKADDR* LocalSockaddr,
                          LPINT LocalSockaddrLength,
                          LPSOCKADDR* RemoteSockaddr,
                          LPINT RemoteSockaddrLength)
{
    __getacceptexsockaddrs(lpOutputBuffer,
                           dwReceiveDataLength,
                           dwLocalAddressLength,
                           dwRemoteAddressLength,
                           LocalSockaddr,
                           LocalSockaddrLength,
                           RemoteSockaddr,
                           RemoteSockaddrLength);
}

ecore_rc stringToAddress(const char* url
                     , struct sockaddr* addr)
{
	char host[50];
	const char* ptr = url;
	const char* end = NULL;

	if('[' == *ptr)
	{
		// 处理 ipv6的地址，格式如 [xxxx:xxxx::xxx]:port
		addr->sa_family = AF_INET6;

		if(NULL == (end = strchr(++ptr, ']')))
			return ECORE_RC_ERROR;

		if( ':' != *(end + 1))
			return ECORE_RC_ERROR;


		strncpy(host, ptr, end-ptr);
		host[end-ptr] =0;
		if(1 != inet_pton(AF_INET6, host, &(((struct sockaddr_in6*)addr)->sin6_addr)))
			return ECORE_RC_ERROR;

		++ end;

		((struct sockaddr_in6*)addr)->sin6_port = htons(atoi(end));
		return ECORE_RC_OK;
	}
	else
	{
		// 处理 ipv4的地址，格式如  xxx.xxx.xxx.xxx:port
		addr->sa_family = AF_INET;

		if(NULL == (end = strchr(ptr, ':')))
			return ECORE_RC_ERROR;

		strncpy(host, ptr, end-ptr);
		host[end-ptr] =0;
		if(1 != inet_pton(AF_INET, host, &(((struct sockaddr_in*)addr)->sin_addr)))
			return ECORE_RC_ERROR;

		++ end;

		((struct sockaddr_in*)addr)->sin_port = htons(atoi(end));
		return ECORE_RC_OK;
	}
}

ecore_rc addressToString(struct sockaddr* addr
                     , const char* schema
                     , size_t schema_len
                     , string_t* url)
{

	size_t len = 0;
	const char* ptr = 0;

    if(NULL == schema)
        string_assignLen(url, "tcp", 3);
    else if(-1 == schema_len)
        string_assignLen(url, schema, strlen(schema));
    else
        string_assignLen(url, schema, schema_len);


	if(addr->sa_family == AF_INET6)
		string_appendLen(url, "6", 1);

	string_appendLen(url, "://", 3);
	len = string_length(url);

	string_appendN(url, 0, IP_ADDRESS_LEN);

	if(AF_INET6 == addr->sa_family)
		ptr = inet_ntop(addr->sa_family,  &(((struct sockaddr_in6*)addr)->sin6_addr), string_data(url) + len, IP_ADDRESS_LEN);
	else
		ptr = inet_ntop(addr->sa_family,  &(((struct sockaddr_in*)addr)->sin_addr), string_data(url) + len, IP_ADDRESS_LEN);
	if(0 == ptr)
	{
		string_truncate(url, 0);
		return ECORE_RC_ERROR;
	}
	string_truncate(url, len + strlen(ptr));

	string_append_sprintf(url, ":%d", 
			ntohs((AF_INET6 == addr->sa_family)?((struct sockaddr_in6*)addr)->sin6_port
			:((struct sockaddr_in*)addr)->sin_port));
	
	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
