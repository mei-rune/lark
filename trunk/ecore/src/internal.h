#ifndef _ecore_internal_h_
#define _ecore_internal_h_ 1

#include "ecore_config.h"
#include "ecore.h"
#include "ecore/link.h"
#include "ports.h"



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

typedef struct _wait_context
{
	void* thread;
} swap_context_t;

void* my_calloc(int _NumOfElements, int _SizeOfElements);
void  my_free(void * _Memory);
void* my_malloc(int _Size);
void* my_realloc(void * _Memory, int _NewSize);



#define _ecore_fire_event(context)	SwitchToFiber((context)->thread);

#define _ecore_wait(core, context)  (context)->thread = GetCurrentFiber(); SwitchToFiber(((ecore_internal_t*)(core)->internal)->main_thread)

int backend_init(ecore_t* core, char* err, int len);
int  backend_poll(ecore_t* core, int milli_seconds);
void  backend_cleanup(ecore_t* core);


unsigned int _address_to_string(struct sockaddr* name
                     , unsigned int len
                     , const char* schema
                     , unsigned int schema_len
                     , string_t* str);

const char* _last_win_error_with_code(unsigned long code);
const char* _last_win_error();
const char* _last_crt_error();
void _set_last_error(ecore_t* core, const char* fmt, ... );


#ifdef __cplusplusi
}
#endif

#endif //_ecore_internal_h_
