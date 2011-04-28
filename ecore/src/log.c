
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

int g_log_is_running = 0;
int g_log_level = ECORE_LOG_ERROR;
log_fn_t g_log_callback;
void* g_log_context = 0;
ecore_queue_t g_log_queue = { 0 };
pthread_t g_log_thread;

#define LOG_PRIFIX 36

typedef struct _log_message_internal
{
	log_message_t log;
	void*  data;

} log_message_internal_t;

const char* _get_log_level_name(int level)
{
	if(ECORE_LOG_SYSTEM <= level)
		return "SYSTEM";
	if(ECORE_LOG_FATAL <= level)
		return "FATAL";
	if(ECORE_LOG_ERROR <= level)
		return "ERROR";
	if(ECORE_LOG_WARN <= level)
		return "WARN";
	if(ECORE_LOG_TRACE <= level)
		return "TRACE";

	return "TRACE";
}

void*  _log_loop(void* data)
{
	char err[ECORE_MAX_ERR_LEN + 4];

#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX
	log_message_internal_t* internal[100];
	size_t count = 100;
	size_t num;
#else
	log_message_internal_t* internal;
	log_message_t* msg;
#endif
	ecore_rc rc;


	memset(err, 0, sizeof(err));


	while(1 == g_log_is_running)
	{
		err[0] = 0;
#ifdef HAS_GETQUEUEDCOMPLETIONSTATUSEX
		num = 0;
		rc = _ecore_queue_pop_some(&g_log_queue, (void**)internal, count, &num, 1000, err, ECORE_MAX_ERR_LEN);

		if(ECORE_RC_TIMEOUT == rc)
			continue;

		if( ECORE_RC_OK != rc)
		{
			internal[0] = (log_message_internal_t*)my_malloc(sizeof(log_message_internal_t));
			internal[0]->data = 0;
			internal[0]->log.context = g_log_context;
			internal[0]->log.message = err;
			internal[0]->log.length = strlen(err);
			num = 1;
		}

		while(0 != (num--))
		{
			log_message_t* msg = &(internal[num]->log);
			(*g_log_callback)(&msg, 1);

			if(0 != internal[num]->data)
				my_free(internal[num]->data);
			my_free(internal[num]);
			internal[num] = 0;
		}
#else
		rc = _ecore_queue_pop(&g_log_queue, (void**)&internal, 1000, err, ECORE_MAX_ERR_LEN);

		if(ECORE_RC_TIMEOUT == rc)
			continue;

		if( ECORE_RC_OK != rc)
		{
			internal = (log_message_internal_t*)my_malloc(sizeof(log_message_internal_t));
			internal->data = 0;
			internal->log.context = g_log_context;
			internal->log.message = err;
			internal->log.length = strlen(err);
		}

		msg = &(internal->log);
		(*g_log_callback)(&msg, 1);

		if(0 != internal->data)
			my_free(internal->data);
		my_free(internal);
		internal = 0;
#endif

	}
	
	pthread_exit((void*)0);
	return 0;
}


DLL_VARIABLE void _ecore_log_finialize()
{
	g_log_is_running = 0;
	pthread_join(g_log_thread, 0);
	_ecore_queue_finalize(&g_log_queue);
}

DLL_VARIABLE ecore_rc _ecore_log_init(int level, log_fn_t callback, void* default_context, char* err, size_t len)
{
	ecore_rc rc;
	int ret;

	if(0 != g_log_is_running)
	{
		snprintf(err, len, "已经初始化过日志系统了");
		return ECORE_RC_ERROR;
	}

	g_log_is_running = 1;
	g_log_level = level;
	g_log_callback = callback;
	g_log_context = default_context;
	rc = _ecore_queue_create(&g_log_queue, err, len);

	if(ECORE_RC_OK != rc)
		return rc;

	ret = pthread_create(&g_log_thread, 0, &_log_loop, 0);
	if(0 != ret)
	{
		snprintf(err, len, "创建线程失败 - [%d]%s", ret, _last_crt_error_with_code(rc));
		_ecore_queue_finalize(&g_log_queue);
		return ECORE_RC_ERROR;
	}

	atexit(&_ecore_log_finialize);
	return ECORE_RC_OK;
}



