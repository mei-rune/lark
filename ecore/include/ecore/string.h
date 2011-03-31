

#ifndef _ecore_string_h_
#define _ecore_string_h_ 1

#include "ecore_config.h"

// Include files
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>



#ifdef __cplusplus
extern "C" {
#endif

typedef struct _string_t
{
	size_t          len;        /*!< Number of characters.                                                  */
	char*           ptr;        /*!< Pointer to the std::string. If \link cstring_t::capacity capacity\endlink
								is 0, the value of this member is undetermined.                        */
	size_t          capacity;   /*!< Number of bytes available.                                             */
} string_t;

#define STRING_T_DEFAULT  { 0, NULL, 0, 0 }

#define  string_init(s) (s)->ptr = 0; (s)->len = 0; (s)->capacity = 0

DLL_VARIABLE void string_finalize(string_t* pcs);

#define  string_data(s) (s)->ptr

#define  string_length(s) (s)->len

//DLL_VARIABLE const char * string_data(string_t* pcs);
//
//DLL_VARIABLE size_t string_len(string_t* pcs);

//DLL_VARIABLE string_t* string_create(const char* s);
//
//DLL_VARIABLE string_t* string_createLen(const char* s, size_t len);
//
//DLL_VARIABLE string_t* string_createN(char ch, size_t n);
//
//DLL_VARIABLE string_t* string_sprintf(const char* fmt, ...);
//
//DLL_VARIABLE string_t* string_vsprintf(const char* fmt, va_list argList);
//
//DLL_VARIABLE void string_free(string_t* pcs);

DLL_VARIABLE void string_create(string_t* pcs, const char* s);

DLL_VARIABLE void string_createLen(string_t* pcs, const char* s, size_t len);

DLL_VARIABLE void string_createN(string_t* pcs, char ch, size_t n);

DLL_VARIABLE void string_create_printf(string_t* pcs, const char* fmt, ...);

DLL_VARIABLE void string_create_vprintf(string_t* pcs, const char* fmt, va_list argList);

DLL_VARIABLE void string_append_printf(string_t* pcs, const char* fmt, ...);

DLL_VARIABLE void string_append_vprintf(string_t* pcs, const char* fmt, va_list argList);

DLL_VARIABLE void string_assign(string_t* pcs, const char* s );

DLL_VARIABLE void string_assignLen(string_t* pcs, const char* s, size_t cch);

//DLL_VARIABLE string_t* string_copy(const string_t* pcs);
DLL_VARIABLE void string_copy(string_t* dst, const string_t* src);

DLL_VARIABLE void string_append(string_t* pcs, const char* s);

DLL_VARIABLE void string_appendLen(string_t* pcs, const char* s, size_t cch);

DLL_VARIABLE void string_truncate(string_t* pcs, size_t len);

DLL_VARIABLE void string_swap(string_t* pcs1, string_t* pcs2);

DLL_VARIABLE int string_readline(FILE* stm, string_t* pcs, size_t* numRead /* = NULL */);

DLL_VARIABLE int string_writeline(FILE* stm, const string_t* pcs, size_t* numWritten /* = NULL */);

DLL_VARIABLE int string_insert(string_t* pcs, int index, const char* s);

DLL_VARIABLE int string_insertLen(string_t* pcs, int index, const char* s, size_t cch);

DLL_VARIABLE int string_replace(string_t* pcs, int index, size_t len, const char* s);

DLL_VARIABLE int string_replaceLen(string_t* pcs, int index, size_t len, const char* s, size_t cch);

DLL_VARIABLE int string_replaceAll(string_t* pcs, const char* f, const char* t, size_t* numReplaced /* = NULL */);

#ifdef __cplusplus
};
#endif


#endif // _ecore_string_h_

