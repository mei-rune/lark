
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <memory.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "iocp.h"


#ifdef __cplusplus
extern "C" 
{
#endif

ecore_rc backend_init(ecore_t* core, char* err, size_t len)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal; 
	internal->backend = &core->in;
	return _ecore_queue_create(&core->in, err, len);
}

ecore_rc  backend_poll(ecore_t* core, int milli_seconds)
{
	char err[ECORE_MAX_ERR_LEN + 4];
	ecore_task_t* task = 0;
	ecore_rc rc = _ecore_queue_pop_task(&core->in
		, &task
		, milli_seconds
		, err
		, ECORE_MAX_ERR_LEN);
	if(ECORE_RC_OK != rc)
	{
		_set_last_error(core, err);
		return rc;
	}

	(*task->fn)(task->data);
	return ECORE_RC_OK;
}

void  backend_cleanup(ecore_t* core)
{
	ecore_internal_t* internal = (ecore_internal_t*)core->internal; 
	internal->backend = 0;
	_ecore_queue_finalize(&core->in);
}


#ifdef __cplusplus
}
#endif
