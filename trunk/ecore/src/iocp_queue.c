
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


ecore_rc _ecore_queue_create(ecore_queue_t* queue, char* err, size_t len)
{
	HANDLE  completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                       0,
                       0,
                       1);

    if(NULL == completion_port)
	{
		snprintf(err, len, "创建完成端口失败 - %s", _last_win_error());
    	return ECORE_RC_ERROR;
	}

	queue->internal = my_malloc(sizeof(iocp_t));
	((iocp_t*)queue->internal)->completion_port = completion_port;

	return ECORE_RC_OK;
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
ecore_rc  _ecore_queue_pop(ecore_queue_t* queue, void** data, int milli_seconds, char* err, size_t len)
{
	iocp_t* iocp = (iocp_t*) queue->internal;
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
            return ECORE_RC_TIMEOUT;

        case ERROR_SUCCESS:
            return ECORE_RC_OK;

        default:
			snprintf(err, len, "发生系统错误 - %s", _last_win_error());
            return ECORE_RC_ERROR;
        }
    }
    else
    {
        iocp_command_t *asynch_result = (iocp_command_t*)(((char*)overlapped) - offsetof(iocp_command_t,invocation));

        asynch_result->result_error = (result)?0:GetLastError();
		asynch_result->result_bytes_transferred = bytes_transferred;
		asynch_result->result_completion_key = (void*)completion_key;

		*data = asynch_result->data;
    }
    return ECORE_RC_OK;
}

#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX

ecore_rc  _ecore_queue_pop_some(ecore_queue_t* queue, void** data, size_t count, size_t* num, int milli_seconds, char* err, size_t len)
{
	iocp_t* iocp = (iocp_t*) queue->internal;
	ULONG numEntriesRemoved = 0;
    OVERLAPPED_ENTRY *overlapped_entries = (OVERLAPPED_ENTRY*)my_calloc(count, sizeof(OVERLAPPED_ENTRY));

    if (GetQueuedCompletionStatusEx(iocp->completion_port,
				  overlapped_entries,
                  count,
                  &numEntriesRemoved,
                  milli_seconds,
				  FALSE))
    {
        switch (GetLastError())
        {
        case WAIT_TIMEOUT:
            return ECORE_RC_TIMEOUT;

        case ERROR_SUCCESS:
            return ECORE_RC_OK;

        default:
			snprintf(err, len, "发生系统错误 - %s", _last_win_error());
            return ECORE_RC_ERROR;
        }
    }
    else
    {
		ULONG i;
		for( i = 0; i < numEntriesRemoved; ++ i)
		{
			iocp_command_t *asynch_result = (iocp_command_t*)(((char*)overlapped_entries[i].lpOverlapped) - offsetof(iocp_command_t,invocation));

			asynch_result->result_error = 0;
			asynch_result->result_bytes_transferred = overlapped_entries[i].dwNumberOfBytesTransferred;
			asynch_result->result_completion_key = (void*)overlapped_entries[i].lpCompletionKey;

			data[i] = asynch_result->data;
		}
		*num = numEntriesRemoved;
    }
    return ECORE_RC_OK;
}

#endif

ecore_rc _ecore_queue_push(ecore_queue_t* queue, void* data, char* err, size_t len)
{
	iocp_t* iocp = (iocp_t*) queue->internal;
	iocp_command_t* command = (iocp_command_t*)my_malloc(sizeof(iocp_command_t));
	memset(command, 0, sizeof(iocp_command_t));

	command->data = data;

	if(!PostQueuedCompletionStatus(iocp->completion_port, 0, 0, &command->invocation))
	{
		snprintf(err, len,  "发生系统错误 - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}
	return ECORE_RC_OK;
}

void _ecore_queue_finalize(ecore_queue_t* queue)
{
    CloseHandle(((iocp_t*) queue->internal)->completion_port);
    my_free(queue->internal);
	queue->internal = NULL;
}

ecore_rc _ecore_queue_pop_task(ecore_queue_t* queue, ecore_task_t** data, int milli_seconds, char* err, size_t len)
{
	return _ecore_queue_pop(queue, (void**)data, milli_seconds, err, len);
}

#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX
ecore_rc _ecore_queue_pop_some_task(ecore_queue_t* queue, ecore_task_t** data, size_t count, size_t* num, int milli_seconds, char* err, size_t len)
{
	return _ecore_queue_pop_some(queue, (void**)data, count, num, milli_seconds, err, len);
}
#endif


void _task_wrapper_run(void* data)
{
	struct _task_wrapper* command = (struct _task_wrapper*)data;
	(*command->task.fn)(command->task.data);
	my_free(command->command);
}

ecore_rc _ecore_queue_push_task(ecore_queue_t* queue, void (*fn)(void* data), void* data, char* err, size_t len)
{
	iocp_t* iocp = (iocp_t*) queue->internal;
	iocp_command_t* command = (iocp_command_t*)my_malloc(sizeof(iocp_command_t));
	memset(command, 0, sizeof(iocp_command_t));
	command->data = &command->task;
	command->task.fn = &_task_wrapper_run;
	command->task.data = &(command->warpper);

	command->warpper.command = command;
	command->warpper.task.fn = fn;
	command->warpper.task.data = data;

	if(!PostQueuedCompletionStatus(iocp->completion_port, 0, 0, &command->invocation))
	{
		snprintf(err, len,  "发生系统错误 - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}
	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
