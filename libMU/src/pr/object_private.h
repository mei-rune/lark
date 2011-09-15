#ifndef _object_private_h_
#define _object_private_h_ 1

#include "platform.h"
#include "internal.h"
#include "array.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif
	
extern const char *object_number_chars;
extern const char *object_hex_chars;

typedef struct object_keyvalue_s object_keyvalue_t;

typedef struct object_hashtable_s {

	/** hash表内块大小 */
	size_t capacity;
	
	/** hash表内当前对象数. */
	size_t used;

	/** hash表内当前对象块指针 */
	object_t** entries;

	/** 用于 toArray() 函数保存临时数据的 */
	object_t** array;

} object_hashtable_t;



typedef struct object_hash_s
{
  mu_boolean is_kv;
  union 
  {
    object_hashtable_t* c_object;
    object_keyvalue_t* c_kv;
  } c_un;
} object_hash_t;


typedef struct object_array_s
{
  ARRAY_DECLARE(object_t*);
} object_array_t;

struct object_s
{
  enum object_type o_type;
  string_t _buffer;

  union data {
	struct {
		int number_type;
#define NUMBER_NAN              0
#define NUMBER_SIGNED_INTEGER   1
#define NUMBER_UNSIGNED_INTEGER 2
#define NUMBER_DOUBLE           3
#define NUMBER_MINUSINFINITY    4
#define NUMBER_POSITIVEINFINITY 5
		union {
			double c_double;
			int64 c_int;
			uint64 c_uint;
		} value;
	} c_number;
	mu_boolean c_boolean;
    string_t c_string;
    object_hash_t c_hash;
    object_array_t c_array;   
  } o;
};

struct object_keyvalue_s
{
  object_t key;
  object_t* value;
  
  object_t* next;
};

#define GETNEXT(el)       el->o.c_hash.c_un.c_kv->next
#define SETNEXT(el, n)    el->o.c_hash.c_un.c_kv->next = n
#define GETKEY(el)        el->o.c_hash.c_un.c_kv->key
#define GETKEY_STRREF(el)        el->o.c_hash.c_un.c_kv->key.o.c_string
#define GETKEY_STR(el)    string_data(&(GETKEY_STRREF(el)))
#define GETKEY_LEN(el)    string_length(&(GETKEY_STRREF(el)))
#define GETVALUE(el)      el->o.c_hash.c_un.c_kv->value
#define SETVALUE(el, val) el->o.c_hash.c_un.c_kv->value = val



#define ENSURE(o, t, r) if(0 == o || o->o_type != t) return r



typedef struct object_type_fn_s
{
  void (*destroy)(object_t *obj);
  void (*finialize)(object_t *obj);
  void (*clear)(object_t *obj);

  mu_boolean (*element_set)(object_t *obj, size_t idx, object_t *val);
  mu_boolean (*element_del)(object_t *obj, size_t idx);
  object_t* (*element_at)(object_t *obj, size_t idx);
  size_t (*length)(object_t *obj);
  void (*print)(object_t *obj);
	
} object_type_fn_t;


extern object_type_fn_t object_fn[];

void _object_default_destroy(object_t *obj);
cstring_t _object_to_print_string(object_t *obj);

object_hashtable_t* _object_hashtable_create(size_t size);
void _object_table_put(object_t* hash, const char* key, size_t key_len, object_t *val);
mu_boolean _object_table_del(object_t* hash, const char* key, size_t key_len);
object_t*  _object_table_get(const object_t* hash, const char* key, size_t key_len);
object_t*  _object_table_elementAt(object_t* hash, size_t idx);
void _object_table_clear(object_t* hash);
void _object_table_destroy(object_t* hash);
size_t _object_table_length(object_t* hash);
void _object_table_print(object_t *obj);

void _object_array_destroy(object_t *obj);
size_t _object_array_length(object_t *obj);
void _object_array_clear(object_t *obj);
mu_boolean _object_array_set(object_t *obj, size_t idx, object_t* val);
mu_boolean _object_array_del(object_t *obj, size_t idx);
object_t* _object_array_at(object_t *obj, size_t idx);
void _object_array_print(object_t *obj);

void _object_string_destroy(object_t* obj);
void _object_string_clear(object_t* obj);
size_t _object_string_length(object_t* obj);
void _object_string_print(object_t *obj);

#ifdef __cplusplus
}
#endif

#endif
