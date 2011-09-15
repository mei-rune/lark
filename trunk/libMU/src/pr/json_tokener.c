#include "json_tokener.h"
#include "json/json_private.h"

#ifdef __cplusplus
extern "C" {
#endif


DLL_VARIABLE object_t* json_parse(const char* data, size_t len)
{
	JSON json;
	object_t* ret;
	if(NULL == _parse_JSON(&json, data, data + len, &ret))
		return NULL;
	return ret;
}

#ifdef __cplusplus
}
#endif