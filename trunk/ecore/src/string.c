


#include "ecore_config.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ecore/string.h"
#include "internal.h"

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

static void string_ensureLen_(string_t* pcs, size_t len)
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

	if(0 == pcs->ptr)
		newPtr = (char*)my_malloc(capacity + 1);
	else
		newPtr = (char*)my_realloc(pcs->ptr, capacity + 1);

	pcs->ptr = newPtr;
	pcs->ptr[pcs->len]  =   '\0';
	pcs->capacity = capacity;

	return ;
}




//DLL_VARIABLE const char * string_data(string_t* pcs)
//{
//	assert(NULL != pcs);
//	return pcs->ptr;
//}
//
//DLL_VARIABLE size_t string_len(string_t* pcs)
//{
//	assert(NULL != pcs);
//	return pcs->len;
//}


DLL_VARIABLE void string_finalize(string_t* pcs)
{
	if(NULL == pcs)
		return;
	my_free(pcs->ptr);
	pcs->ptr = 0;
	pcs->len = 0;
	pcs->capacity = 0;
}

DLL_VARIABLE void string_free(string_t* pcs)
{
	if(NULL == pcs)
		return;

	string_finalize(pcs);
	my_free(pcs);
}

//DLL_VARIABLE string_t* string_vsprintf(const char* fmt, va_list argList )
//{
//	string_t* s = string_createLen(fmt, 1024);
//	int len = vsnprintf(s->ptr, s->capacity, fmt, argList);
//	if(len > 0)
//	{
//		s->len = len;
//		return s;
//	}
//
//	len = _vscprintf(fmt, argList);
//	if(len < 0)
//	{
//		string_free(s);
//		return NULL;
//	}
//
//	string_ensureLen_(s, len+1);
//	len = vsnprintf(s->ptr, s->capacity, fmt, argList);
//	if(len > 0)
//	{
//		s->len = len;
//		return s;
//	}
//
//	string_free(s);
//	return NULL;
//}
//
//DLL_VARIABLE string_t* string_sprintf(const char*fmt, ...)
//{
//	string_t* s = NULL;
//	va_list argList;
//	va_start(argList, fmt);
//	s = string_vsprintf(fmt, argList);
//	va_end( argList );
//	return s;
//}
//
//DLL_VARIABLE string_t* string_createLen(char const          *s
//						   , size_t              len)
//{
//	string_t*       pcs = NULL;
//
//	pcs = (string_t*)my_calloc(1, sizeof(string_t));
//	if (NULL == pcs)
//		return NULL;
//
//	pcs->capacity  = (0 == len) ? 1u : len;
//	pcs->capacity  = (pcs->capacity + (CSTRING_ALLOC_GRANULARITY - 1))
//		& ~(CSTRING_ALLOC_GRANULARITY - 1);
//
//	pcs->ptr = (char*)my_malloc(pcs->capacity + 1);
//	if (NULL == pcs->ptr)
//	{
//		my_free(pcs);
//		return NULL;
//	}
//
//	strncpy(pcs->ptr, s, len);
//	pcs->ptr[pcs->len]   =   '\0';
//	pcs->len = len;
//	return pcs;
//}
//
//DLL_VARIABLE string_t* string_create(char const *s)
//{
//	const size_t    len = string_strlen_safe_(s);
//	return string_createLen(s, len);
//}
//
//DLL_VARIABLE string_t* string_createN(char              ch
//						 ,   size_t            n )
//{
//	string_t* pcs = string_createLen("", n);
//		memset(pcs->ptr, ch, sizeof(char) * n);
//	return pcs;
//}


DLL_VARIABLE string_t* string_createLen(string_t* pcs, const char* s
						   , size_t len)
{
	pcs->capacity  = (10 > len) ? 10 : len;
	pcs->capacity  = (pcs->capacity + (CSTRING_ALLOC_GRANULARITY - 1))
		& ~(CSTRING_ALLOC_GRANULARITY - 1);

	pcs->ptr = (char*)my_malloc(pcs->capacity + 4);

	if(0 != s)
		strncpy(pcs->ptr, s, len);
	pcs->ptr[len]   =   '\0';
	pcs->len = len;
	return pcs;
}

DLL_VARIABLE string_t* string_create(string_t* pcs, char const *s)
{
	size_t len = string_strlen_safe_(s);
	return string_createLen(pcs, s, len);
}

