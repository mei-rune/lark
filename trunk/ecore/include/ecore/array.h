#ifndef _ecore_array_h_
#define _ecore_array_h_ 1

#include "ecore_config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _array array_t;

DLL_VARIABLE array_t* array_new(size_t default_size, void (*free_fn) (void *data));

DLL_VARIABLE void array_free(array_t *al);

DLL_VARIABLE void* array_get(array_t *al, size_t idx);

DLL_VARIABLE void array_set(array_t *al, size_t idx, void *data);

DLL_VARIABLE void array_add(array_t *al, void *data);

DLL_VARIABLE void array_remove(array_t *al, size_t idx);

DLL_VARIABLE size_t array_length(array_t *al);

#ifdef __cplusplus
}
#endif

#endif //_ecore_array_h_
