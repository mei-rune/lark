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
#define strncasecmp  strnicmp
#endif

#ifdef _WIN32
 typedef SOCKET socket_type;
 typedef WSABUF iovec;
#else
 typedef int socket_type;
#endif

#define ECORE_MAX_ERR_LEN 2048

 typedef struct _ecore{
	// 是否正在运行中 ...
	bool is_running;
	void* internal;
 }  ecore_t;

DLL_VARIABLE  int ecore_init(ecore_t*, char* err_buf, int len);
DLL_VARIABLE  void ecore_free(ecore_t*);
DLL_VARIABLE  int ecore_poll(ecore_t*);
DLL_VARIABLE  void ecore_shutdown(ecore_t*);
DLL_VARIABLE int ecore_at_exit(ecore_t* io, void (*cleanup_fn)(void*), void* context);

DLL_VARIABLE  bool _ecore_is_running();

DLL_VARIABLE int ecore_start_thread(ecore_t* core, void (*callback_fn)(void*), void* context);


typedef struct _ecore_io{
	ecore_t* core;
	// io 对象的 url, 如 tcp://xxx.xxx.xxx.xxx:xxx
	const string_t* address;
	void* internal;
 } ecore_io_t;

DLL_VARIABLE ecore_io_t* ecore_io_listion_at(const string_t* str);
DLL_VARIABLE ecore_io_t* ecore_io_accept(ecore_io_t*);
DLL_VARIABLE ecore_io_t* ecore_io_connect(const string_t* str);
DLL_VARIABLE void ecore_io_close(ecore_io_t* io);
DLL_VARIABLE int ecore_io_write_some(ecore_io_t* io, const void* buf, int len);
DLL_VARIABLE int ecore_io_write(ecore_io_t* io, const void* buf, int len);
DLL_VARIABLE int ecore_io_read_some(ecore_io_t* io, void* buf, int len);
DLL_VARIABLE int ecore_io_read(ecore_io_t* io, void* buf, int len);






#ifdef __cplusplus
}
#endif

#endif //_ecore_h_
