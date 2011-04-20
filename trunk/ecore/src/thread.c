
#include "ecore_config.h"
#include <assert.h>
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

	
void _ecore_fire_thread(void* thread)
{
	SwitchToFiber(thread);
}

void CALLBACK _ecore_fiber_proc(void* data)
{
	//char err[ECORE_MAX_ERR_LEN];
	ecore_thread_t* context = (ecore_thread_t*)data;
	ecore_internal_t* internal = (ecore_internal_t*)context->core->internal;

	(*(context->callback_fn))(context->core, context->context);

	ecore_dlink_remove(context);
	ecore_dlink_insert_at_next(&(internal->delete_threads), context);
	//if(NULL != context->join_thread)
	//	_ecore_queue_push_task(&context->core->in, &_ecore_fire_thread, context->join_thread, err, ECORE_MAX_ERR_LEN);
	// 注意这个必须是最后一行
	SwitchToFiber(internal->main_thread);
}


DLL_VARIABLE void _ecore_start_thread_internal(ecore_thread_t* data)
{
	ecore_internal_t* internal = (ecore_internal_t*)data->core->internal;
	ecore_dlink_insert_at_next(&(internal->prepare_threads), data);
}

ecore_rc _ecore_queueTask(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const char* name, size_t name_len, char* err, size_t err_len)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	
	ecore_thread_t* data = (ecore_thread_t*)my_calloc(1, sizeof(ecore_thread_t));
	string_assignLen(&data->name, name, name_len);  
	data->core = core;
	data->callback_fn = callback_fn;
	data->context = context;
	data->self = CreateFiberEx(0,0,FIBER_FLAG_FLOAT_SWITCH,&_ecore_fiber_proc, data);
	if(NULL == data->self)
	{
		snprintf(err, err_len, "创建纤程失败 - %s", _last_win_error());
		return ECORE_RC_ERROR;
	}

	if(GetCurrentFiber() == internal->main_thread)
	{
		_ecore_start_thread_internal(data);
		return ECORE_RC_OK;
	}

	return _ecore_queue_push_task(&core->in, &_ecore_start_thread_internal, data, err, err_len);
}

DLL_VARIABLE ecore_rc ecore_queueTask_c(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const char* name, char* err, size_t err_len)
{
	return _ecore_queueTask(core, callback_fn, context, name, strlen(name), err, err_len);
}

DLL_VARIABLE ecore_rc ecore_queueTask(ecore_t* core, void (*callback_fn)(ecore_t*, void*), void* context, const string_t* name, char* err, size_t err_len)
{
	return _ecore_queueTask(core, callback_fn, context, name->ptr, name->len, err, err_len);
}

//DLL_VARIABLE void ecore_thread_join(ecore_t* core, ecore_handle_t* handle)
//{
//	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
//	ecore_thread_t* thread = (ecore_thread_t*)handle;
//	assert(core == thread->core);
//	thread->join_thread = GetCurrentFiber();
//	
//	// 注意这个必须是最后一行
//	SwitchToFiber(internal->main_thread);
//}


#ifdef __cplusplus
}
#endif