
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

typedef struct _iocp_command {
	OVERLAPPED invocation;

	unsigned int  result_bytes_transferred;
	void*  result_completion_key;
	int    result_error;

	swap_context_t context;

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
        iocp_command_t *asynch_result = (iocp_command_t*)(((char*)overlapped) - offsetof(iocp_command_t,invocation));

        asynch_result->result_error = (result)?0:GetLastError();
		asynch_result->result_bytes_transferred = bytes_transferred;
		asynch_result->result_completion_key = (void*)completion_key;

		_ecore_fire_event(&(asynch_result->context));
    }
    return 0;
}

void  backend_cleanup(ecore_t* core){

	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

    CloseHandle(((iocp_t*) internal->backend)->completion_port);
    my_free(internal->backend);
	internal->backend = NULL;
}

int _parse_ip_address(ecore_t* core, struct sockaddr* addr, size_t len, const char* url)
{
	char host[50];
	const char* ptr = url;

	if('[' == *ptr)
	{
		const char* end = NULL;
		addr->sa_family = AF_INET6;

		if(NULL == (end = strchr(++ptr, ']')))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ']' - %s", url);
			return -1;
		}

		if( ':' != *(end + 1))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ':' - %s", url);
			return -1;
		}


		strncpy(host, ptr, end-ptr);
		host[end-ptr] =0;
		if(1 != inet_pton(AF_INET6, host, &(((struct sockaddr_in6*)addr)->sin6_addr)))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, %s - %s", _last_win_error(), url);
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
			_set_last_error(core, "tcp 的 url 格式不正确, 找不到符号 ':' - %s", url);
			return -1;
		}

		strncpy(host, ptr, end-ptr);
		host[end-ptr] =0;
		if(1 != inet_pton(AF_INET, host, &(((struct sockaddr_in*)addr)->sin_addr)))
		{
			_set_last_error(core, "tcp 的 url 格式不正确, %s - %s", _last_win_error(), url);
			return -1;
		}

		++ end;

		((struct sockaddr_in*)addr)->sin_port = htons(atoi(end));
		return 0;
	}
}
bool _create_listen_tcp(ecore_t* core, ecore_io_t* io, const char* url)
{
	struct sockaddr addr;
	ecore_io_internal_t* internal = NULL;
	socket_type sock;
	
	iocp_t* iocp = (iocp_t*) ((ecore_internal_t*)(core->internal))->backend;

	if(0 != _parse_ip_address(core, &addr, sizeof(struct sockaddr),  url))
		return false;

	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == sock)
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error());
		return false;
	}

    if(NULL == CreateIoCompletionPort((HANDLE)sock
									, iocp->completion_port
                                    , 0
                                    , 1))
	{
		_set_last_error(core, "尝试监听 '%s' 之前将 socket 绑定到完成端口失败时失败 - %s"
			, url
			, _last_win_error());
		goto err;
	}

	if(SOCKET_ERROR == bind(sock, &addr, sizeof(struct sockaddr)))
	{
		_set_last_error(core, "将 socket 绑定到 '%s' 上时失败, - %s", url, _last_win_error());
		goto err;
	}

	if(SOCKET_ERROR == listen(sock, SOMAXCONN))
	{
		_set_last_error(core, "将 socket[%s] 置为监时时失败, - %s", url, _last_win_error());
		goto err;
	}

	internal = (ecore_io_internal_t*)my_malloc(sizeof(ecore_io_internal_t));
	internal->io_sock = sock;
	internal->type = ecore_io_type_tcp_listen;

	io->core = core;
	io->internal = internal;
	string_sprintf(&io->name, "listen[socket=%d, address=%s]",  (int)internal->io_sock, url);
	string_create(&io->local_address,url);
	string_copy(&io->remote_address,&io->local_address);

	return true;

err:
	closesocket(sock);
	return false;
}

bool _create_ipc(ecore_t* core, ecore_io_t* io, const char* addr)
{
	return false;
}

DLL_VARIABLE bool ecore_io_listion_at(ecore_t* core, ecore_io_t* io, const string_t* str)
{
	memset(io, 0, sizeof(ecore_io_t));

	if(0 == strncasecmp("tcp://", string_data(str), 6))
		return _create_listen_tcp(core, io, string_data(str) + 6);
	else if(0 == strncasecmp("ipc://", string_data(str), 6))
		return _create_ipc(core, io, string_data(str) + 6);
	return false;
}

