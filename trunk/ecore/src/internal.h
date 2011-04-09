#ifndef _ecore_internal_h_
#define _ecore_internal_h_ 1

#include "ecore_config.h"
#include "ecore.h"
#include "ecore/link.h"
#include "ports.h"
#include "backend_iocp.h"



#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ecore_thread {
	ecore_t* core;
	void* self;
	void* back_thread;
	void (*callback_fn)(void*);
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



ecore_rc _ecore_task_queue_create(ecore_queue_t* queue, char* err, size_t len);
ecore_rc _ecore_task_queue_pop(ecore_queue_t* queue, ecore_task_t** data, int milli_seconds, char* err, size_t len);
ecore_rc _ecore_task_queue_push(ecore_queue_t* queue, void (*fn)(void*), void* data, char* err, size_t len);
void _ecore_task_queue_finalize(ecore_queue_t* queue);

const char* _last_win_error();
const char* _last_win_error_with_code(unsigned long code);
const char* _last_crt_error();
const char* _last_crt_error_with_code(int code);
void _set_last_error(ecore_t* core, const char* fmt, ... );





#ifdef __cplusplusi
}
#endif

#endif //_ecore_internal_h_
