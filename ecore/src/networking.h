
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
 * ��ʼ��socket����
 */
ecore_rc initializeScket();

/**
 * �ر�socket����
 */
void shutdownSocket();

/**
 * �ж� socket �Ƿ������ݿɶ�
 */
int isReadable(SOCKET sock);

/**
 * �ж� socket �Ƿ��д
 */
int isWritable(SOCKET sock);

/**
 * ���� socket �Ƿ�����, val=1Ϊ������ val=0Ϊ������
 */
ecore_rc setNonblocking(SOCKET sock);

/**
 * �жϲ��ȴ�ֱ��socket���Խ��ж�(д)�������������ʱ
 * @params[ in ] timval ��ʱʱ��
 * @params[ in ] mode �жϵĵĲ������ͣ����select_modeö��
 * @return ���Բ�������true
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
 * �� <schema>://<addr>:<port> ��ʽ��ȡ�� addr �� port ת���� sockaddr����
 * �� schema �� port �ǿ�ѡ��,���� schema �����һ���ַ��� '6' ʱ��ʾ����
 * IPv6��ʽ.
 */
ecore_rc stringToAddress(const char* host
                     , struct sockaddr* addr);

/**
 * ����ַת��Ϊ <schema>://<addr>:<port> ��ʽ���ַ���
 * @return �ɹ�����ת������ַ������ȣ����򷵻� -1
 */
ecore_rc addressToString(struct sockaddr* name
                     , const char* schema
						 , size_t schema_len
						 , string_t* url);


#ifdef __cplusplus
}
#endif

#endif // _networking_h_
