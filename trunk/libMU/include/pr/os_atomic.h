
#ifndef _OS_ATOMIC_H
#define _OS_ATOMIC_H 1

# include "platform.h"


#ifdef __cplusplus
extern "C" {
#endif

DLL_VARIABLE int atomic_add(volatile int *mem , int val) ;

DLL_VARIABLE int atomic_sub(volatile int *mem , int val) ;

DLL_VARIABLE int atomic_dec(volatile int *mem) ;

DLL_VARIABLE int atomic_inc(volatile int *mem) ;

DLL_VARIABLE int atomic_read(volatile int *mem) ;

DLL_VARIABLE int atomic_set(volatile int *mem , int val) ;

DLL_VARIABLE int atomic_cvs(volatile int *mem , int new_val, int old_val);

#ifdef __cplusplus
}
#endif

#endif    /** _OS_ATOMIC_H */
