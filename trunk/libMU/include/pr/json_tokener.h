#ifndef _json_tokener_h_
#define _json_tokener_h_ 1

#include "platform.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_VARIABLE object_t* json_parse(const char* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
