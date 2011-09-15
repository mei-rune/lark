#ifndef _json_private_h_
#define _json_private_h_ 1

#include "platform.h"

#include <string.h>
#include "object.h"
#include "string_buffer.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define MinusInfinity "-Infinity"

typedef struct JSON_s
{
	int allow_NaN;
	int current_nesting;
} JSON;

#define DEBUG(level, x) //printf(x); printf("\r\n");

const char *_parse_JSON(JSON* json, const char *p, const char *pe, object_t** result);
const char *_parse_JSON_c_string(JSON* json, const char *p, const char *pe, string_buffer_t* result);
const char *_parse_JSON_string(JSON* json, const char *p, const char *pe, object_t** result);
const char *_parse_JSON_number(JSON* json, const char *p, const char *pe, object_t** result);
const char *_parse_JSON_array(JSON* json, const char *p, const char *pe, object_t** result);
const char *_parse_JSON_mapping(JSON* json, const char *p, const char *pe, object_t** result);

#ifdef __cplusplus
}
#endif


#endif //_json_private_h_