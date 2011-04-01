
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

void _set_last_error(ecore_t* core, const char* fmt, ... )
{
	//ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	va_list argptr;
	va_start(argptr, fmt);
	string_assign_vsprintf(&core->error, fmt, argptr);
	va_end(argptr);
}

DLL_VARIABLE ecore_rc ecore_init(ecore_t* c, char* err, int len)
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
	ecore_internal_t* internal;
	void* main_thread;


	if(ECORE_RC_OK != initializeScket()){
		snprintf(err, len, "初始化网络库失败 - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}

	main_thread = ConvertThreadToFiberEx(c,FIBER_FLAG_FLOAT_SWITCH);
	if( NULL == main_thread){
		snprintf(err, len, "将当前线程转换到纤程失败 - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}

	internal = (ecore_internal_t*)my_malloc(sizeof(ecore_internal_t));
	internal->main_thread = main_thread;
	DLINK_Initialize(&(internal->active_threads));
	DLINK_Initialize(&(internal->delete_threads));


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

DLL_VARIABLE  void ecore_finalize(ecore_t* core)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

	assert(DLINK_IsEmpty(&(internal->active_threads)));
	assert(DLINK_IsEmpty(&(internal->delete_threads)));

	backend_cleanup(core);

	my_free(internal);
}

DLL_VARIABLE ecore_rc ecore_poll(ecore_t* core, int milli_seconds)
{
	//ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	if(!core->is_running)
	{
		_set_last_error(core, "系统已停止.");
		return ECORE_RC_ERROR;
	}

	return backend_poll(core, milli_seconds);
}

DLL_VARIABLE  void ecore_shutdown(ecore_t* core)
{
	core->is_running = ECORE_RC_ERROR;
}

void CALLBACK _ecore_fiber_proc(void* data)
{
	ecore_thread_t* context = (ecore_thread_t*)data;
	ecore_internal_t* internal = (ecore_internal_t*)context->core->internal;

	(*(context->callback_fn))(context->context);

	DLINK_Remove(context);
	DLINK_InsertNext(&(internal->delete_threads), context);


	// 注意这个必须是最后一行
	SwitchToFiber(context->back_thread);
}

DLL_VARIABLE ecore_rc ecore_start_thread(ecore_t* core, void (*callback_fn)(void*), void* context)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	ecore_thread_t* data = (ecore_thread_t*)my_malloc(sizeof(ecore_thread_t));

	data->core = core;
	data->back_thread = internal->main_thread;
	data->callback_fn = callback_fn;
	data->context = context;

	data->self = CreateFiberEx(0,0,FIBER_FLAG_FLOAT_SWITCH,&_ecore_fiber_proc, data);
	if(NULL == data->self)
	{
		_set_last_error(core, "创建纤程失败 - %s", _last_win_error());
		my_free(data);
		return ECORE_RC_ERROR;
	}

	DLINK_InsertNext(&(internal->active_threads), data);
	SwitchToFiber(data->self);
	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
