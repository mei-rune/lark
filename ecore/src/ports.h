#ifndef _ecore_win32_ports_h_
#define _ecore_win32_ports_h_ 1

#include "ecore_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

const char * inet_ntop(int af, const void *src, char *dst, size_t size);

int inet_pton(int af,  const char *src,  void *dst);

#endif

#ifdef __cplusplus
}
#endif

#endif // _ecore_win32_ports_h_
