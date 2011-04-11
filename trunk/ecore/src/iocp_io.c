
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <memory.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "iocp.h"


#ifdef __cplusplus
extern "C" 
{
#endif

ecore_rc _create_listen_tcp(ecore_t* core, ecore_io_t* io, const char* url)
{
	struct sockaddr addr;
	ecore_io_internal_t* internal = NULL;
	socket_type sock;

	iocp_t* iocp = (iocp_t*) (core->in.internal);

	if(ECORE_RC_OK != stringToAddress(url, &addr))
	{
		_set_last_error(core, "转换地址 ‘%s’ 失败 - %s", url, _last_win_error());
		return ECORE_RC_ERROR;
	}

	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == sock)
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error());
		return ECORE_RC_ERROR;
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
	string_assign_sprintf(&io->name, "listen[socket=%d, address=%s]",  (int)internal->io_sock, url);
	string_assign(&io->local_address,url);
	string_copy(&io->remote_address,&io->local_address);

	return ECORE_RC_OK;

err:
	closesocket(sock);
	return ECORE_RC_ERROR;
}

ecore_rc _create_ipc(ecore_t* core, ecore_io_t* io, const char* addr)
{
	return ECORE_RC_ERROR;
}

DLL_VARIABLE ecore_rc ecore_io_listion_at_url(ecore_t* core, ecore_io_t* io, const char* str)
{
	if(0 == strncasecmp("tcp://", str, 6))
		return _create_listen_tcp(core, io, str + 6);
	else if(0 == strncasecmp("ipc://", str, 6))
		return _create_ipc(core, io, str + 6);
	return ECORE_RC_ERROR;
}

DLL_VARIABLE ecore_rc ecore_io_listion_at(ecore_t* core, ecore_io_t* io, const string_t* str)
{
	return ecore_io_listion_at_url(core, io, string_data(str));
}

DLL_VARIABLE ecore_rc ecore_io_accept(ecore_io_t* listen_io, ecore_t* core, ecore_io_t* accepted_io)
{
	char data_buf[ sizeof(SOCKADDR_STORAGE)*2 + sizeof(SOCKADDR_STORAGE)*2 + 100];

	iocp_command_t command;
	socket_type accepted;
	ecore_io_internal_t* accepted_internal;
	
	iocp_t* iocp = (iocp_t*) (core->in.internal);
	ecore_io_internal_t* internal = (ecore_io_internal_t*)listen_io->internal;

	memset(&command, 0, sizeof(iocp_command_t));

	command.data = &command.task;
	command.task.fn = (void (*)(void*))&_ecore_future_fire;
	command.task.data = &command.future;


	accepted = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == accepted)
	{
		_set_last_error(listen_io->core, "创建 socket 失败, - %s", _last_win_error());
		return ECORE_RC_ERROR;
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

	_ecore_future_wait(listen_io->core, &command.future);

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

		if (ECORE_RC_OK != addressToString(remote_addr
				, NULL
				, 0
				, &accepted_io->remote_address))
		{
			_set_last_error(listen_io->core, "接受器 '%s' 获取连接请求返回,获取远程地址失败 - %s"
					, string_data(&listen_io->name)
					, _last_win_error());
			goto err;
		}

		if (ECORE_RC_OK != addressToString(local_addr
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

	string_assign_sprintf(&accepted_io->name, "acceped[socket=%d, local_address=%s, remote_address=%s]"
		, (int)accepted_internal->io_sock
		, string_data(&accepted_io->local_address)
		, string_data(&accepted_io->remote_address));


	accepted_io->core = core;
	accepted_io->internal = accepted_internal;
	return ECORE_RC_OK;
err:
	closesocket(accepted);
	return ECORE_RC_ERROR;
}

DLL_VARIABLE ecore_rc ecore_io_connect_to(ecore_t* core, ecore_io_t* io, const string_t* url)
{
	struct sockaddr addr;
	iocp_command_t command;
	socket_type connected;
	ecore_io_internal_t* connected_internal;

	iocp_t* iocp = (iocp_t*) (core->in.internal);

	memset(&command, 0, sizeof(iocp_command_t));
	command.data = &command.task;
	command.task.fn = (void (*)(void*))&_ecore_future_fire;
	command.task.data = &command.future;

	if(ECORE_RC_OK != stringToAddress(string_data(url), &addr))
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}

	connected = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == connected)
	{
		_set_last_error(core, "创建 socket 失败, - %s", _last_win_error());
		return ECORE_RC_ERROR;
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

	_ecore_future_wait(core, &command.future);

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

		if (ECORE_RC_OK != addressToString(&name, 0, 0, &io->local_address))
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

	string_assign_sprintf(&io->name, "acceped[socket=%d, local_address=%s, remote_address=%s]"
		, (int)connected_internal->io_sock
		, string_data(&io->local_address)
		, string_data(&io->remote_address));

	io->core = core;
	io->internal = connected_internal;
	return ECORE_RC_OK;
err:
	closesocket(connected);
	return ECORE_RC_ERROR;
}


DLL_VARIABLE ecore_rc ecore_io_connect_to_url(ecore_t* core, ecore_io_t* io, const char* url)
{
	string_t str = STRING_T_DEFAULT;
	ecore_rc rc = ecore_io_connect_to(core, io, &str);
	string_finialize(&str);
	return rc;
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
	string_finialize(&io->name);
	string_finialize(&io->local_address);
	string_finialize(&io->remote_address);
}

DLL_VARIABLE size_t ecore_io_write_some(ecore_io_t* io, const void* buf, size_t len)
{
	DWORD numberOfBytesWrite;
	iocp_command_t command;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));
	command.data = &command.task;
	command.task.fn = (void (*)(void*))&_ecore_future_fire;
	command.task.data = &command.future;

	if(!WriteFile(internal->io_file
		, buf
		, len
		, &numberOfBytesWrite
		, &command.invocation)
	   && WSA_IO_PENDING != WSAGetLastError())
	{
		_set_last_error(io->core, "'%s' 写数据时发生错误 - %s", string_data(&io->name), _last_win_error());
		return -1;
	}
	_ecore_future_wait(io->core, &command.future);

	if(0 != command.result_error)
	{
		_set_last_error(io->core, "'%s' 写数据时发生错误 - %s", string_data(&io->name), _last_win_error_with_code(command.result_error));
		return -1;
	}
	return command.result_bytes_transferred;
}

