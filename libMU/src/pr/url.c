
#include "url.h"
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C"
{
#endif

#if !HAVE_STRNDUP
char* my_strndup(const char* str, size_t n);
#endif /* !HAVE_STRNDUP */
	
#define STATUS_ADDRESS			0
#define STATUS_PORT				1
#define STATUS_PATH				2
#define STATUS_QUERY_STRING		3

DLL_VARIABLE void url_parse(url_t* url, const char* str)
{
	size_t pos;
	size_t prev;
	int status = STATUS_ADDRESS;
	const char* ptr = strstr(str, "://");

	memset(url, 0, sizeof(url_t));
	if(0 == ptr)
	{
		url->schema = my_strdup(str);
		return ;
	}

	url->schema = my_strndup(str, ptr-str);
	ptr +=3;
	
#define ON_ADDRESS( s )	if(STATUS_ADDRESS == status)			\
			{													\
				url->address = my_strndup(&ptr[prev], pos - prev);	\
				prev = pos + 1;									\
				status = s;										\
			}


#define ON_PORT( s )	if(STATUS_ADDRESS == status)			\
			{													\
				url->port = atoi(&ptr[prev]);					\
				prev = pos + 1;									\
				status = s;										\
			}

#define ON_PATH( s )	if(STATUS_PATH == status)				\
			{													\
				url->path = my_strndup(&ptr[prev], pos - prev);	\
				prev = pos + 1;									\
				status = s;										\
			}

	
	pos = -1;
	prev = 0;
	do
	{
		switch(ptr[++ pos])
		{
		case ':':
			ON_ADDRESS(STATUS_PORT);
			break;
		case ';':
			ON_ADDRESS( STATUS_QUERY_STRING )
			else ON_PORT( STATUS_QUERY_STRING )
			break;
		case '/':
			ON_ADDRESS( STATUS_PATH )
			else ON_PORT( STATUS_PATH )
			break;
		case '?':
			ON_ADDRESS( STATUS_QUERY_STRING )
			else ON_PORT( STATUS_QUERY_STRING )
			else ON_PATH( STATUS_QUERY_STRING )
			break;
		case 0:
			ON_ADDRESS( STATUS_QUERY_STRING )
			else ON_PORT( STATUS_QUERY_STRING )
			else ON_PATH( STATUS_QUERY_STRING )
			break;
		}
	}while(STATUS_QUERY_STRING != status);

	if(0 != ptr[pos])
		url->query_string = my_strdup(&ptr[pos]);
}
	
DLL_VARIABLE url_t* url_create(const char* str)
{
	url_t* url = (url_t*)my_malloc(sizeof( url_t));
	url_parse(url, str);
	return url;
}

DLL_VARIABLE void url_free(url_t* url)
{
	if(0 == url)
		return;

	if(0 != url->schema)
		my_free(url->schema);

	if(0 != url->address)
		my_free(url->address);

	if(0 != url->path)
		my_free(url->path);

	if(0 != url->query_string)
		my_free(url->query_string);

	my_free(url);
}


#ifdef __cplusplus
}
#endif