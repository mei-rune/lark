
#include <string.h>
#include "object_private.h"

#ifdef __cplusplus
extern "C"
{
#endif


char _nil_str[] = "null";
char _nan_str[] = "NaN";
char _minusInfinity[] = "-Infinity";
char _positiveInfinity[] = "Infinity";
char _true_str[] = "true";
char _false_str[] = "false";



object_t  _Nil = {
	object_type_nil,
	{4, _nil_str},
	{0}
};

object_t _NaN = {
	object_type_number,
	{3, _nan_str},
	{{NUMBER_NAN, {0}}}
};

object_t  _MinusInfinity = {
	object_type_number,
	{9, _minusInfinity},
	{{NUMBER_MINUSINFINITY, {0}}}
};

object_t  _PositiveInfinity = {
	object_type_number,
	{8, _positiveInfinity},
	{{NUMBER_POSITIVEINFINITY, {0}}}
};

object_t  _trueObject = {
	object_type_boolean,
	{4, _true_str},
	{MU_TRUE}
};

object_t  _falseObject = {
	object_type_boolean,
	{5, _false_str},
	{MU_FALSE}
};

void _object_default_destroy(object_t *obj)
{
	if(0 == obj)
		return;

	if(object_type_number == obj->o_type) {
		if(NUMBER_NAN == obj->o.c_number.number_type
			|| NUMBER_MINUSINFINITY == obj->o.c_number.number_type
			|| NUMBER_POSITIVEINFINITY == obj->o.c_number.number_type)
			return;
	}

	my_free(obj->_buffer.str);
	my_free(obj);
}


DLL_VARIABLE void object_destroy(object_t *obj)
{
	if(0 == obj)
		return;

    if(0 != object_fn[obj->o_type].finialize)
        (*(object_fn[obj->o_type].finialize))(obj);
	
    if(0 != object_fn[obj->o_type].destroy)
        (*(object_fn[obj->o_type].destroy))(obj);
}



DLL_VARIABLE object_t*  object_new_NaN()
{
	return &_NaN;
}

DLL_VARIABLE object_t*  object_new_Nil()
{
	return &_Nil;
}

DLL_VARIABLE object_t*  object_new_MinusInfinity()
{
	return &_MinusInfinity;
}

DLL_VARIABLE object_t*  object_new_PositiveInfinity()
{
	return &_PositiveInfinity;
}

DLL_VARIABLE object_t* object_new_boolean(mu_boolean b)
{
	return (MU_TRUE == b)?&_trueObject:&_falseObject;
}

DLL_VARIABLE object_t* object_new_uint(uint64 i)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_number;
	obj->o.c_number.number_type = NUMBER_UNSIGNED_INTEGER;
	obj->o.c_number.value.c_uint = i;
	string_init(&obj->_buffer, 0, 0);
	return obj;
}

DLL_VARIABLE object_t* object_new_int(int64 i)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_number;
	obj->o.c_number.number_type = NUMBER_SIGNED_INTEGER;
	obj->o.c_number.value.c_int = i;
	string_init(&obj->_buffer, 0, 0);
	return obj;
}

DLL_VARIABLE object_t* object_new_double(double d)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_number;
	obj->o.c_number.number_type = NUMBER_DOUBLE;
	obj->o.c_number.value.c_double = d;
	string_init(&obj->_buffer, 0, 0);
	return obj;
}

DLL_VARIABLE object_t* object_new_string_zerocopy(char *s, size_t len)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_string;
	obj->o.c_string.str = s;
	obj->o.c_string.len = (-1 == len)?strlen(s):len;
	string_init(&obj->_buffer, 0, 0);
	return obj;
}

DLL_VARIABLE object_t* object_new_zerocopy(string_t str)
{
	return object_new_string_zerocopy(string_data(&str), string_length(&str));
}

DLL_VARIABLE object_t* object_new_stringLen(const char *s, size_t len)
{
	if(0 == s)
	    return object_new_string_zerocopy(s, 0);

	if(-1 == len)
		len = strlen(s);

	return object_new_string_zerocopy(my_strndup(s, len), len);
}

DLL_VARIABLE object_t* object_new_stringN(char c, size_t n)
{
	char* s = (char*)my_malloc(n + 1);
	memset(s, c, n);
	s[n] = 0;
	return object_new_string_zerocopy(s, n);
}

DLL_VARIABLE object_t* object_new_string(const char *s)
{
	return object_new_stringLen(s, strlen(s));
}

DLL_VARIABLE object_t* object_new_table(void)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_table;
	obj->o.c_hash.is_kv = MU_FALSE;
	obj->o.c_hash.c_un.c_object = _object_hashtable_create(JSON_OBJECT_DEF_HASH_ENTRIES);
	string_init(&obj->_buffer, 0, 0);
	return obj;
}

DLL_VARIABLE object_t* object_new_array(void)
{
	object_t *obj = (object_t*)my_calloc(sizeof(object_t), 1);
	obj->o_type = object_type_array;
	ARRAY_INIT(object_t*, &obj->o.c_array, 32);
	string_init(&obj->_buffer, 0, 0);
	return obj;
}
	
#ifdef __cplusplus
}
#endif