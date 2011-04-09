
#include "ecore_config.h"
#include "pthread.h"
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


void* queue_run(void* data)
{
	ecore_executor_t* executor = (ecore_executor_t*) data;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)
			executor->internal;
	char err[ECORE_MAX_ERR_LEN + 4];

	memset(&err, 0, sizeof(string_t));

	while( executor->is_running )
	{
		ecore_task_t* task = 0;
		ecore_rc rc = _ecore_task_queue_pop(&internal->queue, &task, 1000, err, ECORE_MAX_ERR_LEN);
		if( ECORE_RC_OK != rc )
		{
			ecore_log_message(ECORE_LOG_ERROR, err);
			continue;
		}

		(*task->fn)(task->data);
	}

	// 将队列中的所有任务完成后，再退出
	while( true )
	{
		ecore_task_t* task = 0;
		ecore_rc rc = _ecore_task_queue_pop(&internal->queue, &task, 1000,  err, ECORE_MAX_ERR_LEN);
		if( ECORE_RC_OK != rc )
			break;

		(*task->fn)(task->data);
	}
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
	ret = _ecore_task_queue_create(&internal->queue, err, len);
	if(ECORE_RC_OK != ret)
		return ret;

	for(i = 0; i < executor->threads; ++ i)
	{
		int rc = pthread_create(&(internal->pths[i]), 0, queue_run, executor);
		if(0 != rc)
		{
			snprintf(err, len, "创建线程失败 - [%d]%s", rc, _last_crt_error_with_code(rc));
			
			executor->is_running = 0;
			internal->threads = i;
			ecore_executor_finalize(executor);
			return ECORE_RC_ERROR;
		}
	}
	return ECORE_RC_ERROR;
}

DLL_VARIABLE void ecore_executor_finalize(ecore_executor_t* executor)
{
	int i;
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)executor->internal;

	
	executor->is_running = 0;
	for( i = 0; i < internal->threads; ++ i)
	{
		pthread_join(internal->pths[i], NULL);
	}

	_ecore_task_queue_finalize(&internal->queue);
	my_free(internal->pths);
	my_free(internal);
	executor->internal = NULL;
}

DLL_VARIABLE ecore_rc ecore_executor_queueTask(ecore_executor_t* executor, void (*fn)(void*), void* data, char* err, size_t len)
{
	ecore_executor_internal_t* internal = (ecore_executor_internal_t*)executor->internal;
	if(0 == executor->is_running)
		return ECORE_RC_STOP;

	return _ecore_task_queue_push(&internal->queue, fn, data, err, len); 
}


#ifdef __cplusplus
}
#endif
