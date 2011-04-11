#ifndef _backend_iocp_h_
#define _backend_iocp_h_ 1

#include "internal.h"
#include "networking.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

	
typedef struct _backend_iocp{
    // Íê³É¶Ë¿Ú¾ä±ú
    HANDLE completion_port;
} iocp_t;

typedef struct _iocp_command iocp_command_t;

struct _task_wrapper
{
	ecore_task_t task;
	iocp_command_t* command;
};

typedef struct _iocp_command {
	OVERLAPPED invocation;

	size_t  result_bytes_transferred;
	void*   result_completion_key;
	int     result_error;

	void* data;
	ecore_task_t task;
	struct _task_wrapper warpper;

	ecore_future_t future;

} iocp_command_t;


#ifdef __cplusplusi
}
#endif

#endif //_backend_iocp_h_
