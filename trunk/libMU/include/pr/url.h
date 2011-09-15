
#ifndef _url_h
#define _url_h 1


# include "platform.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _url_s
{
	char* schema;
	char* address;
	int port;
	char* path;
	char* query_string;
} url_t;


DLL_VARIABLE url_t* url_create(const char* str);
DLL_VARIABLE void url_free(url_t* url);

//DLL_VARIABLE void url_parse(url_t* url, const char* str);

#ifdef __cplusplus
}
#endif

#endif //_url_h