DLL_VARIABLE bool ecore_io_accept(ecore_io_t* listen_io, ecore_io_t* accepted_io)
{
	iocp_command_t command;
	socket_type accepted;
	ecore_io_internal_t* accepted_internal;

	char data_buf[ sizeof(SOCKADDR_STORAGE)*2 + sizeof(SOCKADDR_STORAGE)*2 + 100];
	
	iocp_t* iocp = (iocp_t*) ((ecore_internal_t*)(listen_io->core->internal))->backend;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)listen_io->internal;

	
	memset(accepted_io, 0, sizeof(ecore_io_t));
	memset(&command, 0, sizeof(iocp_command_t));


	accepted = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == accepted)
	{
		_set_last_error(listen_io->core, "创建 socket 失败, - %s", _last_win_error());
		return false;
	}

    if(NULL == CreateIoCompletionPort((HANDLE)accepted
									, iocp->completion_port
                                    , 0
                                    , 1))
	{
		_set_last_error(listen_io->core, "尝试向 '%s' 发送接收连接请求之前将 socket 绑定到完成端口失败时失败 - %s"
			, string_data(&listen_io->name)
			, _last_win_error());
		goto err;
	}

	{
		DWORD bytesTransferred;
		if(!acceptEx(internal->io_sock
				, accepted
				, data_buf
				, 0 //必须为0,否则会有大量的连接处于accept中，因为客户端可能故意只
					//建立连接，没有发送数据。
				, sizeof(SOCKADDR_STORAGE) + sizeof(SOCKADDR_STORAGE)
				, sizeof(SOCKADDR_STORAGE) + sizeof(SOCKADDR_STORAGE)
				, &bytesTransferred
				, &command.invocation)
		  && WSA_IO_PENDING != WSAGetLastError())
		{
			_set_last_error(listen_io->core, "接受器 '%s' 发起连接请求失败 - %s"
					, string_data(&listen_io->name)
					, _last_win_error());
			goto err;
		}
	}

	_ecore_wait(listen_io->core, &command.context);

	if (0 != command.result_error)
	{
		_set_last_error(listen_io->core, "接受器 '%s' 获取连接请求失败 - %s"
				, string_data(&listen_io->name)
				, _last_win_error_with_code(command.result_error));
		goto err;
	}



	{
		struct sockaddr *local_addr = 0;
		struct sockaddr *remote_addr = 0;
		int local_size = 0;
		int remote_size = 0;

		/// 超级奇怪!如果直接用 GetAcceptExSockaddrs 会失败,通过调
		/// 用 GetAcceptExSockaddrs 的函数指针就没有问题.
		getAcceptExSockaddrs(data_buf
						   , 0
						   , sizeof(SOCKADDR_STORAGE) + sizeof(SOCKADDR_STORAGE)
						   , sizeof(SOCKADDR_STORAGE) + sizeof(SOCKADDR_STORAGE)
						   , &local_addr
						   , &local_size
						   , &remote_addr
						   , &remote_size);

		if (-1 == _address_to_string(remote_addr
				, remote_size
				, NULL
				, 0
				, &accepted_io->remote_address))
		{
			_set_last_error(listen_io->core, "接受器 '%s' 获取连接请求返回,获取远程地址失败 - %s"
					, string_data(&listen_io->name)
					, _last_win_error());
			goto err;
		}

		if (-1 == _address_to_string(local_addr
				, local_size
				, NULL
				, 0
				, &accepted_io->local_address))
		{
			_set_last_error(listen_io->core, "接受器 '%s' 获取连接请求返回,获取本地地址失败 - %s"
					, string_data(&listen_io->name)
					, _last_win_error());
			goto err;
		}
	}


     if (SOCKET_ERROR == setsockopt(accepted, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                                    (char *) &(internal->io_sock), sizeof(internal->io_sock)))
     {
			_set_last_error(listen_io->core, "接受器 '%s' 获取连接请求返回,在对 socket 句柄设置 SO_UPDATE_ACCEPT_CONTEXT 选项时发生错误 - "
					, string_data(&listen_io->name)
					, _last_win_error());
			goto err;
     }

	accepted_internal = (ecore_io_internal_t*)my_malloc(sizeof(ecore_io_internal_t));
	accepted_internal->io_sock = accepted;
	accepted_internal->type = ecore_io_type_tcp_accepted;

	string_sprintf(&accepted_io->name, "acceped[socket=%d, local_address=%s, remote_address=%s]"
		, (int)accepted_internal->io_sock
		, string_data(&accepted_io->local_address)
		, string_data(&accepted_io->remote_address));


	accepted_io->core = listen_io->core;
	accepted_io->internal = accepted_internal;
	return true;
err:
	closesocket(accepted);
	return false;
}

