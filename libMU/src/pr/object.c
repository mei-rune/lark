
#include <string.h>
#include "internal.h"
#include "object_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define REFCOUNT_DEBUG 1 */

const char *object_number_chars = "0123456789.+-eE";
const char *object_hex_chars = "0123456789abcdef";



static void _null_empty(object_t* obj){
}

static mu_boolean _is_primary_true(object_t* obj) {
	return MU_TRUE;
}


static mu_boolean _is_primary_false(object_t* obj) {
	return MU_FALSE;
}


static void _object_number_print(object_t* obj)
{
	ENSURE(obj, object_type_number, EMPTY);

	switch(obj->o.c_number.number_type)  {
	case NUMBER_SIGNED_INTEGER: {
 			string_buffer_t buffer = STRING_BUFFER_DEFAULT;
			string_buffer_sprintf(&buffer, "%I64d", obj->o.c_number.value.c_int);
			string_buffer_release(&buffer, &obj->_buffer);
			break;
		}
	case NUMBER_UNSIGNED_INTEGER: {
 			string_buffer_t buffer = STRING_BUFFER_DEFAULT;
			string_buffer_sprintf(&buffer, "%I64u", obj->o.c_number.value.c_uint);
			string_buffer_release(&buffer, &obj->_buffer);
			break;
		}
	case NUMBER_DOUBLE: {
 			string_buffer_t buffer = STRING_BUFFER_DEFAULT;
			string_buffer_sprintf(&buffer, "%lg", obj->o.c_number.value.c_double);
			string_buffer_release(&buffer, &obj->_buffer);
			break;
		}
	}
}

object_type_fn_t object_fn[] = {
  { //object_type_nil 
    0, //void (*destroy)(object_t *obj);
    0, //void (*finialize)(object_t *obj);
    0, //void (*element_set(object_t *obj, size_t idx, object_t *val);
    0, //mu_boolean (*element_del)(object_t *obj, size_t idx);
    0, //object_t* (*element_at)(object_t *obj, size_t idx);
    0, //size_t (*clear)(object_t *obj);
    0, //size_t (*length)(object_t *obj);
    0, //void (*to_string)(object_t *obj);
  },
  {//object_type_boolean
	0,
    0,
    0,
    0,
    0,
    0,
	0,
    0
  },
  { //object_type_muber
	&_object_default_destroy,
  	0,
    0,
    0,
    0,
    0,
    0, 
    &_object_number_print
  },
  { //object_type_string
  	&_object_default_destroy,
    &_object_string_destroy,
    &_object_string_clear,
    0,
    0,
    0,
    &_object_string_length,
    &_object_string_print
  },
  { //object_type_table
	&_object_default_destroy,
    &_object_table_destroy,
    &_object_table_clear,
    0,
    0,
	&_object_table_elementAt,
    &_object_table_length,
    &_object_table_print
  },
  { //object_type_array
	&_object_default_destroy,
    &_object_array_destroy,
    &_object_array_clear,
    &_object_array_set,
    &_object_array_del,
    &_object_array_at,
    &_object_array_length,
    &_object_array_print
  }
};




DLL_VARIABLE mu_boolean object_is_primary(object_t *obj)
{
	if(0 == obj) return MU_FALSE;

	return (object_type_boolean == obj->o_type 
		 || object_type_number == obj->o_type)?MU_TRUE:MU_FALSE;
}

DLL_VARIABLE mu_boolean object_is_type(object_t *obj, enum object_type type)
{
	if(0 == obj)
		return MU_FALSE;

	return (type == obj->o_type)?MU_TRUE:MU_FALSE;
}

DLL_VARIABLE enum object_type object_get_type(object_t *obj)
{
	if(0 == obj) 
		return object_type_nil;

	return obj->o_type;
}


DLL_VARIABLE mu_boolean  object_isNaN(object_t *obj)
{
	ENSURE(obj, object_type_number, MU_FALSE);
	return NUMBER_NAN == obj->o.c_number.number_type;
}

DLL_VARIABLE mu_boolean  object_isNil(object_t *obj)
{
	if(0 == obj) return MU_TRUE;
	return (object_type_nil == obj->o_type)?MU_TRUE:MU_FALSE;
}

DLL_VARIABLE mu_boolean  object_isMinusInfinity(object_t *obj)
{
	ENSURE(obj, object_type_number, MU_FALSE);
	return NUMBER_MINUSINFINITY == obj->o.c_number.number_type;
}

DLL_VARIABLE mu_boolean  object_isPositiveInfinity(object_t *obj)
{
	ENSURE(obj, object_type_number, MU_FALSE);
	return NUMBER_POSITIVEINFINITY == obj->o.c_number.number_type;
}

DLL_VARIABLE size_t object_length(object_t *obj)
{
	if(0 == obj)
		return 0;

    if(0 == object_fn[obj->o_type].length)
    	return 0;

    return (*(object_fn[obj->o_type].length))(obj);
}

DLL_VARIABLE mu_boolean object_put_object(object_t* obj
							, const char *key
							, object_t *val)
{
	ENSURE(obj, object_type_table, MU_FALSE);
	if(0 == key) return MU_FALSE;
	if(0 == val) return MU_FALSE;

	string_clear(&obj->_buffer);
	_object_table_put(obj, key, strlen(key), val);
	return MU_TRUE;
}

DLL_VARIABLE object_t* object_get_object(object_t* obj, const char *key)
{
	ENSURE(obj, object_type_table, 0);
	if(0 == key) return 0;

	return _object_table_get(obj, key, strlen(key));
}

DLL_VARIABLE mu_boolean object_del_object(object_t* obj, const char *key)
{
	ENSURE(obj, object_type_table, MU_FALSE);
	if(0 == key) return MU_FALSE;

	string_clear(&obj->_buffer);
	return _object_table_del(obj, key, strlen(key));
}

#ifdef __cplusplus
}
#endif
