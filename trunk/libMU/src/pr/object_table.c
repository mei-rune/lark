#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "object_private.h"
#include "hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif


void _object_hashtable_resize(object_hashtable_t* hash);


static  object_t* _object_keyvalue_create(const char* key, size_t key_len, object_t* value)
{
	object_keyvalue_t* kv = 0;
	object_t* ret = (object_t*)my_calloc(1, sizeof(object_t));
	ret->o_type = object_type_table;
	kv = (object_keyvalue_t*)my_calloc(1, sizeof(object_keyvalue_t));
	
	kv->next = 0;
	kv->key.o_type = object_type_string;
	kv->key.o.c_string.len = key_len;
	kv->key.o.c_string.str = (char*)my_strndup(key, key_len);

	kv->value = value;

	ret->o.c_hash.is_kv = MU_TRUE;
	ret->o.c_hash.c_un.c_kv = kv;
	return ret;
}


static void _object_finialize(object_t *obj)
{
	if(0 == obj)
		return ;
	
    if(0 == object_fn[obj->o_type].finialize)
    {
        string_clear(&obj->_buffer);
    	return ;
    }

    (*(object_fn[obj->o_type].finialize))(obj);
}

static void _object_keyvalue_clear(object_t* value)
{
	object_destroy(value->o.c_hash.c_un.c_kv->value);
	_object_finialize(&(value->o.c_hash.c_un.c_kv->key));
	my_free(value->o.c_hash.c_un.c_kv);
	value->o.c_hash.c_un.c_kv = 0;
}

static void _object_keyvalue_free(object_t* value)
{
	_object_keyvalue_clear(value);
	my_free(value);
}

object_hashtable_t* _object_hashtable_create(size_t size)
{
	object_hashtable_t* ret = (object_hashtable_t*)my_malloc(sizeof(object_hashtable_t));
	ret->capacity = size;
	ret->entries = (object_t**)my_malloc(sizeof(object_t*)*size);
	memset(ret->entries, 0, sizeof(object_t*)*size);
	ret->used = 0;
	ret->array = 0;
	return ret;
}

static mu_boolean _object_hashtable_put(object_hashtable_t* hash, const char* key, size_t key_len, object_t* value, object_t* new_el)
{
	object_t* el = NULL;
	object_t* parent_el = NULL;
	object_t* root_el = NULL;
	
	size_t index = _hash_string(key, key_len) % hash->capacity;
	root_el = hash->entries[index];


	/* 检测是否已存在 */
	el = root_el;
	while (el && _hashtable_str_cmp(GETKEY_STR(el)
		, GETKEY_LEN(el)
		, key, key_len))
	{
		parent_el = el;
		el = GETNEXT(el);
	}
	
	if(el)
	{
		/* 已存在, 则使用旧对象 */

		// 只有在_object_hashtable_resize调用时, new_el 才不为 null
		// 而 _object_hashtable_resize调用时, hash 一定是空的
		assert(0 == new_el);
		
		object_destroy(GETVALUE(el));
		SETVALUE(el, value);
		return MU_FALSE;
	}

	if(0 == new_el)
		new_el = _object_keyvalue_create(key, key_len, value);
	
	SETNEXT(new_el, NULL);
	
	if (parent_el)
		SETNEXT(parent_el, new_el);
	else
		hash->entries[index] = new_el;
	
	hash->used++;
		
		/* 如果使用率大于 HASHTABLE_MAX_USAGE 时性能下将, 需要调整大小 */
	if ((double) hash->used / hash->capacity > HASHTABLE_MAX_USAGE)
		_object_hashtable_resize(hash);
	
	return MU_TRUE;
}



static void _object_hashtable_resize(object_hashtable_t* hash)
{
	
	size_t i;
	size_t old_size  = hash->capacity;
	object_t** old_entries = hash->entries;

	// 重新分配内存
	hash->capacity *= 2;
	hash->entries = (object_t**)my_calloc(hash->capacity, sizeof(object_t*));

	hash->used = 0;
	
	for (i = 0; i < old_size; i++)
	{
		object_t* el = old_entries[i];
		assert(el->o_type == object_type_table && MU_TRUE == el->o.c_hash.is_kv);
		while (el)
		{
			object_t* next = GETNEXT(el);
			string_t* key = &(GETKEY_STRREF(el));
			_object_hashtable_put(hash, key->str, key->len
				, GETVALUE(el), el);
			el = next;
			
			assert(el->o_type == object_type_table && MU_TRUE == el->o.c_hash.is_kv);
		}
	}

	/* free old data */
	my_free(old_entries);
}

static mu_boolean object_hashtable_del(object_hashtable_t* hash, const char* key, size_t key_len)
{
	object_t* el = NULL;
	object_t* parent_el = NULL;
	object_t* root_el = NULL;
	
	size_t index = _hash_string(key, key_len) % hash->capacity;
	root_el = hash->entries[index];


	/* 检测是否已存在 */
	el = root_el;
	while (el && _hashtable_str_cmp(GETKEY_STR(el)
		, GETKEY_LEN(el)
		, key, key_len))
	{
		parent_el = el;
		el = GETNEXT(el);
	}

	if(!el)  /* 不存在, 则返回 */
		return MU_FALSE;

	if (parent_el)
		SETNEXT(parent_el, GETNEXT(el));
	else
		hash->entries[index] = GETNEXT(el);

	
	_object_keyvalue_free(el);

	if(0 == hash->array)
	{
		my_free(hash->array);
		hash->array = 0;
	}

	hash->used--;
	return MU_TRUE;
}

