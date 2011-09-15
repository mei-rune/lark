
#include "platform.h"
#include "object_private.h"

#ifdef __cplusplus
extern "C"
{
#endif

DLL_VARIABLE mu_boolean object_put_int(object_t *obj, const char *key, int64 val)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_int(val));
}

DLL_VARIABLE mu_boolean object_put_uint(object_t *obj, const char *key, uint64 val)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_uint(val));
}

DLL_VARIABLE mu_boolean object_put_double(object_t *obj, const char *key, double val)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_double(val));
}

DLL_VARIABLE mu_boolean object_put_boolean(object_t *obj, const char *key, mu_boolean val)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_boolean(val));
}

DLL_VARIABLE mu_boolean object_put_string(object_t *obj, const char *key, const char* val)
{
	ENSURE(obj, object_type_table, MU_FALSE);
	return object_put_object(obj, key, object_new_string(val));
}

DLL_VARIABLE mu_boolean object_put_stringLen(object_t *obj, const char *key, const char* val, size_t len)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_stringLen(val, len));
}

DLL_VARIABLE mu_boolean object_put_stringN(object_t *obj, const char *key, char val, size_t n)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_stringN(val, n));
}

DLL_VARIABLE mu_boolean object_put_string_zerocopy(object_t *obj, const char *key, char* val, size_t len)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_string_zerocopy(val, len));
}

DLL_VARIABLE mu_boolean object_put_zerocopy(object_t *obj, const char *key, string_t val)
{
	ENSURE(obj, object_type_table, MU_FALSE);

	return object_put_object(obj, key, object_new_zerocopy(val));
}

DLL_VARIABLE mu_boolean object_get_boolean(object_t *obj, const char *key, mu_boolean defaultValue)
{
	return object_to_boolean(object_get_object(obj, key), defaultValue);	
}

DLL_VARIABLE int64      object_get_int(object_t *obj, const char *key, int64 defaultValue)
{
	return object_to_int(object_get_object(obj, key), defaultValue);	
}

DLL_VARIABLE uint64     object_get_uint(object_t *obj, const char *key, uint64 defaultValue)
{
	return object_to_uint(object_get_object(obj, key), defaultValue);	
}

DLL_VARIABLE double     object_get_double(object_t *obj, const char *key, double defaultValue)
{
	return object_to_double(object_get_object(obj, key), defaultValue);	
}

DLL_VARIABLE cstring_t   object_get_string(object_t *obj, const char *key, const char* defaultValue, size_t len)
{
	return object_to_string(object_get_object(obj, key), defaultValue, len);	
}


DLL_VARIABLE mu_boolean object_element_push_boolean(object_t *obj, mu_boolean val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_boolean(val));
}

DLL_VARIABLE mu_boolean object_element_push_int(object_t *obj, int64 val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_int(val));
}

DLL_VARIABLE mu_boolean object_element_push_uint(object_t *obj, uint64 val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_uint(val));
}

DLL_VARIABLE mu_boolean object_element_push_double(object_t *obj, double val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_double(val));
}

DLL_VARIABLE mu_boolean object_element_push_string(object_t *obj, const char* val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_string(val));
}

DLL_VARIABLE mu_boolean object_element_push_stringLen(object_t *obj, const char* val, size_t len)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_stringLen(val, len));
}

DLL_VARIABLE mu_boolean object_element_push_stringN(object_t *obj, char val, size_t n)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_stringN(val, n));
}

DLL_VARIABLE mu_boolean object_element_push_string_zerocopy(object_t *obj, char* val, size_t len)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_string_zerocopy(val, len));
}

DLL_VARIABLE mu_boolean object_element_push_zerocopy(object_t *obj, string_t val)
{
	ENSURE(obj, object_type_array, MU_FALSE);

	return object_element_push(obj, object_new_zerocopy(val));
}

DLL_VARIABLE mu_boolean object_element_pop_boolean(object_t *obj, mu_boolean defaultValue)
{
	object_t* ret;
	mu_boolean val;
	if(!obj)
		return defaultValue;

	ret = object_element_pop(obj);
	if(0 == ret)
		return defaultValue;
	val = object_to_boolean(ret, defaultValue);
	object_destroy(ret);
	return val;
}

