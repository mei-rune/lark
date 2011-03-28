#ifndef _ecore_internal_h_
#define _ecore_internal_h_ 1

#include "ecore_config.h"
#include "ecore.h"
#include "ecore/link.h"

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

	char err[ECORE_MAX_ERR_LEN];
} ecore_internal_t;


typedef struct _ecore_io_internal {
	HANDLE file;
} ecore_io_internal_t;

typedef struct _wait_context
{
	int type;
	union 
	{
		void* thread;
		struct 
		{
			void (*complete_fn)(void*);
			void* context;
		} func;
	} data;

} _wait_context_t;


void* my_calloc(int _NumOfElements, int _SizeOfElements);
void  my_free(void * _Memory);
void* my_malloc(int _Size);
void* my_realloc(void * _Memory, int _NewSize);



#define _ecore_switch(to)													\
		if(aio_command_buffer == to->type)									\
			SwitchToFiber(to->data.thread);								\
		else															\
			to->data.func.complete_fn(to->data.func.context)

#define _ecore_wait(core)   SwitchToFiber(((ecore_internal_t*)(core)->internal)->main_thread)

int backend_init(ecore_t* core, char* err, int len);
int  backend_poll(ecore_t* core);
void  backend_cleanup(ecore_t* core);




const char* __last_win_error(unsigned long code);
const char* _last_win_error();
const char* _last_crt_error();
void _set_last_error(ecore_t* core, const char* fmt, ... );


#ifdef __cplusplusi
}
#endif

#endif //_ecore_internal_h_
