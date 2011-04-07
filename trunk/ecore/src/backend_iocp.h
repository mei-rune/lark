#ifndef _backend_iocp_h_
#define _backend_iocp_h_ 1

#include "internal.h"
#include "networking.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif


#define _ecore_fire_event(context)	SwitchToFiber((context)->thread);

#define _ecore_future_wait(c, context)  (context)->core = c; (context)->thread = GetCurrentFiber(); SwitchToFiber(((ecore_internal_t*)(c)->internal)->main_thread)



#ifdef __cplusplusi
}
#endif

#endif //_backend_iocp_h_
