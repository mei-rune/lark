
#include "ecore_config.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

DLL_VARIABLE ecore_rc ecore_async_warp(ecore_t* core, void (*fn)(void*), void* data)
{
	ecore_future_t* future = ecore_executor_queueTask(core->executor, fn, data);
	if(NULL == future)
		return ECORE_RC_ERROR;

	_ecore_future_wait(core, future);
	return ECORE_RC_OK;
}

#ifdef __cplusplus
}
#endif
