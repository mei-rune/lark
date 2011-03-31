
#ifndef _networking_h_
#define _networking_h_ 1

#include "ecore_config.h"
#include <winsock2.h>
#include <Mswsock.h>
#ifdef _MSC_VER
#include "win32/stdbool.h"
#else
#include <stdbool.h>
#endif
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


enum select_mode
{
    select_read = 1
    , select_write = 2
    , select_error = 4
};


/**
 * ��ʼ��socket����
 */
bool initializeScket();

/**
 * �ر�socket����
 */
void shutdownSocket();

/**
 * �ж� socket �Ƿ������ݿɶ�
 */
bool isReadable(SOCKET sock);

/**
 * �ж� socket �Ƿ��д
 */
bool isWritable(SOCKET sock);

/**
 * ���� socket �Ƿ�����
 */
bool setBlocking(SOCKET sock, bool val);

/**
 * �жϲ��ȴ�ֱ��socket���Խ��ж�(д)�������������ʱ
 * @params[ in ] timval ��ʱʱ��
 * @params[ in ] mode �жϵĵĲ������ͣ����select_modeö��
 * @return ���Բ�������true
 */
bool poll(SOCKET sock, const struct timeval* timeval, int select_mode);

/**
 * @see MSDN
 */
bool transmitFile(SOCKET hSocket,
                  HANDLE hFile,
                  DWORD nNumberOfBytesToWrite,
                  DWORD nNumberOfBytesPerSend,
                  LPOVERLAPPED lpOverlapped,
                  LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
                  DWORD dwFlags);

/**
 * @see MSDN
 */
bool acceptEx(SOCKET sListenSocket,
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
bool transmitPackets(SOCKET hSocket,
                     TRANSMIT_PACKETS_ELEMENT* lpPacketArray,
                     DWORD nElementCount,
                     DWORD nSendSize,
                     LPOVERLAPPED lpOverlapped,
                     DWORD dwFlags);

/**
 * @see MSDN
 */
bool connectEx(SOCKET s,
               const struct sockaddr* name,
               int namelen,
               PVOID lpSendBuffer,
               DWORD dwSendDataLength,
               LPDWORD lpdwBytesSent,
               LPOVERLAPPED lpOverlapped);

/**
 * @see MSDN
 */
bool disconnectEx(SOCKET hSocket,
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
 * �� <schema>://<addr>:<port> ��ʽ��ȡ�� addr �� port ת���� sockaddr����
 * �� schema �� port �ǿ�ѡ��,���� schema �����һ���ַ��� '6' ʱ��ʾ����
 * IPv6��ʽ.
 */
bool stringToAddress(const char* host
                     , struct sockaddr* addr
                     , unsigned int* len);

/**
 * ����ַת��Ϊ <schema>://<addr>:<port> ��ʽ���ַ���
 */
unsigned int addressToString(struct sockaddr* name
                     , unsigned int len
                     , const char* schema
                     , unsigned int schema_len
                     , char* data
					 , unsigned int data_len);


#ifdef __cplusplus
}
#endif

#endif // _networking_h_
