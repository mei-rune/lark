
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "internal.h"
#include "networking.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _backend_iocp{
    // 完成端口句柄
    HANDLE completion_port;
} iocp_t;


//enum io_type
//{
//	io_type_tcp = 1,
//	io_type_ipc = 2,
//	io_type_file = 3
//};


enum iocp_command_type
{
	aio_command_function,
	aio_command_buffer
};

typedef struct _iocp_command {
	OVERLAPPED req;

	DWORD bytes_transferred;
	void * completion_key;
	int error;

	
} iocp_command_t;

int  backend_init(ecore_t* core, char* err, int len)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

	HANDLE  completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                       0,
                       0,
                       1);

    if(NULL == completion_port)
	{
		snprintf(err, len, "创建完成端口失败 - %s", _last_win_error());
    	return -1;
	}

	internal->backend = my_malloc(sizeof(iocp_t));
	((iocp_t*)internal->backend)->completion_port = completion_port;

	return 0;
}


/// If the function dequeues a completion packet for a successful I/O operation
/// from the completion port, the return value is nonzero. The function stores
/// information in the variables pointed to by the lpNumberOfBytesTransferred,
/// lpCompletionKey, and lpOverlapped parameters
///
/// 如果函数从端口取出一个完成包，且完成操作是成功的，则返回非0值。上下文数据
/// 保存在lpNumberOfBytesTransferred，lpCompletionKey，lpOverlapped中
///
/// If *lpOverlapped is NULL and the function does not dequeue a completion packet
/// from the completion port, the return value is zero. The function does not
/// store information in the variables pointed to by the lpNumberOfBytes and
/// lpCompletionKey parameters. To get extended error information, call GetLastError.
/// If the function did not dequeue a completion packet because the wait timed out,
/// GetLastError returns WAIT_TIMEOUT.
///
/// 如lpOverlapped 是NULL，没有从端口取出一个完成包，则返回0值。lpNumberOfBytesTransferred
/// ，lpCompletionKey，lpOverlapped也没有保存上下文数据，可以用GetLastError取
/// 得详细错误。如果没有从端口取出一个完成包，可能是超时，GetLastError返回WAIT_TIMEOUT
///
/// If *lpOverlapped is not NULL and the function dequeues a completion packet for
/// a failed I/O operation from the completion port, the return value is zero.
/// The function stores information in the variables pointed to by lpNumberOfBytes,
/// lpCompletionKey, and lpOverlapped. To get extended error information, call GetLastError.
///
/// 如果 lpOverlapped 不是NULL，但完成操作是失败的，则返回0值。上下文数据保存在
/// lpNumberOfBytesTransferred，lpCompletionKey，lpOverlapped中，可以用GetLastError
/// 取得详细错误。
///
/// If a socket handle associated with a completion port is closed, GetQueuedCompletionStatus
/// returns ERROR_SUCCESS, with *lpOverlapped non-NULL and lpNumberOfBytes equal zero.
///
/// 如一个socket句柄被关闭了，GetQueuedCompletionStatus返回ERROR_SUCCESS， lpOverlapped
/// 不是NULL,lpNumberOfBytes等于0。
int  backend_poll(ecore_t* core, int milli_seconds){
	
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	iocp_t* iocp = (iocp_t*) internal->backend;
    OVERLAPPED *overlapped = 0;
    DWORD bytes_transferred = 0;

    ULONG_PTR completion_key = 0;

    BOOL result = GetQueuedCompletionStatus(iocp->completion_port,
                  &bytes_transferred,
                  &completion_key,
                  &overlapped,
                  milli_seconds);
    if (NULL == overlapped)
    {
        switch (GetLastError())
        {
        case WAIT_TIMEOUT:
            return 1;

        case ERROR_SUCCESS:
            return 0;

        default:
            return -1;
        }
    }
    else
    {
        iocp_command_t *asynch_result = (iocp_command_t*)(((char*)overlapped) - offsetof(iocp_command_t,req));
        if (!result)
            asynch_result->error = GetLastError();

		asynch_result->bytes_transferred = bytes_transferred;
		asynch_result->completion_key = completion_key;

		_ecore_switch(asynch_result);

		//if(aio_command_buffer == asynch_result->type)
		//	SwitchToFiber(asynch_result->data.thread);
		//else
		//	asynch_result->data.func.complete_fn(asynch_result->data.func.context);
    }
    return 0;	
}

void  backend_cleanup(ecore_t* core){
	
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

    CloseHandle(((iocp_t*) internal->backend)->completion_port);
    my_free(internal->backend);
	internal->backend = NULL;
}

