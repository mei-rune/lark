#ifndef _ecore_h_
#define _ecore_h_ 1

#include "ecore_config.h"

#include "ecore/string.h"
#ifdef _MSC_VER
#include "win32/stdbool.h"
#else
#include <stdbool.h>
#endif
#include <WinSock2.h>
#include <Mswsock.h>

#ifdef __cplusplus
extern "C" {
#endif

#if _WIN32
#define snprintf _snprintf
#define vscprintf _vscprintf
#define strncasecmp  strnicmp
#define strcasecmp  stricmp
#endif

#ifdef _WIN32
 typedef SOCKET socket_type;
 typedef WSABUF iovec;
#else
 typedef int socket_type;
#endif

#define ECORE_MAX_ERR_LEN 2048

 typedef int ecore_rc;

#define  ECORE_RC_TIMEOUT     1
#define  ECORE_RC_OK          0
#define  ECORE_RC_ERROR      -1
#define  ECORE_RC_AGAIN      -2
#define  ECORE_RC_BUSY       -3
#define  ECORE_RC_DONE       -4
#define  ECORE_RC_DECLINED   -5
#define  ECORE_RC_ABORT      -6



 typedef struct _ecore_executor
 {
	// 是否正在运行中 , 1 为运行中， 0 为停止
	int is_running;
	// 线程数
 	int threads;
 	void* internal;
 } ecore_executor_t;

 typedef struct _ecore{
	// 是否正在运行中 , 1 为运行中， 0 为停止
	int is_running;
	// 最后一次的错误
	string_t error;

	// 线程池
	ecore_executor_t* executor;

	void* internal;
 }  ecore_t;

DLL_VARIABLE ecore_rc ecore_init(ecore_t* core, char* err_buf, int len);
DLL_VARIABLE void ecore_finalize(ecore_t* core);
DLL_VARIABLE ecore_rc ecore_poll(ecore_t* core, int milli_seconds);
DLL_VARIABLE void ecore_shutdown(ecore_t* core);
DLL_VARIABLE ecore_rc ecore_at_exit(ecore_t* io, void (*cleanup_fn)(void*), void* context);


DLL_VARIABLE ecore_rc ecore_start_thread(ecore_t* core, void (*callback_fn)(void*), void* context);


typedef struct _ecore_io{
	ecore_t* core;
	// io 对象名字
	string_t name;
	// io 对象的本地地址的 url, 如 tcp://xxx.xxx.xxx.xxx:xxx
	string_t local_address;
	// io 对象的远程地址的 url, 如 tcp://xxx.xxx.xxx.xxx:xxx
	string_t remote_address;
	void* internal;
 } ecore_io_t;

DLL_VARIABLE ecore_rc ecore_io_listion_at(ecore_t* core, ecore_io_t* io, const string_t* str);
DLL_VARIABLE ecore_rc ecore_io_accept(ecore_io_t* listen_io, ecore_io_t* accepted_io);
DLL_VARIABLE ecore_rc ecore_io_connect(ecore_t* core, ecore_io_t*, const string_t* str);
DLL_VARIABLE void ecore_io_close(ecore_io_t* io);
DLL_VARIABLE size_t ecore_io_write_some(ecore_io_t* io, const void* buf, size_t len);
DLL_VARIABLE ecore_rc ecore_io_write(ecore_io_t* io, const void* buf, size_t len);
DLL_VARIABLE size_t ecore_io_read_some(ecore_io_t* io, void* buf, size_t len);
DLL_VARIABLE ecore_rc ecore_io_read(ecore_io_t* io, void* buf, size_t len);

DLL_VARIABLE ecore_rc ecore_async_warp(ecore_t* core, void (*fn)(void*), void* data);

DLL_VARIABLE ecore_rc ecore_executor_init(ecore_executor_t* executor, char* err, size_t len);
DLL_VARIABLE ecore_rc ecore_executor_queueTask(ecore_executor_t* executor, void (*fn)(void*), void* data);
DLL_VARIABLE ecore_rc ecore_executor_finalize(ecore_executor_t* executor);

#ifdef __cplusplus
}
#endif

#endif //_ecore_h_