DLL_VARIABLE void ecore_log_message(void* ctxt, int level, const char* message)
{
	ecore_log_format(ctxt, level, message);
}


DLL_VARIABLE void ecore_log_vformat(void* ctxt, int level, const char* fmt, va_list argList)
{
	char* ptr;
	int len;

    struct tm   tm;
	struct timeval tv;
	time_t currnet;
	ecore_thread_t* thread;

	if(level < g_log_level)
		return;


	thread = (ecore_thread_t*)GetFiberData();

	gettimeofday(&tv);
	currnet = tv.tv_sec;
#ifndef _WIN32
    localtime_r(&currnet, &tm);
#elif defined(_MSC_VER)
    localtime_s(&tm, &currnet);
#else
     tm = *localtime(&currnet);
#endif

	ptr = (char*)my_malloc(ECORE_MAX_ERR_LEN + LOG_PRIFIX + thread->name.len + 4);
	len = ECORE_MAX_ERR_LEN;
	len = vsnprintf(ptr + LOG_PRIFIX + thread->name.len, len, fmt, argList);
	if(len > 0)
		goto successed;

	len = vscprintf(fmt, argList);
	if(len <= 0)
		goto err;

	my_free(ptr);
	ptr = (char*)my_malloc(len + LOG_PRIFIX + thread->name.len + 4);

	len = vsnprintf(ptr + LOG_PRIFIX + thread->name.len, len+4, fmt, argList);
	if(len <= 0)
		goto err;

successed:
	{
	char err[ECORE_MAX_ERR_LEN];
	log_message_internal_t* log = (log_message_internal_t*)
					my_malloc(sizeof(log_message_internal_t));
	log->data = ptr;
	log->log.message = ptr;
	log->log.length = LOG_PRIFIX + thread->name.len + len + 2;
	log->log.context = (0 == ctxt)?g_log_context:ctxt;


	ptr[LOG_PRIFIX + thread->name.len + len] = '\r';
	ptr[LOG_PRIFIX + thread->name.len + len+1] = '\n';
	ptr[LOG_PRIFIX + thread->name.len + len+2] = 0;

	memset(ptr, ' ', LOG_PRIFIX + thread->name.len);
	len = snprintf(ptr, LOG_PRIFIX + thread->name.len, "%04d%02d%02d %02d:%02d:%02d.%03ld %s [%s] "
	, tm.tm_year + 1900
	, tm.tm_mon + 1
	, tm.tm_mday
    , tm.tm_hour
	, tm.tm_min
	, tm.tm_sec
	, tv.tv_usec / 1000
	, _get_log_level_name(level)
	, thread->name.ptr);

	if(len <= 0)
		goto err;

	ptr[len] = ' ';

	if(ECORE_RC_OK == _ecore_queue_push(&g_log_queue, log, err, ECORE_MAX_ERR_LEN))
		return;

	my_free(log);
	}
err:
	printf( "%04d%02d%02d %02d:%02d:%02d.%03ld %s [%s] "
	, tm.tm_year + 1900
	, tm.tm_mon + 1
	, tm.tm_mday
    , tm.tm_hour
	, tm.tm_min
	, tm.tm_sec
	, tv.tv_usec / 1000
	, _get_log_level_name(level)
	, thread->name.ptr);
	vprintf(fmt, argList);
	printf("\n");

	my_free(ptr);
	return;
}

DLL_VARIABLE void ecore_log_format(void* ctxt, int level, const char* fmt, ...)
{
	va_list argList;

	if(level < g_log_level)
		return;

	va_start(argList, fmt);
	ecore_log_vformat(ctxt, level, fmt, argList);
	va_end( argList );
}

#ifdef __cplusplus
}
#endif