DLL_VARIABLE string_t* string_createN(string_t* pcs, char ch
						 , size_t n)
{
	string_createLen(pcs, NULL, n);
	memset(pcs->ptr, ch, sizeof(char) * n);
	return pcs;
}

DLL_VARIABLE void string_vsprintf_(string_t* pcs, size_t begin, const char* fmt, va_list argList)
{
	int len = 0;

	string_ensureLen_(pcs, pcs->len + 200);
	len = vsnprintf(pcs->ptr + begin, pcs->capacity - begin, fmt, argList);
	if(len > 0)
	{
		pcs->len = begin + len;
		return;
	}

	len = vscprintf(fmt, argList);
	if(len <= 0)
		return ;

	string_ensureLen_(pcs, begin + len + 10);
	len = vsnprintf(pcs->ptr + begin, pcs->capacity - begin, fmt, argList);
	if(len > 0)
	{
		pcs->len = begin + len;
		return ;
	}
	return;
}

DLL_VARIABLE string_t* string_assign_vsprintf(string_t* pcs, const char*fmt, va_list argList)
{
	string_vsprintf_(pcs, 0, fmt, argList);
	return pcs;
}

DLL_VARIABLE string_t* string_assign_sprintf(string_t* pcs, const char*fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	string_vsprintf_(pcs, 0, fmt, argList);
	va_end( argList );
	return pcs;
}

DLL_VARIABLE string_t* string_append_vprintf(string_t* pcs, const char*fmt, va_list argList)
{
	string_vsprintf_(pcs, pcs->len, fmt, argList);
	return pcs;
}

DLL_VARIABLE string_t* string_append_printf(string_t* pcs, const char*fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	string_vsprintf_(pcs, pcs->len, fmt, argList);
	va_end( argList );
	return pcs;
}

DLL_VARIABLE string_t* string_assign(string_t*   pcs
						 ,   char const*         s)
{
	const size_t len = string_strlen_safe_(s);
	string_assignLen(pcs, s, len);
	return pcs;
}

DLL_VARIABLE string_t* string_assignLen(string_t*       pcs
					  ,       char const*             s
					  ,       size_t                  len)
{
	string_ensureLen_(pcs, len);
	strncpy(pcs->ptr, s, len);
	pcs->len = len;
	pcs->ptr[pcs->len] = '\0';
	return pcs;
}

DLL_VARIABLE string_t* string_assignN(string_t*       pcs
					  , char   ch
					  , size_t n)
{
	string_ensureLen_(pcs, n);
	memset(pcs->ptr, ch, n);
	pcs->len = n;
	pcs->ptr[pcs->len] = '\0';
	return pcs;
}

//DLL_VARIABLE string_t* string_copy(const string_t*   pcs)
//{
//	cstring_assert(NULL != pcs);
//
//	return string_createLen(pcs->ptr, pcs->len);
//}

DLL_VARIABLE string_t* string_copy(string_t* dst, const string_t* src)
{
	string_createLen(dst, src->ptr, src->len);
	return dst;
}

DLL_VARIABLE string_t* string_append(string_t*   pcs
				   ,   char const*         s)
{
	const size_t len = string_strlen_safe_(s);
	string_appendLen(pcs, s, len);
	return pcs;
}

DLL_VARIABLE string_t* string_appendLen(string_t*   pcs
					  ,       char const*         s
					  ,       size_t              len)
{
	size_t newLen = 0;

	cstring_assert(NULL != pcs);
	cstring_assert(NULL != s || 0 == len);

	newLen = pcs->len + len;
	string_ensureLen_(pcs, newLen);

	strncpy(pcs->ptr + pcs->len, s, len);
	pcs->len = newLen;
	pcs->ptr[pcs->len] = '\0';
	return pcs;
}

DLL_VARIABLE string_t* string_appendN(string_t*   pcs
					  ,       char                ch
					  ,       size_t              n)
{
	size_t newLen = pcs->len + n;
	string_ensureLen_(pcs, newLen);

	memset(pcs->ptr + pcs->len, ch, n);
	pcs->len = newLen;
	pcs->ptr[pcs->len] = '\0';
	return pcs;
}

DLL_VARIABLE string_t* string_truncate(string_t*   pcs
					 , size_t    len)
{
	cstring_assert(NULL != pcs);

	if (len < pcs->len)
		pcs->ptr[pcs->len = len] = '\0';
	return pcs;
}

