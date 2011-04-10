#ifndef _ecore_win32_ports_h_
#define _ecore_win32_ports_h_ 1

#include "ecore_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
 #include <time.h>
 int gettimeofday(struct timeval* tv);
#else
 #include <sys/time.h>
#endif

#ifndef HAS_INET_NTOP

const char * inet_ntop(int af, const void *src, char *dst, size_t size);
int inet_pton(int af,  const char *src,  void *dst);

#endif



#ifdef __cplusplus
}
#endif

#endif // _ecore_win32_ports_h_
