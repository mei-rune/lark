
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "internal.h"
#include "networking.h"

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__

#ifndef FIBER_FLAG_FLOAT_SWITCH
#define FIBER_FLAG_FLOAT_SWITCH 0x1     // context switch floating point
#endif

#ifndef ConvertThreadToFiberEx
#define ConvertThreadToFiberEx(lpParameter,dwFlags)  ConvertThreadToFiber(lpParameter)
#endif

#endif

void* my_calloc(int _NumOfElements, int _SizeOfElements)
{
	void* ptr = my_malloc(_NumOfElements*_SizeOfElements);
	memset(ptr, 0, _NumOfElements*_SizeOfElements);
	return ptr;
}

void  my_free(void * _Memory)
{
 free(((char*)_Memory) - 4);
}

void* my_malloc(int _Size)
{
	return ((char*)malloc(_Size + 4)) + 4;
}

void* my_realloc(void * _Memory, int _NewSize)
{
	return ((char*)realloc(((char*)_Memory) - 4, _NewSize + 4)) + 4;
}

char*  my_strdup(const char * src)
{
	size_t len = strlen(src) + 1;
	char* ptr =(char*)my_calloc(len, sizeof(char));
	strcpy(ptr, src);
	return ptr;
}


const char* _last_win_error()
{
	return _last_win_error_with_code(GetLastError());
}

#ifdef __GNUC__
	__thread char* lpMsgBuf = NULL;
#else
	__declspec( thread ) char* lpMsgBuf = NULL;
#endif


const char* _last_win_error_with_code(unsigned long code)
{
	DWORD ret = 0;
	if(NULL != lpMsgBuf)
	{
		LocalFree(lpMsgBuf);
		lpMsgBuf = NULL;
	}

    ret = FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPSTR) & lpMsgBuf,
                    0,
                    NULL);
    if (ret <= 0)
    {
		lpMsgBuf = (char*)LocalAlloc(LMEM_FIXED, 65);
        strcpy(lpMsgBuf, "<FormatMessageA 出错>");
		return lpMsgBuf;
    }

    if ('\r' == lpMsgBuf[ret - 1 ] || '\n' == lpMsgBuf[ ret - 1 ])
        lpMsgBuf[ret - 1 ] = 0;
    if ('\r' == lpMsgBuf[ret - 2 ] || '\n' == lpMsgBuf[ ret - 2 ])
        lpMsgBuf[ret - 2 ] = 0;

    return lpMsgBuf;
}

const char* _last_crt_error()
{
	return strerror(errno);
}

const char* _last_crt_error_with_code(int code)
{
	return strerror(code);
}

void _set_last_error(ecore_t* core, const char* fmt, ... )
{
	//ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	va_list argptr;
	va_start(argptr, fmt);
	string_assign_vsprintf(&core->error, fmt, argptr);
	va_end(argptr);
}


DLL_VARIABLE ecore_rc ecore_init(ecore_t* c, char* err, size_t len)
{

	//最后，让我们假设一个线程中有2个纤程，总结一下纤程的用法：
	//1、使用ConverThreadToFiber(Ex)将当前线程转换到纤程，这是纤程F1
	//2、定义一个纤程函数，用于创建一个新纤程
	//3、纤程F1中调用CreateFiber(Ex)函数创建一个新的纤程F2
	//4、SwitchToFiber函数进行纤程切换，让新创建的纤程F2执行
	//5、F2纤程函数执行完毕的时候，使用SwitchToFiber转换到F1
	//6、在纤程F1中调用DeleteFiber来删除纤程F2
	//7、纤程F1中调用ConverFiberToThread，转换为线程
	//8、线程结束
	ecore_internal_t* internal = (ecore_internal_t*)my_calloc(1, sizeof(ecore_internal_t));
	internal->main_thread = GetCurrentFiber();
	ecore_dlink_initialize(&(internal->prepare_threads));
	ecore_dlink_initialize(&(internal->active_threads));
	ecore_dlink_initialize(&(internal->delete_threads));
	ecore_slink_initialize(&(internal->cleanups));


	c->internal = internal;
	c->is_running = 1;
	string_init(&c->error);

	if(ECORE_RC_OK != backend_init(c, err, len))
	{
		ConvertFiberToThread();
		my_free(internal);
		return ECORE_RC_ERROR;
	}

	return ECORE_RC_OK;
}

DLL_VARIABLE void ecore_at_exit(ecore_t* core, void (*cleanup_fn)(void*), void* context)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	ecore_cleanup_t* cleanup = (ecore_cleanup_t*) my_malloc(sizeof(ecore_cleanup_t));
	cleanup->cleanup_fn = cleanup_fn;
	cleanup->context = context;
	cleanup->_next = 0;
	ecore_slink_push(&internal->cleanups, cleanup);
}

ecore_rc _ecore_poll(ecore_t* core, int milli_seconds)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

	while(!ecore_dlink_is_empty(&(internal->prepare_threads)))
	{
		ecore_thread_t* thread = internal->prepare_threads._next;
		
		ecore_dlink_remove(thread);
		ecore_dlink_insert_at_next(&(internal->active_threads), thread);
		SwitchToFiber(thread->self);
	}

	while(!ecore_dlink_is_empty(&(internal->delete_threads)))
	{
		ecore_thread_t* thread = internal->delete_threads._next;
		ecore_dlink_remove(thread);
		DeleteFiber(thread->self);
		string_finialize(&thread->name);
		my_free(thread);
	}


	return backend_poll(core, milli_seconds);
}

DLL_VARIABLE ecore_rc ecore_loop(ecore_t* core, int milli_seconds)
{
	ecore_rc ret = ECORE_RC_OK;

	while(core->is_running)
	{
		ecore_rc ret = _ecore_poll(core, milli_seconds);
		if(ECORE_RC_OK == ret)
			continue;

		if(ECORE_RC_TIMEOUT != ret)
			break;
	}
	
	if(ECORE_RC_TIMEOUT == ret)
		return ECORE_RC_OK;

	return ret;
}




DLL_VARIABLE  void ecore_finialize(ecore_t* core)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	ecore_cleanup_t* cleanup = internal->cleanups._next;

	
	while(true)
	{
		ecore_rc ret = _ecore_poll(core, 1000);
		if(ECORE_RC_OK != ret)
		{
			if(ECORE_RC_TIMEOUT != ret)
				ecore_log_message(0, ECORE_LOG_FATAL, core->error.ptr);
			break;
		}
	}

	assert(ecore_dlink_is_empty(&(internal->prepare_threads)));
	assert(ecore_dlink_is_empty(&(internal->active_threads)));
	assert(ecore_dlink_is_empty(&(internal->delete_threads)));

	while(0 != cleanup)
	{
		ecore_cleanup_t* next = cleanup->_next;
		(*cleanup->cleanup_fn)(cleanup->context);
		my_free(cleanup);
		cleanup = next;
	}

	backend_cleanup(core);
	my_free(internal);
	core->internal = 0;
	string_finialize(&core->error);
}

DLL_VARIABLE  void ecore_shutdown(ecore_t* core)
{
	core->is_running = ECORE_RC_ERROR;
}

void _ecore_future_fire(ecore_future_t* future)
{
	SwitchToFiber(future->thread);
}

#ifdef __cplusplus
}
#endif