DLL_VARIABLE void string_swap(string_t* pcs1
				 ,   string_t* pcs2)
{
	string_t  cs;

	cs      =   *pcs1;
	*pcs1   =   *pcs2;
	*pcs2   =   cs;
}

DLL_VARIABLE int string_insert(
					  string_t*       pcs
					  ,   int                     index
					  ,   char const*             s
					  )
{
	const size_t cch = string_strlen_safe_(s);
	return string_insertLen(pcs, index, s, cch);
}

DLL_VARIABLE int string_insertLen(string_t*       pcs
						  ,   int                     index
						  ,   char const*             s
						  ,   size_t                  len
							 )
{
	size_t  realIndex;

	if (0 >= len)
		return 0;

	cstring_assert(NULL != pcs);

	if (index < 0)
	{
		if (-index > (int)pcs->len)
		{
			errno = EINVAL;
			return -1;
		}
		realIndex = pcs->len - (size_t)(-index);
	}
	else
	{
		realIndex = (size_t)index;
		if (realIndex > pcs->len)
		{
			errno = EINVAL;
			return -1;
		}
	}


	if (len > pcs->capacity - pcs->len)
		string_ensureLen_(pcs, pcs->len + len);

	/* copy over the rhs of the std::string */
	memmove(pcs->ptr + realIndex + len, pcs->ptr + realIndex, (pcs->len - realIndex) * sizeof(char));
	pcs->len += len;
	memcpy(pcs->ptr + realIndex, s, sizeof(char) * len);
	pcs->ptr[pcs->len] = '\0';
	return 0;
}

DLL_VARIABLE int string_replace(
						string_t*       pcs
						,   int                     index
						,   size_t                  len
						,   char const*             s
						)
{
	const size_t cch = string_strlen_safe_(s);
	return string_replaceLen(pcs, index, len, s, cch);
}

DLL_VARIABLE int string_replaceLen(
						   string_t*       pcs
						   ,   int                     index
						   ,   size_t                  len
						   ,   char const*             s
						   ,   size_t                  cch
							  )
{
	size_t  realIndex;

	cstring_assert(NULL != pcs);

	if (index < 0)
	{
		if (-index > (int)pcs->len)
		{
			errno = EINVAL;
			return -1;
		}
		realIndex = pcs->len - (size_t)(-index);
	}
	else
	{
		realIndex = (size_t)index;
		if (realIndex + len > pcs->len)
		{
			errno = EINVAL;
			return -1;
		}
	}


	if (cch > len)
		string_ensureLen_(pcs, pcs->len + (cch - len));

	if (len != cch)
	{
		/* copy over the rhs of the std::string */
		const size_t n = pcs->len - (realIndex + len);

		memmove(pcs->ptr + realIndex + cch, pcs->ptr + realIndex + len, n * sizeof(char));

		if (cch > len)
			pcs->len += (cch - len);
		else
			pcs->len -= (len - cch);

		pcs->ptr[pcs->len] = '\0';
	}

	/* we can simply replace directly */
	memcpy(pcs->ptr + realIndex, s, sizeof(char) * cch);

	return 0;
}

DLL_VARIABLE int string_replaceAll(string_t*       pcs
							  ,   char const*             f
							  ,   char const*             t
							  ,   size_t*                 numReplaced /* = NULL */ )
{
	size_t  numReplaced_;

	cstring_assert(NULL != pcs);

	if (NULL == numReplaced)
		numReplaced = &numReplaced_;

	*numReplaced = 0;

	if (NULL == f ||
		'\0' == f[0])
		return 0;

	if (0 == pcs->len)
		return 0;
	{
		/* Search for first, then replace, then repeat from search pos */
		const size_t  flen  =   strlen(f);
		const size_t  tlen  =   string_strlen_safe_(t);
		size_t        pos   =   0;
		char*         p;

		for (; NULL != (p = strstr(pcs->ptr + pos, f)); pos += tlen)
		{
			int rc;

			pos = (size_t)(p - pcs->ptr);

			rc = string_replaceLen(pcs, (int)pos, flen, t, tlen);

			if (0 != rc)
				return rc;
		}
	}
	return 0;
}

#ifdef __cplusplus
}
#endif
