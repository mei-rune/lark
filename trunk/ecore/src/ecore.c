
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "internal.h"

#ifdef __cplusplus
extern "C" {
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
	return __last_win_error(GetLastError());
}

__declspec( thread ) char* lpMsgBuf = NULL;
 


const char* __last_win_error(unsigned long code)
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
        strcpy(lpMsgBuf, "<FormatMessageA ����>");
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
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(internal->err, ECORE_MAX_ERR_LEN, fmt, argptr);
	va_end(argptr);
}

DLL_VARIABLE  int ecore_init(ecore_t* c, char* err, int len)
{

	//��������Ǽ���һ���߳�����2���˳̣��ܽ�һ���˳̵��÷���
	//1��ʹ��ConverThreadToFiber(Ex)����ǰ�߳�ת�����˳̣������˳�F1
	//2������һ���˳̺��������ڴ���һ�����˳�
	//3���˳�F1�е���CreateFiber(Ex)��������һ���µ��˳�F2
	//4��SwitchToFiber���������˳��л������´������˳�F2ִ��
	//5��F2�˳̺���ִ����ϵ�ʱ��ʹ��SwitchToFiberת����F1
	//6�����˳�F1�е���DeleteFiber��ɾ���˳�F2
	//7���˳�F1�е���ConverFiberToThread��ת��Ϊ�߳�
	//8���߳̽���
	ecore_internal_t* internal;
	void* main_thread = ConvertThreadToFiberEx(c,FIBER_FLAG_FLOAT_SWITCH);
	if( NULL == main_thread){
		snprintf(err, len, "����ǰ�߳�ת�����˳�ʧ�� - %s", _last_win_error());  
		return -1;
	}

	internal = (ecore_internal_t*)my_malloc(sizeof(ecore_internal_t));
	internal->main_thread = main_thread;
	DLINK_Initialize(&(internal->active_threads));
	DLINK_Initialize(&(internal->delete_threads));

	
	c->internal = internal;
	c->is_running = true;

	if(0 != backend_init(c, err, len))
	{
		ConvertFiberToThread();
		my_free(internal);
		return -1;
	}

	return 0;
}

DLL_VARIABLE  void ecore_free(ecore_t* core)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;

	assert(DLINK_IsEmpty(&(internal->active_threads)));
	assert(DLINK_IsEmpty(&(internal->delete_threads)));

	backend_cleanup(core);

	my_free(internal);
}

DLL_VARIABLE int ecore_poll(ecore_t* core)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	if(!core->is_running)
	{
		_set_last_error(core, "ϵͳ��ֹͣ.");
		return -1;
	}

	

	return backend_poll(core);
}

DLL_VARIABLE  void ecore_shutdown(ecore_t* core)
{
	core->is_running = false;
}

DLL_VARIABLE  bool _ecore_is_running(ecore_t* core)
{
	return core->is_running;
}



void CALLBACK _ecore_fiber_proc(void* data)
{
	ecore_thread_t* context = (ecore_thread_t*)data;
	ecore_internal_t* internal = (ecore_internal_t*)context->core->internal;

	(*(context->callback_fn))(context->context);

	DLINK_Remove(context);
	DLINK_InsertNext(&(internal->delete_threads), context);


	// ע��������������һ��
	SwitchToFiber(context->back_thread);
}

DLL_VARIABLE int ecore_start_thread(ecore_t* core, void (*callback_fn)(void*), void* context)
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
		my_free(data);
		_set_last_error(core, "�����˳�ʧ�� - %s", _last_win_error());
		return -1;
	}

	DLINK_InsertNext(&(internal->active_threads), data);
	SwitchToFiber(data->self);
}

#ifdef __cplusplus
}
#endif
