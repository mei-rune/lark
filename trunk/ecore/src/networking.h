
#ifndef _networking_h_
#define _networking_h_ 1

#include "ecore_config.h"
#include <winsock2.h>
#include <Mswsock.h>
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

# ifndef _io_packect_buf_
# define _io_packect_buf_
typedef TRANSMIT_PACKETS_ELEMENT io_packect_buf;
# endif // ___iopack___

# ifndef _io_file_buf_
# define _io_file_buf_
typedef TRANSMIT_FILE_BUFFERS io_file_buf;
# endif // _io_file_buf_


#ifndef _MSC_VER
#define SO_UPDATE_CONNECT_CONTEXT   0x7010
#endif // _MSC_VER

#define IP_ADDRESS_LEN      60

enum select_mode
{
    select_read = 1
    , select_write = 2
    , select_error = 4
};


/**
 * 初始化socket服务
 */
ecore_rc initializeScket();

/**
 * 关闭socket服务
 */
void shutdownSocket();

/**
 * 判断 socket 是否有数据可读
 */
int isReadable(SOCKET sock);

/**
 * 判断 socket 是否可写
 */
int isWritable(SOCKET sock);

/**
 * 设置 socket 是否阻塞, val=1为阻塞， val=0为非阻塞
 */
ecore_rc setNonblocking(SOCKET sock);

/**
 * 判断并等待直到socket可以进行读(写)操作，或出错，或超时
 * @params[ in ] timval 超时时间
 * @params[ in ] mode 判断的的操作类型，请见select_mode枚举
 * @return 可以操作返回true
 */
int poll(SOCKET sock, const struct timeval* timeval, int select_mode);

/**
 * @see MSDN
 */
ecore_rc transmitFile(SOCKET hSocket,
                  HANDLE hFile,
                  DWORD nNumberOfBytesToWrite,
                  DWORD nNumberOfBytesPerSend,
                  LPOVERLAPPED lpOverlapped,
                  LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
                  DWORD dwFlags);

/**
 * @see MSDN
 */
ecore_rc acceptEx(SOCKET sListenSocket,
              SOCKET sAcceptSocket,
              PVOID lpOutputBuffer,
              DWORD dwReceiveDataLength,
              DWORD dwLocalAddressLength,
              DWORD dwRemoteAddressLength,
              LPDWORD lpdwBytesReceived,
              LPOVERLAPPED lpOverlapped);

/**
 * @see MSDN
 */
ecore_rc transmitPackets(SOCKET hSocket,
                     TRANSMIT_PACKETS_ELEMENT* lpPacketArray,
                     DWORD nElementCount,
                     DWORD nSendSize,
                     LPOVERLAPPED lpOverlapped,
                     DWORD dwFlags);

/**
 * @see MSDN
 */
ecore_rc connectEx(SOCKET s,
               const struct sockaddr* name,
               int namelen,
               PVOID lpSendBuffer,
               DWORD dwSendDataLength,
               LPDWORD lpdwBytesSent,
               LPOVERLAPPED lpOverlapped);

/**
 * @see MSDN
 */
ecore_rc disconnectEx(SOCKET hSocket,
                  LPOVERLAPPED lpOverlapped,
                  DWORD dwFlags,
                  DWORD reserved);

/**
 * @see MSDN
 */
void getAcceptExSockaddrs(PVOID lpOutputBuffer,
                          DWORD dwReceiveDataLength,
                          DWORD dwLocalAddressLength,
                          DWORD dwRemoteAddressLength,
                          LPSOCKADDR* LocalSockaddr,
                          LPINT LocalSockaddrLength,
                          LPSOCKADDR* RemoteSockaddr,
                          LPINT RemoteSockaddrLength);

/**
 * 从 <schema>://<addr>:<port> 格式中取出 addr 和 port 转换成 sockaddr，其
 * 中 schema 与 port 是可选的,其中 schema 中最后一个字符是 '6' 时表示采用
 * IPv6格式.
 */
ecore_rc stringToAddress(const char* host
                     , struct sockaddr* addr);

/**
 * 将地址转换为 <schema>://<addr>:<port> 格式的字符串
 * @return 成功返回转换后的字符串长度，否则返回 -1
 */
ecore_rc addressToString(struct sockaddr* name
                     , const char* schema
						 , size_t schema_len
						 , string_t* url);


#ifdef __cplusplus
}
#endif

#endif // _networking_h_
