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

#define ECORE_MAX_ERR_LEN 512

 typedef int ecore_rc;

#define  ECORE_RC_TIMEOUT     1
#define  ECORE_RC_OK          0
#define  ECORE_RC_ERROR      -1
#define  ECORE_RC_STOP       -2
#define  ECORE_RC_SYSTEM     -3


typedef struct _ecore_queue
{
	void* internal;
} ecore_queue_t;

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

	// 队列， 用户不可以初始化它
	ecore_queue_t in;

	// 内部数据， 不可访问
	void* internal;

 }  ecore_t;



DLL_VARIABLE ecore_rc ecore_init(ecore_t* core, char* err, size_t len);
DLL_VARIABLE void ecore_finialize(ecore_t* core);
DLL_VARIABLE ecore_rc ecore_loop(ecore_t* core, int milli_seconds);
DLL_VARIABLE void ecore_shutdown(ecore_t* core);
DLL_VARIABLE void ecore_at_exit(ecore_t* core, void (*cleanup_fn)(void*), void* context);

DLL_VARIABLE ecore_rc ecore_async_warp(ecore_t* core, void (*fn)(void*), void* data);


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

DLL_VARIABLE ecore_rc ecore_io_connect_to_url(ecore_t* core, ecore_io_t*, const char* url);
DLL_VARIABLE ecore_rc ecore_io_connect_to(ecore_t* core, ecore_io_t*, const string_t* url);
DLL_VARIABLE ecore_rc ecore_io_listion_at_url(ecore_t* core, ecore_io_t* io, const char* str);
DLL_VARIABLE ecore_rc ecore_io_listion_at(ecore_t* core, ecore_io_t* io, const string_t* url);
DLL_VARIABLE ecore_rc ecore_io_accept(ecore_io_t* listen_io, ecore_t* core, ecore_io_t* accepted_io);
DLL_VARIABLE void ecore_io_close(ecore_io_t* io);
DLL_VARIABLE size_t ecore_io_write_some(ecore_io_t* io, const void* buf, size_t len);
DLL_VARIABLE ecore_rc ecore_io_write(ecore_io_t* io, const void* buf, size_t len);
DLL_VARIABLE size_t ecore_io_read_some(ecore_io_t* io, void* buf, size_t len);
DLL_VARIABLE ecore_rc ecore_io_read(ecore_io_t* io, void* buf, size_t len);


DLL_VARIABLE ecore_rc ecore_executor_init(ecore_executor_t* executor, char* err, size_t len);
DLL_VARIABLE ecore_rc ecore_executor_queueJob(ecore_executor_t* executor, void (*fn)(void*), void* data, char* err, size_t len);
DLL_VARIABLE void ecore_executor_finialize(ecore_executor_t* executor);


typedef struct _log_message
{
	void* context;
	const char* message;
	size_t length;
} log_message_t;

typedef void (*log_fn_t)(const log_message_t * const * msg, size_t n);

#define ECORE_LOG_SYSTEM   9000
#define ECORE_LOG_FATAL    8000
#define ECORE_LOG_NONE     7000
#define ECORE_LOG_ERROR    6000
#define ECORE_LOG_WARN     5000
#define ECORE_LOG_TRACE    4000
#define ECORE_LOG_ALL      0000

DLL_VARIABLE void ecore_log_message(void* context, int level, const char* message);
DLL_VARIABLE void ecore_log_format (void* context, int level, const char* fmt, ...);
DLL_VARIABLE void ecore_log_vformat(void* context, int level, const char* fmt, va_list argList);


 typedef struct _ecore_application
 {
	 // 日志的级别
	 int log_level;
	 // 日志消息的处理函数
	 log_fn_t log_callback;
	 // 日志的上下文信息
	 void* log_context;

	 // 并发数
	 ecore_t* backend_cores;
	 size_t backend_core_num;

	 // 后台线程数用于执行一些同步函数, @see ecore_async_warp
	 size_t backend_threads;

	 void* internal;
 } ecore_application_t;

DLL_VARIABLE ecore_rc ecore_application_init(ecore_application_t* application, char* err, size_t len);
DLL_VARIABLE ecore_rc ecore_application_queueJob(void (*fn)(void*), void* data, char* err, size_t len);
DLL_VARIABLE ecore_rc ecore_application_queueTask_c(void (*callback_fn)(ecore_t*, void* context), void* context, const char* name, char* err, size_t err_len);
DLL_VARIABLE ecore_rc ecore_application_queueTask(void (*callback_fn)(ecore_t*, void* context), void* context, const string_t* name, char* err, size_t err_len);
DLL_VARIABLE ecore_rc ecore_queueTask_c(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const char* name, char* err, size_t err_len);
DLL_VARIABLE ecore_rc ecore_queueTask(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const string_t* name, char* err, size_t err_len);


DLL_VARIABLE void ecore_application_loop();
DLL_VARIABLE void ecore_application_shutdown();

#ifdef __cplusplus
}
#endif

#endif //_ecore_h_
