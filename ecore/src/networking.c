
#include "ecore_config.h"
#include "networking.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  __GNUC__

static LPFN_TRANSMITFILE __transmitfile = NULL;
static LPFN_ACCEPTEX __acceptex = NULL;
static LPFN_TRANSMITPACKETS __transmitpackets = NULL;
static LPFN_CONNECTEX __connectex = NULL;
static LPFN_DISCONNECTEX __disconnectex = NULL;
static LPFN_GETACCEPTEXSOCKADDRS __getacceptexsockaddrs = NULL;

#endif

#define sys_call(func)   (SOCKET_ERROR != (func))?true:false

#define sys_call_with_boolean(func)   (TRUE == (func))?true:false


bool poll(SOCKET sock, const struct timeval* time_val, int mode)
{
    fd_set socket_set;
    FD_ZERO(&socket_set);
    FD_SET(sock, &socket_set);

    return (1 == select(0, (mode & select_read) ? &socket_set : NULL
                          , (mode & select_write) ? &socket_set : NULL
                          , (mode & select_error) ? &socket_set : NULL
                          , time_val));
}

bool isReadable(SOCKET sock)
{
    struct timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 0;
    return poll(sock, &time_val, select_read);
}

bool isWritable(SOCKET sock)
{
    struct timeval time_val;
    time_val.tv_sec = 0;
    time_val.tv_usec = 0;
    return poll(sock, &time_val, select_write);
}

bool setBlocking(SOCKET sock, bool val)
{
    u_long nonblock = (val)? 1 : 0;
    return sys_call(ioctlsocket(sock,
                              FIONBIO,
                              &nonblock));
}
//
//bool send_n(SOCKET sock, const char* buf, size_t length)
//{
//    do
//    {
//#pragma warning(disable: 4267)
//        int n = send(sock, buf, length, 0);
//#pragma warning(default: 4267)
//        if (0 >= n)
//            return false;
//
//        length -= n;
//        buf += n;
//    }
//    while (0 < length);
//
//    return true;
//}
//
//bool recv_n(SOCKET sock, char* buf, size_t length)
//{
//    do
//    {
//#pragma warning(disable: 4267)
//        int n = ::recv(sock, buf, length, 0);
//#pragma warning(default: 4267)
//
//        if (0 >= n)
//            return false;
//
//        length -= n;
//        buf += n;
//    }
//    while (0 < length);
//
//    return true;
//}
//
//bool sendv_n(SOCKET sock, const io_mem_buf* wsaBuf, size_t size)
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
//            return false;
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
//    return true;
//}
//
//bool recvv_n(SOCKET sock, io_mem_buf* wsaBuf, size_t size)
//{
//    io_mem_buf* p = wsaBuf;
//
//    do
//    {
//        DWORD numberOfBytesRecvd = 0;
//#pragma warning(disable: 4267)
//        if (SOCKET_ERROR == ::WSARecv(sock, p, size, &numberOfBytesRecvd, 0, 0 , 0))
//#pragma warning(default: 4267)
//            return false;
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
//    return true;
//}
bool  initializeScket()
{

#ifndef  __GNUC__
    GUID GuidConnectEx = WSAID_CONNECTEX;
    GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    GUID GuidTransmitFile = WSAID_TRANSMITFILE;
    GUID GuidTransmitPackets = WSAID_TRANSMITPACKETS;

	SOCKET cliSock;
    DWORD dwBytes = 0;

#endif

    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;

    if (LOBYTE(wsaData.wVersion) != 2 ||
            HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return false;
    }


#ifndef  __GNUC__
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
#endif

    return true;
}

void shutdownSocket()
{
    WSACleanup();
}

