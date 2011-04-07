
#include "ecore_config.h"
#include "pthread.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef ecore_executor_internal
{
	pthread_t* threads;

} ecore_executor_internal_t;


void* queue_run(void* data)
{
	ecore_executor_t* executor = (ecore_executor_t*) data;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)
			executor->internal;

	while( executor->is_running )
	{
		ecore_task_t* task = ecore_queue_take(internal->in, 1000);
		if( 0 == task )
			continue;

		(*task->fn)(task->data);
	}

	// 将队列中的所有任务完成后，再退出
	while( true )
	{
		ecore_task_t* task = ecore_queue_take(internal->in, 1000);
		if( 0 == task )
			break;

		(*task->fn)(task->data);
	}
}

DLL_VARIABLE ecore_rc ecore_executor_init(ecore_executor_t* executor)
{
	return ECORE_RC_ERROR;
}

DLL_VARIABLE ecore_rc ecore_executor_finalize(ecore_executor_t* executor)
{
	return ECORE_RC_ERROR;
}

DLL_VARIABLE ecore_rc ecore_executor_queueTask(ecore_executor_t* executor, void (*fn)(void*), void* data)
{
	return ECORE_RC_ERROR;
}


#ifdef __cplusplus
}
#endif
