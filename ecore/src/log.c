
#include "ecore_config.h"
#include "internal.h"


#ifdef __cplusplus
extern "C" {
#endif

int g_level = ECORE_LOG_ERROR;
log_fn_t g_log_callback;

DLL_VARIABLE void ecore_log_set_level(int level)
{
	g_level = level;
}

DLL_VARIABLE void ecore_log_set_handler(log_fn_t callback)
{
	g_log_callback = callback;
}

DLL_VARIABLE void ecore_log_message(int level, const char* message)
{
	if(level < g_level)
		return;

	printf(message);
}

DLL_VARIABLE void ecore_log_vformat(int level, const char* fmt, va_list argList)
{
	char err[ECORE_MAX_ERR_LEN+4];
	char* ptr = err;
	int len = ECORE_MAX_ERR_LEN;

	if(level < g_level)
		return;


	len = vsnprintf(ptr, len, fmt, argList);
	if(len > 0)
	{
		ecore_log_message(level, ptr);
		return;
	}

	len = vscprintf(fmt, argList);
	if(len <= 0)
		return ;

	ptr = my_malloc(len + 4);
	
	len = vsnprintf(ptr, len+4, fmt, argList);
	if(len > 0)
		ecore_log_message(level, ptr);
	else
		vprintf(fmt, argList);
}

DLL_VARIABLE void ecore_log_format(int level, const char* fmt, ...)
{
	va_list argList;

	if(level < g_level)
		return;

	va_start(argList, fmt);
	ecore_log_vformat(level, fmt, argList);
	va_end( argList );
}

#ifdef __cplusplus
}
#endif
