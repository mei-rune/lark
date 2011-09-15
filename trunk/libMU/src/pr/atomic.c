
#if defined(WIN32) || defined(_WINDOWS)
#include <windows.h>
#endif

#include "os_atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif

DLL_VARIABLE int atomic_add(volatile int *mem , int val)
{
#ifdef _WIN32
	return InterlockedExchangeAdd(mem, val) + val;
	//return InterlockedAdd(mem , val);
#else
	return __sync_add_and_fetch (mem, val);
#endif
}

DLL_VARIABLE int atomic_sub(volatile int *mem , int val)
{
#ifdef _WIN32
	return InterlockedExchangeAdd(mem , - val) - val;
	//return InterlockedAdd(mem , -val);
#else
	return __sync_sub_and_fetch (mem, val);
#endif
}

DLL_VARIABLE int atomic_dec(volatile int *mem)
{
#ifdef _WIN32
	return InterlockedDecrement(mem) ;
#else
	return __sync_sub_and_fetch(mem, 1);
#endif
}

DLL_VARIABLE int atomic_inc(volatile int *mem)
{
#ifdef _WIN32
	return InterlockedIncrement(mem) ;
#else
	return __sync_add_and_fetch(mem, 1);
#endif
}

DLL_VARIABLE int atomic_read(volatile int *mem)
{
	return (*mem) ;
}

DLL_VARIABLE int atomic_set(volatile int *mem , int val)
{
#ifdef _WIN32
	return InterlockedExchange(mem , val) ;
#else
	return __sync_lock_test_and_set(mem, val);
#endif
}

DLL_VARIABLE int atomic_cvs(volatile int *mem , int new_val, int old_val)
{
#ifdef _WIN32
	return InterlockedCompareExchange(mem,1, 0);
#else
	return __sync_val_compare_and_swap(mem,0, 1);
#endif
}

#ifdef __cplusplus
}
#endif