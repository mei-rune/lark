#ifndef _itostr_h
#define _itostr_h 1


#include "platform.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

DLL_VARIABLE size_t ui64tostr_r(uint64 value, char *string, size_t len, int radix);

DLL_VARIABLE size_t i64tostr_r(int64 value, char *string, size_t len, int radix);

DLL_VARIABLE char * i64tostr(int64 value, char *string, int radix);

DLL_VARIABLE char * ui64tostr(uint64 value, char *string, int radix);

DLL_VARIABLE char * itostr(int value, char *string, int radix);

DLL_VARIABLE char * uitostr(unsigned int value, char *string, int radix);

DLL_VARIABLE char * ltostr(long value, char *string, int radix);

DLL_VARIABLE char * ultostr(unsigned long value, char *string, int radix);

#ifdef __cplusplus
}
#endif

#ifndef _WIN32
#define itoa itostr
#define ltoa ltostr
#endif

#endif
