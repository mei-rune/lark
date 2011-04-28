#ifndef _ecore_pthread_windows_h_
#define _ecore_pthread_windows_h_ 1

#include "ecore_config.h"

#ifdef _MSC_VER

#include <windows.h>
#include <process.h>
#include <intrin.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_CANCEL_ENABLE        0x01  /* Cancel takes place at next cancellation point */
#define PTHREAD_CANCEL_DISABLE       0x00  /* Cancel postponed */
#define PTHREAD_CANCEL_DEFERRED      0x02  /* Cancel waits until cancellation point */

/** 
 * @defgroup thread 线程相关的函数
 *  @{
 */
typedef HANDLE pthread_t;
typedef struct _opaque_pthread_attr_t { long __sig; } pthread_attr_t;
typedef DWORD pthread_key_t;

int pthread_create(pthread_t * thread,
		const pthread_attr_t * attr, 
		void *(*start_routine)(void *), 
		void * arg);


int pthread_join(pthread_t thread, void **value_ptr);

void pthread_exit(void *value_ptr);

int pthread_attr_init(pthread_attr_t *attr);

int pthread_attr_destroy(pthread_attr_t *attr);

int pthread_setcancelstate(int state, int *oldstate);

pthread_t pthread_self(void);

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));

int pthread_key_delete(pthread_key_t key);

int pthread_setspecific(pthread_key_t key, const void *value);

void * pthread_getspecific(pthread_key_t key);

/**
 *  @}
 */

/** 
 * @defgroup mutex Mutex相关的函数
 *  @{
 */
typedef struct _opaque_pthread_mutexattr_t { long __sig; } pthread_mutexattr_t;
typedef struct _opaque_pthread_condattr_t {long __sig; } pthread_condattr_t;
typedef CRITICAL_SECTION pthread_mutex_t;

int pthread_mutex_init(pthread_mutex_t * mutex,
	const pthread_mutexattr_t  * attr);

int pthread_mutex_lock(pthread_mutex_t *mutex);

int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_mutex_destroy(pthread_mutex_t *mutex);

/**
 *  @}
 */

/** 
 * @defgroup CONDITION_VARIABLE Condition variable相关的函数
 *  @{
 */
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)

typedef CONDITION_VARIABLE pthread_cond_t;

#else 

typedef struct
{
  int waiters_count_;
  // Count of the number of waiters.

  CRITICAL_SECTION waiters_count_lock_;
  // Serialize access to <waiters_count_>.

  int release_count_;
  // Number of threads to release via a <pthread_cond_broadcast> or a
  // <pthread_cond_signal>. 
  
  int wait_generation_count_;
  // Keeps track of the current "generation" so that we don't allow
  // one thread to steal all the "releases" from the broadcast.

  HANDLE event_;
  // A manual-reset event that's used to block and release waiting
  // threads. 
} pthread_cond_t;

#endif

int pthread_cond_init(pthread_cond_t * cond, const pthread_condattr_t * attr);

int pthread_cond_signal(pthread_cond_t *cond);

int pthread_cond_broadcast(pthread_cond_t *cond);

int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t  * mutex);

int pthread_cond_destroy(pthread_cond_t *cond);

/**
 *  @}
 */

#ifdef __cplusplus
}
#endif

#endif // _MSC_VER

#endif // _ecore_pthread_windows_h_
