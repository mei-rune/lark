

#ifndef _string_buffer_h_
#define _string_buffer_h_ 1

// Include files
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "platform.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct string_s
{
	size_t          len;
	char*           str;
} string_t;

typedef struct cstring_s
{
	size_t          len;
	const char*     str;
} cstring_t;

typedef struct string_buffer_s
{
	size_t          len;       
	char*           str;    
	size_t          capacity; 
} string_buffer_t;

#define STRING_BUFFER_DEFAULT  { 0, NULL, 0 }
#define STRING_DEFAULT  { 0, NULL }

#define  string_init(s, p, l) (s)->str = (p); (s)->len = (l);

#define  string_buffer_init(s) (s)->str = 0; (s)->len = 0; (s)->capacity = 0

DLL_VARIABLE void string_buffer_destroy(string_buffer_t* pcs);

#define  string_data(s) (s)->str

#define  string_length(s) (s)->len

#define  string_clear(s) (s)->len = 0;

#define  string_assign(s, v) (s)->len = (v)->len; (s)->str = (v)->str


DLL_VARIABLE void string_swap(string_t* lhs, string_t* rhs);

DLL_VARIABLE void cstring_swap(cstring_t* lhs, cstring_t* rhs);

DLL_VARIABLE void string_buffer_release(string_buffer_t* lhs, string_t* rhs);

DLL_VARIABLE void string_buffer_swap_to(string_buffer_t* lhs, string_t* rhs);

DLL_VARIABLE void string_buffer_swap(string_buffer_t* lhs, string_buffer_t* rhs);

DLL_VARIABLE string_buffer_t* string_buffer_assign(string_buffer_t* pcs, const char* s);

DLL_VARIABLE string_buffer_t* string_buffer_assignLen(string_buffer_t* pcs, const char* s, size_t cch);

DLL_VARIABLE string_buffer_t* string_buffer_assignN(string_buffer_t* pcs, char ch, size_t n);

DLL_VARIABLE string_buffer_t* string_buffer_vsprintf(string_buffer_t* pcs, const char*fmt, va_list argList);

DLL_VARIABLE string_buffer_t* string_buffer_sprintf(string_buffer_t* pcs, const char*fmt, ...);

DLL_VARIABLE string_buffer_t* string_buffer_append(string_buffer_t*   pcs
				   ,   char const*         s);

DLL_VARIABLE string_buffer_t* string_buffer_appendLen(string_buffer_t*   pcs
					  , char const*         s
					  , size_t              len);

DLL_VARIABLE string_buffer_t* string_buffer_appendN(string_buffer_t*   pcs
					  , char                ch
					  , size_t              n);

DLL_VARIABLE string_buffer_t* string_buffer_append_vsprintf(string_buffer_t* pcs, const char*fmt, va_list argList);

DLL_VARIABLE string_buffer_t* string_buffer_append_sprintf(string_buffer_t* pcs, const char*fmt, ...);

#ifdef __cplusplus
};
#endif


#endif // _string_buffer_h_

