
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
	char err[ECORE_MAX_ERR_LEN];
	ecore_thread_t* context = (ecore_thread_t*)data;
	ecore_internal_t* internal = (ecore_internal_t*)context->core->internal;

	(*(context->callback_fn))(context->context);

	DLINK_Remove(context);
	DLINK_InsertNext(&(internal->delete_threads), context);

	_ecore_queue_push_task(&context->core->in, &_ecore_fire_thread, context->join_thread, err, ECORE_MAX_ERR_LEN);
	// 注意这个必须是最后一行
	SwitchToFiber(internal->main_thread);
}


DLL_VARIABLE ecore_rc _ecore_start_thread_internal(ecore_t* core, ecore_handle_t* handle, void (*callback_fn)(void*), void* context, const char* name)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	ecore_thread_t* data = (ecore_thread_t*)my_calloc(1, sizeof(ecore_thread_t));

	string_assign(&data->name, name);  
	data->core = core;
	data->callback_fn = callback_fn;
	data->context = context;

	
	//array_needsize (void*, (data->back_threads), (data->back_threadmax), data->back_threadcnt + 1, array_init_zero);

	data->self = CreateFiberEx(0,0,FIBER_FLAG_FLOAT_SWITCH,&_ecore_fiber_proc, data);
	if(NULL == data->self)
	{
		_set_last_error(core, "创建纤程失败 - %s", _last_win_error());
		my_free(data);
		return ECORE_RC_ERROR;
	}

	if(NULL != handle)
		*handle = data;

	DLINK_InsertNext(&(internal->prepare_threads), data);
	return ECORE_RC_OK;
}

struct _thread_wrapper
{
	ecore_t* core;
	void (*callback_fn)(void*);
	void* context;
	string_t name;
};

void _ecore_start_thread_wrapper(struct _thread_wrapper* wrapper)
{
	if(ECORE_RC_OK != _ecore_start_thread_internal(wrapper->core
		, 0
		, wrapper->callback_fn
		, wrapper->context
		, wrapper->name.ptr))
		ecore_log_format(0, ECORE_LOG_FATAL, "创建线程失败 -%s!", wrapper->core->error.ptr);
}

DLL_VARIABLE ecore_rc ecore_start_threadex(ecore_t* core, ecore_handle_t* handle, void (*callback_fn)(void*), void* context, const char* name, size_t len)
{
	char err[ECORE_MAX_ERR_LEN];
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	struct _thread_wrapper* wrapper;
	void* thread = GetCurrentFiber();

	if(thread == internal->main_thread)
		return _ecore_start_thread_internal(core, handle, callback_fn, context, name);

	wrapper = (struct _thread_wrapper*)my_malloc(sizeof(struct _thread_wrapper));
	wrapper->core = core;
	wrapper->callback_fn = callback_fn;
	wrapper->context = context;
	string_init(&wrapper->name);
	string_assignLen(&wrapper->name, name, len);

	return _ecore_queue_push_task(&core->in, &_ecore_start_thread_wrapper, wrapper, err, ECORE_MAX_ERR_LEN);
}

DLL_VARIABLE ecore_rc ecore_start_thread2(ecore_t* core, void (*callback_fn)(void*), void* context, const char* name)
{
	return ecore_start_threadex(core, 0, callback_fn, context, name, strlen(name));
}

DLL_VARIABLE ecore_rc ecore_start_thread(ecore_t* core, void (*callback_fn)(void*), void* context, const string_t* name)
{
	return ecore_start_threadex(core, 0, callback_fn, context, name->ptr, name->len);
}

DLL_VARIABLE void ecore_thread_join(ecore_t* core, ecore_handle_t* handle)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal;
	ecore_thread_t* thread = (ecore_thread_t*)handle;
	assert(core == thread->core);
	thread->join_thread = GetCurrentFiber();
	
	// 注意这个必须是最后一行
	SwitchToFiber(internal->main_thread);
}


#ifdef __cplusplus
}
#endif