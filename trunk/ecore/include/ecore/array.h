#ifndef _ecore_array_h_
#define _ecore_array_h_ 1

#include "ecore_config.h"

#ifdef __cplusplus
extern "C" {
#endif


//typedef struct _array array_t;

//DLL_VARIABLE array_t* array_new(size_t default_size, void (*free_fn) (void *data));
//
//DLL_VARIABLE void array_free(array_t *al);
//
//DLL_VARIABLE void* array_get(array_t *al, size_t idx);
//
//DLL_VARIABLE void array_set(array_t *al, size_t idx, void *data);
//
//DLL_VARIABLE void array_add(array_t *al, void *data);
//
//DLL_VARIABLE void array_remove(array_t *al, size_t idx);
//
//DLL_VARIABLE size_t array_length(array_t *al);


void * array_realloc (int elem, void *base, int *cur, int cnt);

#define EMPTY


#define array_init_zero(base,count)	\
  memset ((void *)(base), 0, sizeof (*(base)) * (count))

#define array_needsize(type,base,cur,cnt,init)			\
  if (expect_false ((cnt) > (cur)))						\
    {													\
      int ocur_ = (cur);								\
      (base) = (type *)array_realloc					\
         (sizeof (type), (base), &(cur), (cnt));		\
      init ((base) + (ocur_), (cur) - ocur_);			\
    }

#define array_free(stem, idx)		\
  ev_free (stem ## s idx); stem ## cnt idx = stem ## max idx = 0; stem ## s idx = 0


#ifdef __cplusplus
}
#endif

#endif //_ecore_array_h_
