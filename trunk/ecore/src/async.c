
#include "ecore_config.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

struct _async_warpper
{
	ecore_t* core;
	ecore_future_t future;
	ecore_task_t task;
};

void _async_warp_run(struct _async_warpper* warpper)
{
	char err[ECORE_MAX_ERR_LEN + 4];
	ecore_rc ret = _ecore_queue_push_task(&warpper->core->in
		, (void (*)(void*))&_ecore_future_fire
		, &warpper->future
		, err
		, ECORE_MAX_ERR_LEN);

	if(ECORE_RC_OK != ret)
		ecore_log_message(0, ECORE_LOG_FATAL,err);
}

DLL_VARIABLE ecore_rc ecore_async_warp(ecore_t* core
	, void (*fn)(void*), void* data)
{
	char err[ECORE_MAX_ERR_LEN + 4];
	ecore_rc ret;
	struct _async_warpper warpper;

	warpper.core = core;
	warpper.task.fn = fn;
	warpper.task.data = data;

	ret = ecore_executor_queueJob(core->executor
		,(void (*)(void*)) &_async_warp_run
		, &warpper
		, err
		, ECORE_MAX_ERR_LEN);
	if(ECORE_RC_OK != ret)
	{
		_set_last_error(core, err);
		return ret;
	}

	_ecore_future_wait(core, &warpper.future);
	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