DLL_VARIABLE ecore_rc ecore_io_write(ecore_io_t* io, const void* buf, size_t len)
{
	const char* ptr = (const char*)buf;
	do
	{
		size_t n = ecore_io_write_some(io, ptr, len);
	    if (-1 == n)
	        return ECORE_RC_ERROR;

	    assert( n <= len );

	    len -= n;
	    ptr += n;
	}
	while (0 != len);

	return ECORE_RC_OK;
}

DLL_VARIABLE size_t ecore_io_read_some(ecore_io_t* io, void* buf, size_t len)
{
	DWORD numberOfBytesRead;
	iocp_command_t command;
	ecore_io_internal_t* internal = (ecore_io_internal_t*)io->internal;
	memset(&command, 0, sizeof(iocp_command_t));
	command.data = &command.task;
	command.task.fn = (void (*)(void*))&_ecore_future_fire;
	command.task.data = &command.future;

	if(!ReadFile(internal->io_file
		, buf
		, len
		, &numberOfBytesRead
		, &command.invocation)
	   && WSA_IO_PENDING != WSAGetLastError())
	{
		_set_last_error(io->core, "'%s' 读数据时发生错误 - %s", string_data(&io->name), _last_win_error());
		return -1;
	}

	_ecore_future_wait(io->core, &command.future);

	if(0 != command.result_error)
	{
		_set_last_error(io->core, "'%s' 读数据时发生错误 - %s", string_data(&io->name), _last_win_error_with_code(command.result_error));
		return -1;
	}
	
	if(0 == command.result_bytes_transferred)
	{
		_set_last_error(io->core, "'%s' 读数据时发现用户主动关闭连接", string_data(&io->name));
		return 0;
	}

	return command.result_bytes_transferred;
}

DLL_VARIABLE ecore_rc ecore_io_read(ecore_io_t* io, void* buf, size_t len)
{
	char* ptr = (char*)buf;
	do
	{
		size_t n = ecore_io_read_some(io, ptr, len);
	    if (-1 == n)
	        return ECORE_RC_ERROR;

	    assert( n <= len );

	    len -= n;
	    ptr += n;
	}
	while (0 != len);

	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
