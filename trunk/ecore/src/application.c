
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "internal.h"
#include "networking.h"

#ifdef __cplusplus
extern "C" {
#endif


ecore_application_t* g_application = 0;

DLL_VARIABLE void _ecore_application_shutdown(ecore_application_t* application)
{
	size_t i;

	for(i = 0; i < application->backend_core_num; ++ i)
	{
		ecore_shutdown(&application->backend_cores[i]);
	}
}

struct _application_loop_wrapper
{
	ecore_t* core;
	ecore_application_t* application;
};

void _application_loop_internal(struct _application_loop_wrapper* wrapper)
{
	char err[ECORE_MAX_ERR_LEN+1];

	ecore_t* core = wrapper->core;
	ecore_application_t* application = wrapper->application;

	my_free(wrapper);

	if(ECORE_RC_OK != ecore_init(core,err,ECORE_MAX_ERR_LEN))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, err);
		_ecore_application_shutdown(application);
		return;
	}

	if(ECORE_RC_OK != ecore_loop(core, 1000))
		ecore_log_message(0, ECORE_LOG_FATAL, core->error.ptr);

	_ecore_application_shutdown(application);
	ecore_finialize(core);
}

DLL_VARIABLE ecore_rc _ecore_application_init(ecore_application_t* application, char* err, size_t len)
{
	ecore_rc rc;
	size_t i;
	size_t threads;

	ecore_application_internal_t* internal = (ecore_application_internal_t*)my_calloc(1,
												sizeof(ecore_application_internal_t));
	internal->self_thread = (ecore_thread_t*)my_calloc(1, sizeof(ecore_thread_t));
	string_assign(&internal->self_thread->name, "application thread");
	internal->self_thread->self = ConvertThreadToFiberEx(internal->self_thread,FIBER_FLAG_FLOAT_SWITCH);

	application->internal = internal;

	if( NULL == internal->self_thread->self){
		snprintf(err, len, "将当前线程转换到纤程失败 - %s", _last_win_error());
		rc = ECORE_RC_ERROR;
		goto e1;
	}


	if(ECORE_RC_OK != initializeScket()){
		snprintf(err, len, "初始化网络库失败 - %s", _last_win_error());
		rc = ECORE_RC_ERROR;
		goto e2;
	}

	rc = _ecore_log_init(application->log_level
		, application->log_callback, application->log_context, err, len);


	if(ECORE_RC_OK != rc)
		goto e3;

	if(0 == application->backend_core_num)
		application->backend_core_num = 1;

	if(0 == application->backend_cores)
	{
		application->backend_cores = (ecore_t*)my_calloc(application->backend_core_num, sizeof(ecore_t));
		internal->delete_cores = 1;
	}
	else
	{
		internal->delete_cores = 0;
	}

	if(ECORE_RC_OK != ecore_init(&application->backend_cores[0],err,len))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, err);
		goto e4;
	}


	threads = application->backend_threads + application->backend_core_num -1;
	if(0 != threads)
	{
		internal->executor = (ecore_executor_t*)my_calloc(1, sizeof(ecore_executor_t));
		internal->executor->threads = threads;

		rc = ecore_executor_init(internal->executor, err, len);

		if(ECORE_RC_OK != rc)
			goto e5;

		for(i = 1; i < application->backend_core_num; ++ i)
		{
			struct _application_loop_wrapper* wrapper = (struct _application_loop_wrapper*)my_malloc(sizeof(struct _application_loop_wrapper));
			wrapper->application = application;
			wrapper->core = &application->backend_cores[i];
			if(ECORE_RC_OK != ecore_application_queueJob((void (*)(void*))&_application_loop_internal, wrapper, err, len))
			{
				ecore_log_message(0, ECORE_LOG_FATAL, err);
				goto e6;
			}
		}
	}


	return ECORE_RC_OK;
e6:
	for(i = 0; i < application->backend_core_num; ++ i)
	{
		ecore_shutdown(&application->backend_cores[i]);
	}

	ecore_executor_finialize(internal->executor);