DLL_VARIABLE int64      object_element_pop_int(object_t *obj, int64 defaultValue)
{
	object_t* ret;
	int64 val;
	if(!obj)
		return defaultValue;

	ret = object_element_pop(obj);
	if(0 == ret)
		return defaultValue;
	val = object_to_int(ret, defaultValue);
	object_destroy(ret);
	return val;
}

DLL_VARIABLE uint64     object_element_pop_uint(object_t *obj, uint64 defaultValue)
{
	object_t* ret;
	uint64 val;
	if(!obj)
		return defaultValue;

	ret = object_element_pop(obj);
	if(0 == ret)
		return defaultValue;
	val = object_to_uint(ret, defaultValue);
	object_destroy(ret);
	return val;
}

DLL_VARIABLE double     object_element_pop_double(object_t *obj, double defaultValue)
{
	object_t* ret;
	double val;
	if(!obj)
		return defaultValue;

	ret = object_element_pop(obj);
	if(0 == ret)
		return defaultValue;
	val = object_to_double(ret, defaultValue);
	object_destroy(ret);
	return val;
}

DLL_VARIABLE string_t   object_element_pop_string(object_t *obj)
{
	object_t* ret;
	string_t val = STRING_DEFAULT;
	if(!obj)
		return val;

	ret = object_element_pop(obj);
	if(0 == ret)
		return val;

	
	object_to_string(ret, 0, 0);
	string_swap(&obj->_buffer, &val);

	object_destroy(ret);
	return val;
}


DLL_VARIABLE mu_boolean object_element_set_boolean(object_t *obj, size_t idx, mu_boolean value)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_boolean(value));
}
DLL_VARIABLE mu_boolean  object_element_set_int(object_t *obj, size_t idx, int64 value)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_int(value));
}
DLL_VARIABLE mu_boolean  object_element_set_uint(object_t *obj, size_t idx, uint64 value)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_uint(value));
}
DLL_VARIABLE mu_boolean  object_element_set_double(object_t *obj, size_t idx, double value)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_double(value));
}
DLL_VARIABLE mu_boolean  object_element_set_string(object_t *obj, size_t idx, const char* str)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_string(str));
}
DLL_VARIABLE mu_boolean  object_element_set_stringLen(object_t *obj, size_t idx, const char* str, size_t len)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_stringLen(str, len));
}
DLL_VARIABLE mu_boolean  object_element_set_stringN(object_t *obj, size_t idx, char c, size_t n)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_stringN(c, n));
}
DLL_VARIABLE mu_boolean  object_element_set_string_zerocopy(object_t *obj, size_t idx, char* str, size_t len)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_string_zerocopy(str, len));
}
DLL_VARIABLE mu_boolean  object_element_set_zerocopy(object_t *obj, size_t idx, string_t str)
{
	ENSURE(obj, object_type_array, MU_FALSE);
	return object_element_set(obj, idx, object_new_zerocopy(str));
}

DLL_VARIABLE int64      object_element_at_int(object_t *obj, size_t idx, int64 defaultValue)
{
	return object_to_int(object_element_at(obj, idx), defaultValue);
}
DLL_VARIABLE uint64     object_element_at_uint(object_t *obj, size_t idx, uint64 defaultValue)
{
	return object_to_uint(object_element_at(obj, idx), defaultValue);
}
DLL_VARIABLE double     object_element_at_double(object_t *obj, size_t idx, double defaultValue)
{
	return object_to_double(object_element_at(obj, idx), defaultValue);
}
DLL_VARIABLE mu_boolean object_element_at_boolean(object_t *obj, size_t idx, mu_boolean defaultValue)
{
	return object_to_boolean(object_element_at(obj, idx), defaultValue);
}
DLL_VARIABLE cstring_t  object_element_at_string(object_t *obj, size_t idx, const char* defaultStr, size_t len)
{
	return object_to_string(object_element_at(obj, idx), defaultStr, len);
}

#ifdef __cplusplus
}
#endif

