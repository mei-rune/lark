
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "string_buffer.h"

#define CSTRING_ALLOC_GRANULARITY   (8)
#define CSTRING_OFFSET_SIZE         (16)


#define cstring_assert(expr)        assert(expr)

#ifdef __cplusplus
extern "C"
{
#endif

static size_t string_strlen_safe_(char const* s)
{
	return (NULL == s) ? 0 : strlen(s);
}

static void string_ensureLen_(string_buffer_t* pcs, size_t len)
{
	char*       newPtr = NULL;
	size_t  capacity = (0 == len) ? 1u : len;


	cstring_assert(NULL != pcs);

	if (0 != pcs->capacity &&
		pcs->capacity > len)
		return ;


	capacity  = (capacity + (CSTRING_ALLOC_GRANULARITY - 1)) & ~(CSTRING_ALLOC_GRANULARITY - 1);
	if (capacity < pcs->capacity * 2)
		capacity = pcs->capacity * 2;

	if(0 == pcs->str)
		newPtr = (char*)my_malloc(capacity + 1);
	else
		newPtr = (char*)my_realloc(pcs->str, capacity + 1);

	pcs->str = newPtr;
	pcs->str[pcs->len]  =   '\0';
	pcs->capacity = capacity;

	return ;
}

DLL_VARIABLE void string_buffer_destroy(string_buffer_t* pcs)
{
	if(NULL == pcs)
		return;
	my_free(pcs->str);
	pcs->str = 0;
	pcs->len = 0;
	pcs->capacity = 0;
}

DLL_VARIABLE string_buffer_t* string_buffer_assign(string_buffer_t*   pcs
						 ,   char const*         s)
{
	const size_t len = string_strlen_safe_(s);
	string_buffer_assignLen(pcs, s, len);
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_assignLen(string_buffer_t*       pcs
					  ,       char const*             s
					  ,       size_t                  len)
{
	string_ensureLen_(pcs, len);
	strncpy(pcs->str, s, len);
	pcs->len = len;
	pcs->str[pcs->len] = '\0';
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_assignN(string_buffer_t*       pcs
					  , char   ch
					  , size_t n)
{
	string_ensureLen_(pcs, n);
	memset(pcs->str, ch, n);
	pcs->len = n;
	pcs->str[pcs->len] = '\0';
	return pcs;
}


DLL_VARIABLE void string_vsprintf_(string_buffer_t* pcs, size_t begin, const char* fmt, va_list argList)
{
	int len = 0;

	string_ensureLen_(pcs, pcs->len + 200);
	len = vsnprintf(pcs->str + begin, pcs->capacity - begin, fmt, argList);
	if(len > 0)
	{
		pcs->len = begin + len;
		return;
	}

	len = vscprintf(fmt, argList);
	if(len <= 0)
		return ;

	string_ensureLen_(pcs, begin + len + 10);
	len = vsnprintf(pcs->str + begin, pcs->capacity - begin, fmt, argList);
	if(len > 0)
	{
		pcs->len = begin + len;
		return ;
	}
	return;
}

DLL_VARIABLE string_buffer_t* string_buffer_vsprintf(string_buffer_t* pcs, const char*fmt, va_list argList)
{
	string_vsprintf_(pcs, 0, fmt, argList);
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_sprintf(string_buffer_t* pcs, const char*fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	string_vsprintf_(pcs, 0, fmt, argList);
	va_end( argList );
	return pcs;
}


DLL_VARIABLE string_buffer_t* string_buffer_append_vsprintf(string_buffer_t* pcs, const char*fmt, va_list argList)
{
	string_vsprintf_(pcs, pcs->len, fmt, argList);
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_append_sprintf(string_buffer_t* pcs, const char*fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	string_vsprintf_(pcs, pcs->len, fmt, argList);
	va_end( argList );
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_append(string_buffer_t*   pcs
				   ,   char const*         s)
{
	const size_t len = string_strlen_safe_(s);
	string_buffer_appendLen(pcs, s, len);
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_appendLen(string_buffer_t*   pcs
					  ,       char const*         s
					  ,       size_t              len)
{
	size_t newLen = 0;

	cstring_assert(NULL != pcs);
	cstring_assert(NULL != s || 0 == len);

	newLen = pcs->len + len;
	string_ensureLen_(pcs, newLen);

	strncpy(pcs->str + pcs->len, s, len);
	pcs->len = newLen;
	pcs->str[pcs->len] = '\0';
	return pcs;
}

DLL_VARIABLE string_buffer_t* string_buffer_appendN(string_buffer_t*   pcs
					  ,       char                ch
					  ,       size_t              n)
{
	size_t newLen = pcs->len + n;
	string_ensureLen_(pcs, newLen);

	memset(pcs->str + pcs->len, ch, n);
	pcs->len = newLen;
	pcs->str[pcs->len] = '\0';
	return pcs;
}


DLL_VARIABLE void string_buffer_swap(string_buffer_t* lhs, string_buffer_t* rhs)
{
	string_buffer_t tmp;
	memcpy(&tmp, lhs, sizeof(string_buffer_t));
	memcpy(lhs, rhs, sizeof(string_buffer_t));
	memcpy(rhs, &tmp, sizeof(string_buffer_t));
}

DLL_VARIABLE void string_buffer_swap_to(string_buffer_t* lhs, string_t* rhs)
{
	string_t tmp;
	tmp.str = rhs->str;
	tmp.len = rhs->len;

	rhs->str = lhs->str;
	rhs->len = lhs->len;
	
	lhs->str = tmp.str;
	lhs->len = tmp.len;
	lhs->capacity =  tmp.len;
}

DLL_VARIABLE void string_buffer_release(string_buffer_t* lhs, string_t* rhs)
{
	if(0 != rhs->str)
		my_free(rhs->str);

	rhs->str = lhs->str;
	rhs->len = lhs->len;
	
	lhs->str = 0;
	lhs->len = 0;
	lhs->capacity = 0;
}


DLL_VARIABLE void string_swap(string_t* lhs, string_t* rhs)
{
	string_t tmp;
	memcpy(&tmp, lhs, sizeof(string_t));
	memcpy(lhs, rhs, sizeof(string_t));
	memcpy(rhs, &tmp, sizeof(string_t));
}

DLL_VARIABLE void cstring_swap(cstring_t* lhs, cstring_t* rhs)
{
	cstring_t tmp;
	memcpy(&tmp, lhs, sizeof(cstring_t));
	memcpy(lhs, rhs, sizeof(cstring_t));
	memcpy(rhs, &tmp, sizeof(cstring_t));
}


#ifdef __cplusplus
}
#endif
