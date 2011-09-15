#ifndef _object_h_
#define _object_h_ 1

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mjx_handle_s mjx_handle_t;
typedef struct mjx_context_s mjx_context_t;
typedef struct mjx_event_s {
	// 事件的类型
	int type;
#define MJX_EVENT_CONNECT      1
#define MJX_EVENT_READ         2
#define MJX_EVENT_WRITE        3
#define MJX_EVENT_DISCONNECT   4

	// 事件相关联的句柄
	mjx_handle_t* handle;

	void* context;
} mjx_event_t;

typedef struct mjx_msg_s {
    void *content;
    size_t flags;
    size_t vsm_size;
#define MJX_MAX_VSM_SIZE 30
    unsigned char vsm_data [MJX_MAX_VSM_SIZE];
} mjx_msg_t;


typedef void (mjx_free_fn) (void *data, void *hint);

DLL_VARIABLE int mjx_msg_init (mjx_msg_t *msg);
DLL_VARIABLE int mjx_msg_init_size (mjx_msg_t *msg, size_t size);
DLL_VARIABLE int mjx_msg_init_data (mjx_msg_t *msg, void *data,
    size_t size, mjx_free_fn *ffn, void *hint);
DLL_VARIABLE int mjx_msg_close (mjx_msg_t *msg);
DLL_VARIABLE int mjx_msg_move (mjx_msg_t *dest, mjx_msg_t *src);
DLL_VARIABLE int mjx_msg_copy (mjx_msg_t *dest, mjx_msg_t *src);
DLL_VARIABLE void *mjx_msg_data (mjx_msg_t *msg);
DLL_VARIABLE size_t mjx_msg_size (mjx_msg_t *msg);


DLL_VARIABLE int  mjx_init(mjx_context_t* ctxt);
DLL_VARIABLE void mjx_destroy(mjx_context_t* ctxt);
DLL_VARIABLE int  mjx_submit(mjx_context_t* ctxt, mjx_event_t** events, size_t num);
DLL_VARIABLE int  mjx_getevents(mjx_context_t* ctxt, mjx_event_t** events, size_t num, struct timespec* timeout);



int mjx_io_connect(const char* url, void (*)(mjx_event_t* evt));
int mjx_io_write_msg(mjx_handle_t* handle, mjx_msg_t* msg, void* context);
int mjx_io_read_msg(mjx_handle_t* handle, mjx_msg_t* msg, void* context);
int mjx_io_disconnect(mjx_handle_t* handle, void (*)(mjx_event_t* evt));

int mjx_io_set_option(mjx_handle_t* handler, const void* data, size_t len);
int mjx_io_get_option(mjx_handle_t* handler, void* data, size_t len);





#ifdef __cplusplus
}
#endif


#endif