static void object_hashtable_put(object_hashtable_t* hash, const char* key, size_t key_len, object_t *val)
{
	if(0 == val)
	{
		object_hashtable_del(hash, key, key_len);
		return;
	}

	if(_object_hashtable_put(hash, key, key_len, val, NULL))
	{
		// 没有旧值, 说明结构发生变化
		if(0 == hash->array)
		{
			my_free(hash->array);
			hash->array = 0;
		}
	}
}

static object_t* object_hashtable_get(const object_hashtable_t* hash, const char* key, size_t key_len)
{
	object_t* el = hash->entries[_hash_string(key, key_len) % hash->capacity];
	while (el) {
		if (_hashtable_str_cmp(GETKEY_STR(el)
		, GETKEY_LEN(el), key, key_len) == 0)
			return GETVALUE(el);

		el = GETNEXT(el);
	}

	return NULL;
}


static object_t* object_hashtable_elementAt(object_hashtable_t* hash, size_t idx)
{	
	size_t i;
	size_t count;
	if(0 == hash || idx >= hash->used)
		return 0;

	if(0 != hash->array)
		return hash->array[idx];


	if(hash->used > 20)
	{
		hash->array = (object_t**)my_malloc(sizeof(object_t*)*hash->used + 1);
		
		count = 0;
		for (i = 0; i < hash->capacity; i++) 
		{
			object_t* el = hash->entries[i];
			while (el) 
			{
				hash->array[count ++ ] = el;
				el = GETNEXT(el);
			}
		}

		return hash->array[idx];
	}

	count = 0;
	for (i = 0; i < hash->capacity; i++) 
	{
		object_t* el = hash->entries[i];
		while (el) 
		{
			if(idx == count ++)
				return el;

			el = GETNEXT(el);
		}
	}
	return 0;
}

static void object_hashtable_clear(object_hashtable_t* hash)
{
	size_t i;
	if(0 == hash)
		return;


	for (i = 0; i < hash->capacity; i++) 
	{
		object_t* el = hash->entries[i];
		hash->entries[i] = 0;
		while (el) 
		{
			object_t* next = GETNEXT(el);
			_object_keyvalue_free(el);
			el = next;
		}
	}

	if(0 == hash->array)
	{
		my_free(hash->array);
		hash->array = 0;
	}
	hash->used = 0;
}


static void object_hashtable_destroy(object_hashtable_t* hash)
{
	if(0 == hash)
		return;

	object_hashtable_clear(hash);
	my_free(hash);
}


/* table_object */
void _object_table_put(object_t* hash, const char* key, size_t key_len, object_t *val)
{
	if(hash->o.c_hash.is_kv)
		return;
	
	object_hashtable_put(hash->o.c_hash.c_un.c_object, key, key_len, val);
}


mu_boolean _object_table_del(object_t* hash, const char* key, size_t key_len)
{
	if(hash->o.c_hash.is_kv)
		return MU_FALSE;

	return object_hashtable_del(hash->o.c_hash.c_un.c_object, key, key_len);
}

object_t* _object_table_get(const object_t* hash, const char* key, size_t key_len)
{
	if(hash->o.c_hash.is_kv)
	{
		if(0 == strcasecmp("key", key))
			return &(GETKEY(hash));
		else if(0 == strcasecmp("value", key))
			return GETVALUE(hash);
		else
			return 0;
	}
	return object_hashtable_get(hash->o.c_hash.c_un.c_object, key, key_len);
}

object_t* _object_table_elementAt(object_t* hash, size_t idx)
{
	if(hash->o.c_hash.is_kv)
	{
		switch(idx)
		{
		case 0:
			return &(GETKEY(hash));
		case 1:
			return GETVALUE(hash);
		}
		return 0;
	}

	return object_hashtable_elementAt(hash->o.c_hash.c_un.c_object, idx);
}

void _object_table_clear(object_t* hash)
{
	if(hash->o.c_hash.is_kv)
			return ;

	object_hashtable_clear(hash->o.c_hash.c_un.c_object);
}

void _object_table_destroy(object_t* hash)
{
	if(hash->o.c_hash.is_kv)
		_object_keyvalue_clear(hash);
	else
		object_hashtable_destroy(hash->o.c_hash.c_un.c_object);
}

size_t _object_table_length(object_t* hash)
{
	return (hash->o.c_hash.is_kv)?2:hash->o.c_hash.c_un.c_object->used;
}

void _object_table_print(object_t *obj) {
	string_buffer_t buffer = STRING_BUFFER_DEFAULT;
	if(obj->o.c_hash.is_kv) {
		cstring_t key_str = _object_to_print_string(object_element_at(obj, 0));
		cstring_t value_str = _object_to_print_string(object_element_at(obj, 1));
		string_buffer_assignLen(&buffer, string_data(&key_str), string_length(&key_str));
		string_buffer_appendLen(&buffer, ":", 1);
		string_buffer_appendLen(&buffer, string_data(&value_str), string_length(&value_str));
        string_buffer_release(&buffer, &obj->_buffer);	
	} else {
		_object_array_print(obj);
	}
}

#ifdef __cplusplus
}
#endif