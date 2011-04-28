#ifndef _ecore_internal_h_
#define _ecore_internal_h_ 1

#include "ecore_config.h"
#include "ecore.h"
#include "ecore/link.h"
#include "ports.h"



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

#if __GNUC__ >= 4
# define expect(expr,value)         __builtin_expect ((expr),(value))
# define noinline                   __attribute__ ((noinline))
#else
# define expect(expr,value)         (expr)
# define noinline
# if __STDC_VERSION__ < 199901L && __GNUC__ < 2
#  define inline
# endif
#endif

#define expect_false(expr) expect ((expr) != 0, 0)
#define expect_true(expr)  expect ((expr) != 0, 1)



typedef struct _ecore_thread {
	string_t name;
	ecore_t* core;
	void* self;
	//void* join_thread;
	void (*callback_fn)(ecore_t*, void*);
	void* context;
	struct _ecore_thread* _prev;
	struct _ecore_thread* _next;

} ecore_thread_t;


typedef struct _ecore_cleanup {
	void (*cleanup_fn)(void*);
	void* context;
	struct _ecore_cleanup* _next;
} ecore_cleanup_t;

typedef struct _ecore_internal
{
	void* main_thread;
	ecore_thread_t prepare_threads;
	ecore_thread_t active_threads;
	ecore_thread_t delete_threads;

	ecore_cleanup_t cleanups;

	bool is_running;

	void* backend;

} ecore_internal_t;


typedef struct _ecore_io_internal {
	union {
	HANDLE file;
	socket_type sock;
	} io_handle;
#define io_sock io_handle.sock
#define io_file io_handle.file

	int type;

#define ecore_io_type_tcp_listen      1
#define ecore_io_type_tcp_accepted    2
#define ecore_io_type_tcp_connected   3
#define	ecore_io_type_ipc             4
#define	ecore_io_type_file            5

} ecore_io_internal_t;


void* my_calloc(int _NumOfElements, int _SizeOfElements);
void  my_free(void * _Memory);
void* my_malloc(int _Size);
void* my_realloc(void * _Memory, int _NewSize);


ecore_rc backend_init(ecore_t* core, char* err, size_t len);
ecore_rc  backend_poll(ecore_t* core, int milli_seconds);
void  backend_cleanup(ecore_t* core);



typedef struct _ecore_future
{
	ecore_t* core;
	void* thread;
} ecore_future_t;


void _ecore_future_fire(ecore_future_t* future);
//void _ecore_future_wait(ecore_t* c, ecore_future_t* future);

#define _ecore_future_wait(c, future)									\
	(future)->core = c;													\
	(future)->thread = GetCurrentFiber();								\
	SwitchToFiber(((ecore_internal_t*)(c)->internal)->main_thread)		\



typedef struct _ecore_task
{
	void (*fn)(void* data);
	void* data;

} ecore_task_t;

ecore_rc _ecore_queue_create(ecore_queue_t* queue, char* err, size_t len);
ecore_rc _ecore_queue_pop(ecore_queue_t* queue, void** data, int milli_seconds, char* err, size_t len);
#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX
ecore_rc _ecore_queue_pop_some(ecore_queue_t* queue, void** data, size_t count, size_t* num, int milli_seconds, char* err, size_t len);
#endif
ecore_rc _ecore_queue_push(ecore_queue_t* queue, void* data, char* err, size_t len);
void _ecore_queue_finalize(ecore_queue_t* queue);

ecore_rc _ecore_queue_pop_task(ecore_queue_t* queue, ecore_task_t** data, int milli_seconds, char* err, size_t len);
#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX
ecore_rc _ecore_queue_pop_some_task(ecore_queue_t* queue, ecore_task_t** data, size_t count, size_t* num, int milli_seconds, char* err, size_t len);
#endif
ecore_rc _ecore_queue_push_task(ecore_queue_t* queue, void (*fn)(void*), void* data, char* err, size_t len);

ecore_rc _ecore_queueTask(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const char* name, size_t name_len, char* err, size_t err_len);

const char* _last_win_error();
const char* _last_win_error_with_code(unsigned long code);
const char* _last_crt_error();
const char* _last_crt_error_with_code(int code);
void _set_last_error(ecore_t* core, const char* fmt, ... );


DLL_VARIABLE ecore_rc _ecore_log_init(int level, log_fn_t callback, void* default_context, char* err, size_t len);
//DLL_VARIABLE void _ecore_log_finialize();



struct ecore_io_wrapper
{
	ecore_io_t io;
	struct ecore_io_wrapper* _next;
	struct ecore_io_wrapper* _prev;
};


typedef struct _ecore_application_internal
{
	ecore_thread_t* self_thread;

	ecore_application_t* application;
	ecore_executor_t* executor;

	//ÊÇ·ñÉ¾³ý backend_cores
	int delete_cores;

} ecore_application_internal_t;

#ifdef __cplusplusi
}
#endif

#endif //_ecore_internal_h_
