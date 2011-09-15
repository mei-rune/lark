#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object_private.h"

#ifdef __cplusplus
extern "C" {
#endif


void _object_string_destroy(object_t* obj)
{
	ENSURE(obj, object_type_string, EMPTY);
	my_free(obj->o.c_string.str);
	obj->o.c_string.str = 0;
	obj->o.c_string.len = 0;
}

void _object_string_clear(object_t* obj)
{
	ENSURE(obj, object_type_string, EMPTY);
	obj->o.c_string.len = 0;
}

size_t _object_string_length(object_t* obj)
{
	ENSURE(obj, object_type_string, 0);
	return obj->o.c_string.len;
}

void _object_string_print(object_t *obj)
{
	string_buffer_t buffer = STRING_BUFFER_DEFAULT;
	ENSURE(obj, object_type_string, 0);
	string_buffer_assignLen(&buffer, "\"", 1);
	string_buffer_appendLen(&buffer, obj->o.c_string.str, obj->o.c_string.len);
	string_buffer_appendLen(&buffer, "\"", 1);
    string_buffer_release(&buffer, &obj->_buffer);
}

#ifdef __cplusplus
}
#endif