#ifndef  __GNUC__
bool transmitFile(SOCKET hSocket,
                  HANDLE hFile,
                  DWORD nNumberOfBytesToWrite,
                  DWORD nNumberOfBytesPerSend,
                  LPOVERLAPPED lpOverlapped,
                  LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
                  DWORD dwFlags)
{

    return sys_call_with_boolean(transmitfile(hSocket
                                  , hFile
                                  , nNumberOfBytesToWrite
                                  , nNumberOfBytesPerSend
                                  , lpOverlapped
                                  , lpTransmitBuffers
                                  , dwFlags));
}

bool acceptEx(SOCKET sListenSocket,
              SOCKET sAcceptSocket,
              PVOID lpOutputBuffer,
              DWORD dwReceiveDataLength,
              DWORD dwLocalAddressLength,
              DWORD dwRemoteAddressLength,
              LPDWORD lpdwBytesReceived,
              LPOVERLAPPED lpOverlapped)
{
    return sys_call_with_boolean(__acceptex(sListenSocket,
                              sAcceptSocket,
                              lpOutputBuffer,
                              dwReceiveDataLength,
                              dwLocalAddressLength,
                              dwRemoteAddressLength,
                              lpdwBytesReceived,
                              lpOverlapped));
}

bool transmitPackets(SOCKET hSocket,
                     LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,
                     DWORD nElementCount,
                     DWORD nSendSize,
                     LPOVERLAPPED lpOverlapped,
                     DWORD dwFlags)
{
    return sys_call_with_boolean(__transmitpackets(hSocket,
                                     lpPacketArray,
                                     nElementCount,
                                     nSendSize,
                                     lpOverlapped,
                                     dwFlags));
}

bool connectEx(SOCKET s,
               const struct sockaddr* name,
               int namelen,
               PVOID lpSendBuffer,
               DWORD dwSendDataLength,
               LPDWORD lpdwBytesSent,
               LPOVERLAPPED lpOverlapped)
{
    return sys_call_with_boolean(__connectex(s,
                               name,
                               namelen,
                               lpSendBuffer,
                               dwSendDataLength,
                               lpdwBytesSent,
                               lpOverlapped));
}

bool disconnectEx(SOCKET hSocket,
                  LPOVERLAPPED lpOverlapped,
                  DWORD dwFlags,
                  DWORD reserved)
{
    return sys_call_with_boolean(__disconnectex(hSocket,
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


#endif  //__GNUC__

bool stringToAddress(const char* host
                     , struct sockaddr* addr
                     , unsigned int* len)
{
	const char* begin;
    memset(addr, 0, *len);
    addr->sa_family = AF_INET;

    begin = strstr(host, "://");
    if (NULL != begin)
    {
        if (begin != host && '6' == *(begin - 1))
            addr->sa_family = AF_INET6;

        begin += 3;
    }
    else
    {
        begin = host;
    }

    return sys_call(WSAStringToAddress((LPTSTR)begin
									, addr->sa_family
									, 0
									, addr
									, len));
}

unsigned int addressToString(struct sockaddr* addr
                     , unsigned int   len
                     , const char* schema
                     , unsigned int schema_len
                     , char* data
					 , unsigned int data_len)
{
	char* ptr = data;
	if(data_len > 16)
		return -1;

    if(NULL == schema)
	{
    	schema = "tcp";
    	schema_len = 3;
	}
    else if(-1 == schema_len)
    {
    	schema_len = strlen(schema);
    }


    if(schema_len >= data_len)
    	return -1;

	strncpy(ptr, schema, schema_len);
	data_len -= schema_len;
	ptr += schema_len;


	if(addr->sa_family == AF_INET6)
	{
		*ptr = '6';
		++ptr;
		--data_len;
	}


    if(3 >= data_len)
    	return -1;

	memcpy(ptr, "://", 3);
	data_len -= 3;
	ptr += 3;

	{

		DWORD addressLength = data_len;
		if (SOCKET_ERROR == WSAAddressToStringA(addr
						, len
						, NULL
						, ptr
						, &addressLength))
			return -1;

		ptr += addressLength;
	}

	*ptr = 0;
    return ptr-data;
}

#ifdef __cplusplus
}
#endif