DLL_VARIABLE bool ecore_io_connect(ecore_t* core, ecore_io_t* io, const string_t* url)
{
	struct sockaddr addr;
	iocp_command_t command;
	socket_type connected;
	ecore_io_internal_t* connected_internal;

	iocp_t* iocp = (iocp_t*) ((ecore_internal_t*)(core->internal))->backend;

	
	memset(io, 0, sizeof(ecore_io_t));
	
	if(0 != _parse_ip_address(core, &addr, sizeof(struct sockaddr), string_data(url)))
		return false;

	connected = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == connected)
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error());
		return false;
	}

    if(NULL == CreateIoCompletionPort((HANDLE)connected
									, iocp->completion_port
                                    , 0
                                    , 1))
	{
		_set_last_error(core, "尝试向 '%s' 连接请求之前将 socket 绑定到完成端口失败时失败 - %s"
					, string_data(url)
					, _last_win_error());
		goto err;
	}

	{
		DWORD bytesTransferred;
		if(!connectEx(connected
				, &addr
				, sizeof(addr) 
				, 0
				, 0
				, &bytesTransferred
				, &command.invocation)
		  && WSA_IO_PENDING != WSAGetLastError())
		{
			_set_last_error(core, "尝试向 '%s' 发送连接请求时失败 - %s"
					, string_data(url)
					, _last_win_error());
			goto err;
		}
	}

	_ecore_wait(core, &command.context);

	if (0 != command.result_error)
	{
		_set_last_error(core, "尝试连接到 '%s' 失败 - %s"
				, string_data(url)
				, _last_win_error_with_code(command.result_error));
		goto err;
	}


    if (SOCKET_ERROR == setsockopt(connected, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT,
                                    NULL, 0))
    {
			_set_last_error(core, "尝试连接到 '%s' 成功后，在对 socket 句柄设置 SO_UPDATE_CONNECT_CONTEXT 选项时发生错误 - "
					, string_data(url)
					, _last_win_error());
			goto err;
    }


	string_copy(&io->remote_address, url);

	{
		// 取本地地址
		struct sockaddr name;
        int namelen = sizeof(name);

        if (SOCKET_ERROR == getsockname(connected, & name, &namelen))
        {
			_set_last_error(core, "尝试连接到 '%s' 成功后，取本地地址时失败 - "
					, string_data(url)
					, _last_win_error());
			goto err;
        }

		if (!_address_to_string(&name, namelen, 0, 0, &io->local_address))
        {
			_set_last_error(core, "尝试连接到 '%s' 成功后，转换本地地址时失败 - "
					, string_data(url)
					, _last_win_error());
			goto err;
        }
	}

	connected_internal = (ecore_io_internal_t*)my_malloc(sizeof(ecore_io_internal_t));
	connected_internal->io_sock = connected;
	connected_internal->type = ecore_io_type_tcp_connected;

	string_sprintf(&io->name, "acceped[socket=%d, local_address=%s, remote_address=%s]"
		, (int)connected_internal->io_sock
		, string_data(&io->local_address)
		, string_data(&io->remote_address));

	io->core = core;
	io->internal = connected_internal;
	return true;
err:
	closesocket(connected);
	return false;
}

DLL_VARIABLE void ecore_io_close(ecore_io_t* io)
{
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	switch(internal->type)
	{
	case ecore_io_type_tcp_listen:
	case ecore_io_type_tcp_accepted:
	case ecore_io_type_tcp_connected:
		closesocket(internal->io_sock);
		break;
	case ecore_io_type_ipc:
		break;
	case ecore_io_type_file:
		//CloseFile(internal->io_file);
		break;
	default:
		assert(false);
	}

	my_free(internal);
	string_finalize(&io->name);
	string_finalize(&io->local_address);
	string_finalize(&io->remote_address);
}

DLL_VARIABLE unsigned int ecore_io_write_some(ecore_io_t* io, const void* buf, unsigned int len)
{
	DWORD numberOfBytesWrite;
	iocp_command_t command;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));

	if(!WriteFile(internal->io_file
		, buf
		, len
		, &numberOfBytesWrite
		, &command.invocation)
	   && WSA_IO_PENDING != WSAGetLastError())
	{
		_set_last_error(io->core, _last_win_error());
		return -1;
	}
	_ecore_wait(io->core, &command.context);
	return (0 == command.result_error)? command.result_bytes_transferred:-1;
}

DLL_VARIABLE bool ecore_io_write(ecore_io_t* io, const void* buf, unsigned int len)
{
	const char* ptr = (const char*)buf;
	do
	{
		unsigned int n = ecore_io_write_some(io, ptr, len);
	    if (-1 == n)
	        return false;

	    assert( n <= len );

	    len -= n;
	    ptr += n;
	}
	while (0 != len);

	return true;
}

DLL_VARIABLE unsigned int ecore_io_read_some(ecore_io_t* io, void* buf, unsigned int len)
{
	DWORD numberOfBytesRead;
	iocp_command_t command;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));

	if(!ReadFile(internal->io_file
		, buf
		, len
		, &numberOfBytesRead
		, &command.invocation)
	   && WSA_IO_PENDING != WSAGetLastError())
	{
		_set_last_error(io->core, _last_win_error());
		return -1;
	}

	_ecore_wait(io->core, &command.context);
	return (0 == command.result_error)? command.result_bytes_transferred:-1;
}

DLL_VARIABLE bool ecore_io_read(ecore_io_t* io, void* buf, unsigned int len)
{
	char* ptr = (char*)buf;
	do
	{
		unsigned int n = ecore_io_read_some(io, ptr, len);
	    if (-1 == n)
	        return false;

	    assert( n <= len );

	    len -= n;
	    ptr += n;
	}
	while (0 != len);

	return true;
}


#ifdef __cplusplus
}
#endif
