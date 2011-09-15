
#include "platform.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "string_buffer.h"


#ifdef __cplusplus
extern "C"
{
#endif

char* my_strndup(const char* str, size_t n)
{
	char* ret = (char*)my_malloc(n*sizeof(char) + 10);
	memcpy(ret, str, n);
	ret[n] = '\0';
	return ret;
}


#ifdef _WIN32

void _last_win_error_with_code(unsigned long code, string_t* err)
{
	DWORD ret = 0;
	char* lpMsgBuf = NULL;


    ret = FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPSTR) & lpMsgBuf,
                    0,
                    NULL);
    if (ret <= 0)
    {
		static char msg[] = "<FormatMessageA ³ö´í>";
		
		if(0 != err->str)
			my_free(err->str);
		err->str = my_strndup(msg, sizeof(msg));
		err->len = sizeof(msg);
		return;
    }

    if ('\r' == lpMsgBuf[ret - 1 ] || '\n' == lpMsgBuf[ ret - 1 ])
	{
        lpMsgBuf[ret - 1 ] = 0;
		ret --;
	}
    if ('\r' == lpMsgBuf[ret - 1 ] || '\n' == lpMsgBuf[ ret - 1 ])
	{
        lpMsgBuf[ret - 1 ] = 0;
		ret --;
	}

	if(0 != err->str)
		my_free(err->str);
	err->str = my_strndup(lpMsgBuf, ret);
	err->len = ret;

	LocalFree(lpMsgBuf);
    return ;
}

#endif

#ifdef __cplusplus
}
#endif

