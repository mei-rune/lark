
#include "ecore_config.h"
#ifdef _MSC_VER
#include "pthread_windows.h"
#else
#include <pthread.h>
#endif
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif



typedef struct _ecore_executor_internal
{
	pthread_t* pths;
	int threads;
	ecore_queue_t queue;
} ecore_executor_internal_t;


void* _ecore_executor_routine(void* data)
{
	ecore_thread_t self;
	ecore_executor_t* executor = (ecore_executor_t*) data;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)
			executor->internal;
	char err[ECORE_MAX_ERR_LEN + 4] = {0, 0, 0, 0};

	memset(&self, 0, sizeof(self));
	string_assign(&self.name, "executor thread");
	self.self = ConvertThreadToFiberEx(&self,FIBER_FLAG_FLOAT_SWITCH);

	while( executor->is_running )
	{
		ecore_task_t* task = 0;
		ecore_rc rc = _ecore_queue_pop_task(&internal->queue, &task, 1000, err, ECORE_MAX_ERR_LEN);
		if( ECORE_RC_OK != rc )
		{
			if(ECORE_RC_TIMEOUT != rc)
				ecore_log_message(0, ECORE_LOG_ERROR, err);
			continue;
		}

		(*task->fn)(task->data);
	}

	// 将队列中的所有任务完成后，再退出
	while( true )
	{
		ecore_task_t* task = 0;
		ecore_rc rc = _ecore_queue_pop_task(&internal->queue, &task, 1000,  err, ECORE_MAX_ERR_LEN);
		if( ECORE_RC_OK != rc )
			break;

		(*task->fn)(task->data);
	}

	ConvertFiberToThread();

	pthread_exit((void*)0);
	return 0;
}

DLL_VARIABLE ecore_rc ecore_executor_init(ecore_executor_t* executor, char* err, size_t len)
{
	int i;
	ecore_rc ret;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)
		my_malloc(sizeof(ecore_executor_internal_t));
	internal->pths = (pthread_t*)my_malloc(sizeof(pthread_t)*executor->threads);
	internal->threads = 0;

	executor->is_running = 1;
	ret = _ecore_queue_create(&internal->queue, err, len);
	if(ECORE_RC_OK != ret)
		return ret;

	executor->internal = internal;

	for(i = 0; i < executor->threads; ++ i)
	{
		int rc = pthread_create(&(internal->pths[i]), 0, &_ecore_executor_routine, executor);
		if(0 != rc)
		{
			snprintf(err, len, "创建线程失败 - [%d]%s", rc, _last_crt_error_with_code(rc));

			executor->is_running = 0;
			internal->threads = i;
			ecore_executor_finialize(executor);
			return ECORE_RC_ERROR;
		}
	}
	return ECORE_RC_OK;
}

DLL_VARIABLE void ecore_executor_finialize(ecore_executor_t* executor)
{
	int i;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)executor->internal;


	executor->is_running = 0;
	for( i = 0; i < internal->threads; ++ i)
	{
		pthread_join(internal->pths[i], NULL);
	}

	_ecore_queue_finalize(&internal->queue);
	my_free(internal->pths);
	my_free(internal);
	executor->internal = NULL;
}

DLL_VARIABLE ecore_rc ecore_executor_queueJob(ecore_executor_t* executor, void (*fn)(void*), void* data, char* err, size_t len)
{
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)executor->internal;
	if(0 == executor->is_running)
		return ECORE_RC_STOP;

	return _ecore_queue_push_task(&internal->queue, fn, data, err, len);
}


#ifdef __cplusplus
}
#endif