int _parse_ip_address(ecore_t* core, struct sockaddr* addr, const char* addr_and_port)
{
	char host[50];
	const char* ptr = addr_and_port;

	if('[' == *ptr)
	{
		const char* end = NULL;
		addr->sa_family = AF_INET6;

		if(NULL == (end = strchr(++ptr, ']')))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ']' - %s", addr_and_port); 
			return -1;
		}

		if( ':' != *(end + 1))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ':' - %s", addr_and_port); 
			return -1;
		}
				
		
		strncpy(host, ptr, end-ptr);
		if(1 != inet_pton(AF_INET6, host, &(((struct sockaddr_in6*)addr)->sin6_addr)))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, %s - %s", _last_win_error(), addr_and_port); 
			return -1;
		}
		
		++ end;

		((struct sockaddr_in6*)addr)->sin6_port = htons(atoi(end));
		return 0;
	}
	else
	{
		const char* end = NULL;
		addr->sa_family = AF_INET;

		if(NULL == (end = strchr(ptr, ':')))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ':' - %s", addr_and_port); 
			return -1;
		}

		strncpy(host, ptr, end-ptr);
		if(1 != inet_pton(AF_INET, host, &(((struct sockaddr_in*)addr)->sin_addr)))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, %s - %s", _last_win_error(), addr_and_port); 
			return -1;
		}
		
		++ end;

		((struct sockaddr_in*)addr)->sin_port = htons(atoi(end));
		return 0;
	}
}
ecore_io_t* _create_tcp(ecore_t* core, const char* addr_and_port)
{
	struct sockaddr addr;
	ecore_io_t* io = NULL;
	ecore_io_internal_t* internal = NULL;
	socket_type sock;

	if(0 != _parse_ip_address(core, &addr, addr_and_port))
		return NULL;

	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == sock)
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error()); 
		return ;
	}

	if(SOCKET_ERROR == bind(sock, &addr, sizeof(struct sockaddr)))
	{
		_set_last_error(core, "将 socket 绑定到 '%s' 上时失败, - %s", addr_and_port, _last_win_error()); 
		return ;
	}

	if(SOCKET_ERROR == listen(sock, SOMAXCONN))
	{
		_set_last_error(core, "将 socket[%s] 置为监时时失败, - %s", addr_and_port, _last_win_error()); 
		return ;
	}

	internal = (ecore_io_internal_t*)my_malloc(sizeof(ecore_io_internal_t));
	internal->file = (HANDLE)sock;
	
	io = (ecore_io_t*)my_malloc(sizeof(ecore_io_t));

	io->core = core;
	io->address = string_create(addr_and_port);
	return io;
}

ecore_io_t* _create_ipc(ecore_t* core, const char* addr)
{
	return NULL;
}

DLL_VARIABLE ecore_io_t* ecore_io_listion_at(ecore_t* core, const string_t* str)
{
	if(0 == strncasecmp("tcp://", string_data(str), 6))
		return _create_tcp(core, string_data(str) + 6);
	else if(0 == strncasecmp("ipc://", string_data(str), 6))
		return _create_ipc(core, string_data(str) + 6);
	return NULL;
}

DLL_VARIABLE ecore_io_t* ecore_io_accept(ecore_io_t* io)
{
	iocp_command_t command;
	socket_type accepted;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));

	
	accepted = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == accepted)
	{
		_set_last_error(io->core, "创建 socket 失败, - %s", _last_win_error()); 
		return 0;
	}
	
	//if(acceptEx(internal->file,accepted,
 //             PVOID lpOutputBuffer,
 //             DWORD dwReceiveDataLength,
 //             DWORD dwLocalAddressLength,
 //             DWORD dwRemoteAddressLength,
 //             LPDWORD lpdwBytesReceived,
 //             LPOVERLAPPED lpOverlapped) )
	//{
	//	_ecore_wait(io->core);
	//	return (0 == command.error)? command.bytes_transferred:-1;
	//}
	return 0;
}

DLL_VARIABLE ecore_io_t* ecore_io_connect(ecore_io_t* io, const string_t* str);
DLL_VARIABLE void ecore_io_close(ecore_io_t* io);

DLL_VARIABLE int ecore_io_write_some(ecore_io_t* io, const void* buf, unsigned int len)
{
	iocp_command_t command;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));

	if( WriteFileEx(internal->file
		, buf
		, len
		, (LPOVERLAPPED)(((char*)&command) - offsetof(iocp_command_t,req))
		, NULL ))
	{
		_ecore_wait(io->core);
		return (0 == command.error)? command.bytes_transferred:-1;
	}
	return -1;
}

DLL_VARIABLE int ecore_io_write(ecore_io_t* io, const void* buf, int len);
DLL_VARIABLE int ecore_io_read_some(ecore_io_t* io, void* buf, int len);
DLL_VARIABLE int ecore_io_read(ecore_io_t* io, void* buf, int len);



#ifdef __cplusplus
}
#endif
