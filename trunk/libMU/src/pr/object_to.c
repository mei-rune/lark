
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "object_private.h"

#ifdef __cplusplus
extern "C"
{
#endif

DLL_VARIABLE mu_boolean object_to_boolean(object_t *obj, mu_boolean defaultValue)
{
	if(!obj) 
		return defaultValue;

	switch(obj->o_type)
	{
	case object_type_nil:
		return defaultValue;
	case object_type_boolean:
		return obj->o.c_boolean;
	case object_type_number: {
		switch(obj->o.c_number.number_type){
		case NUMBER_NAN:
			return defaultValue;
		case NUMBER_SIGNED_INTEGER:
			return (0 == obj->o.c_number.value.c_int)?MU_FALSE:MU_TRUE;
		case NUMBER_UNSIGNED_INTEGER:
			return (0 == obj->o.c_number.value.c_uint)?MU_FALSE:MU_TRUE;
		case NUMBER_DOUBLE:
			return (0 == obj->o.c_number.value.c_double)?MU_FALSE:MU_TRUE;
		case NUMBER_MINUSINFINITY:
		case NUMBER_POSITIVEINFINITY:
			return MU_TRUE;
		}
		return defaultValue;
	}
	case object_type_string:
		return (string_length(&obj->o.c_string) != 0);
	default:
		return defaultValue;
	}
}

DLL_VARIABLE int64 object_to_int(object_t *obj, int64 defaultValue)
{
	int cint;

	if(!obj)
		return defaultValue;
	switch(obj->o_type)
	{
	case object_type_number: {
		switch(obj->o.c_number.number_type){
			case NUMBER_SIGNED_INTEGER:
				return obj->o.c_number.value.c_int;
			case NUMBER_UNSIGNED_INTEGER:
				return (_I64_MAX < obj->o.c_number.value.c_uint)
					?defaultValue:obj->o.c_number.value.c_uint;
			case NUMBER_DOUBLE:
#pragma warning( disable : 4244 )
				return (_I64_MAX < obj->o.c_number.value.c_double 
					|| _I64_MIN < obj->o.c_number.value.c_double)?
							defaultValue:obj->o.c_number.value.c_double;
#pragma warning( default : 4244 )
		}
		return defaultValue;
	}
	case object_type_boolean:
		return obj->o.c_boolean;
	case object_type_string:
		if(sscanf(string_data(&obj->o.c_string), "%d", &cint) == 1)
			return cint;
	default:
		return defaultValue;
	}
}

DLL_VARIABLE uint64 object_to_uint(object_t *obj, uint64 defaultValue)
{
	unsigned int cint;

	if(!obj)
		return defaultValue;
	switch(obj->o_type)
	{
	case object_type_boolean:
		return obj->o.c_boolean?1:0;
	case object_type_number: {
		switch(obj->o.c_number.number_type){
			case NUMBER_SIGNED_INTEGER:
				return (0 > obj->o.c_number.value.c_int)
					?defaultValue:obj->o.c_number.value.c_int;
			case NUMBER_UNSIGNED_INTEGER:
				return obj->o.c_number.value.c_uint;
			case NUMBER_DOUBLE:
#pragma warning( disable : 4244 )
				return (_I64_MAX < obj->o.c_number.value.c_double 
					|| _I64_MIN < obj->o.c_number.value.c_double)?
							defaultValue:obj->o.c_number.value.c_double;
#pragma warning( default : 4244 )
		}
		return defaultValue;
	}
	case object_type_string:
		if(sscanf(string_data(&obj->o.c_string), "%u", &cint) == 1)
			return cint;
	default:
		return defaultValue;
	}
}

DLL_VARIABLE double object_to_double(object_t *obj, double defaultValue)
{
	double cdouble;

	if(!obj) 
		return defaultValue;

	switch(obj->o_type) 
	{
	case object_type_boolean:
		return obj->o.c_boolean?1:0;
	case object_type_number: {
		switch(obj->o.c_number.number_type){
			case NUMBER_SIGNED_INTEGER:
#pragma warning( disable : 4244 )
				return obj->o.c_number.value.c_int;
#pragma warning( default : 4244 )
			case NUMBER_UNSIGNED_INTEGER:
#pragma warning( disable : 4244 )
				return obj->o.c_number.value.c_uint;
#pragma warning( default : 4244 )
			case NUMBER_DOUBLE:
				return obj->o.c_number.value.c_double;
		}
		return defaultValue;
	}
	case object_type_string:
		if(sscanf(string_data(&obj->o.c_string), "%lf", &cdouble) == 1)
			return cdouble;
	
	case object_type_nil:
	default:
		return defaultValue;
	}
}	

DLL_VARIABLE cstring_t object_to_string(object_t *obj, const char* defaultValue, size_t len)
{
	cstring_t ret = {(-1 == len)?((0 == defaultValue)?0:strlen(defaultValue)):len
                        , defaultValue};
	if(!obj) 
		return ret;

	if(object_type_string == obj->o_type){
		string_assign(&ret, &obj->o.c_string);
		return ret;
	} else if(object_type_nil == obj->o_type){
		return ret;
	} else if(0 != object_fn[obj->o_type].print){
    	(*(object_fn[obj->o_type].print))(obj);
	}
	string_assign(&ret, &obj->_buffer);
	return ret;
}



cstring_t _object_to_print_string(object_t *obj)
{
	cstring_t ret = STRING_DEFAULT;
	if(!obj) 
		return ret;
	
	if(0 != object_fn[obj->o_type].print){
    	(*(object_fn[obj->o_type].print))(obj);
	}
	string_assign(&ret, &obj->_buffer);
	return ret;
}

#ifdef __cplusplus
}
#endif