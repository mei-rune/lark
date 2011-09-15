
#ifndef WIN32_INCL_SYS_H_TIME
#define WIN32_INCL_SYS_H_TIME

#include <stddef.h>
# include <winsock2.h>
# include "platform.h"

#ifdef __cplusplus
extern "C"
{
#endif

DLL_VARIABLE void gettimeofday(struct timeval* tv, void* dummy);

#ifdef __cplusplus
}
#endif 

#endif 