e5:
	if(0 != internal->executor)
	{
		my_free(internal->executor);
		internal->executor = 0;
	}
e4:
	if(1 ==	internal->delete_cores)
	{
		my_free(application->backend_cores);
		application->backend_cores = NULL;
		internal->delete_cores = 0;
	}

	_ecore_log_finialize();
e3:
	shutdownSocket();
e2:
	ConvertFiberToThread();
	my_free(internal->self_thread);
	internal->self_thread = NULL;
e1:
	my_free(internal);
	application->internal = 0;
	return rc;
}

DLL_VARIABLE void _ecore_application_finialize(ecore_application_t* application)
{
	ecore_application_internal_t* internal = (ecore_application_internal_t*)application->internal;

	if(0 != internal->executor)
	{
		ecore_executor_finialize(internal->executor);
		my_free(internal->executor);
		internal->executor = 0;
	}


	if(1 ==	internal->delete_cores)
	{
		my_free(application->backend_cores);
		application->backend_cores = NULL;
		internal->delete_cores = 0;
	}

	_ecore_log_finialize();


	my_free(internal->self_thread);
	internal->self_thread = NULL;

	shutdownSocket();

	ConvertFiberToThread();

	my_free(internal);
	application->internal = NULL;
}

DLL_VARIABLE void _ecore_application_loop(ecore_application_t* application)
{
	if(ECORE_RC_OK != ecore_loop(&application->backend_cores[0], 1000))
		ecore_log_message(0, ECORE_LOG_FATAL, application->backend_cores[0].error.ptr);


	_ecore_application_shutdown(application);

	ecore_finialize(&application->backend_cores[0]);

	_ecore_application_finialize(application);
}

DLL_VARIABLE ecore_rc ecore_application_init(ecore_application_t* application, char* err, size_t len)
{
	if(0 != g_application){
		snprintf(err, len, "已经初始化过了");
		return ECORE_RC_ERROR;
	}
	g_application = application;
	return _ecore_application_init(application, err, len);
}


DLL_VARIABLE ecore_rc ecore_application_queueJob(void (*fn)(void*), void* data, char* err, size_t len)
{
	ecore_application_internal_t* internal;

	if(0 == g_application)
	{
		snprintf(err, len, "没有初始化或没有后台线程!");
		return ECORE_RC_ERROR;
	}
	internal = (ecore_application_internal_t*)g_application->internal;
	return ecore_executor_queueJob(internal->executor, fn, data, err, len);
}

struct _ecore_queueTask_wrapper
{
	ecore_t* ecore;
	void (*fn)(ecore_t* ecore, void* context);
	void* context;
};

ecore_rc _ecore_application_queueTask(void (*fn)(ecore_t* ecore, void* context), void* context, const char* name, size_t name_len, char* err, size_t err_len)
{
	if(0 == g_application)
	{
		snprintf(err, err_len, "没有初始化或没有后台线程!");
		return ECORE_RC_ERROR;
	}
	return _ecore_queueTask(&g_application->backend_cores[0], fn, context, name, name_len, err, err_len);
}


DLL_VARIABLE ecore_rc ecore_application_queueTask_c(void (*callback_fn)(ecore_t*, void* context), void* context, const char* name, char* err, size_t err_len)
{
	return _ecore_application_queueTask(callback_fn, context, name, strlen(name), err, err_len);
}

DLL_VARIABLE ecore_rc ecore_application_queueTask(void (*callback_fn)(ecore_t*, void* context), void* context, const string_t* name, char* err, size_t err_len)
{
	return _ecore_application_queueTask(callback_fn, context, string_data(name), string_length(name), err, err_len);
}

DLL_VARIABLE void ecore_application_loop()
{
	_ecore_application_loop(g_application);
}


DLL_VARIABLE void ecore_application_shutdown()
{
	if(0 == g_application)
			return ;

	_ecore_application_shutdown(g_application);
}

#ifdef __cplusplus
}
#endif
