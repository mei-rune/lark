#ifndef _object_h_
#define _object_h_ 1

#include "platform.h"

#include <stddef.h>
#include "string_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_OBJECT_DEF_HASH_ENTRIES 16

typedef struct object_s object_t;

/* 支持的对象类型 */

typedef enum object_type {
  object_type_nil,
  object_type_boolean,
  object_type_number,
  object_type_string,
  object_type_table,
  object_type_array,
} object_type;


/**
 * 检测对象是不是指定的类型
 */
DLL_VARIABLE mu_boolean object_is_type(object_t *obj, enum object_type type);

/**
 * 检测对象是不是原生类型(即非用户自定义类型)
 */
DLL_VARIABLE mu_boolean object_is_primary(object_t *obj);

/**
 * 返回对象的类型
 */
DLL_VARIABLE enum object_type object_get_type(object_t *obj);

/**
 * @defgroup new 对象创建系列函数
 * 
 * 创建一个对象, 对象的类型根据 object_new_* 名的后缀决定
 * 
 * @{
 * 
 * @param[in] b 为对象的布尔值
 * @param[in] i 为对象的整型值
 * @param[in] ui 为对象的无符号整型值
 * @param[in] d 为对象的 double 值
 * @param[in] s 为对象的字符串值
 * @param[in] len 字符串的长度
 * @returns 返回类型为 object_type_string 的对象实例
 */
DLL_VARIABLE object_t* object_new_NaN();
DLL_VARIABLE object_t* object_new_Nil();;
DLL_VARIABLE object_t*  object_new_MinusInfinity();
DLL_VARIABLE object_t*  object_new_PositiveInfinity();
DLL_VARIABLE object_t* object_new_table(void);
DLL_VARIABLE object_t* object_new_array(void);
DLL_VARIABLE object_t* object_new_boolean(mu_boolean b);
DLL_VARIABLE object_t* object_new_int(int64 i);
DLL_VARIABLE object_t* object_new_uint(uint64 ui);
DLL_VARIABLE object_t* object_new_double(double d);
DLL_VARIABLE object_t* object_new_string(const char *s);
DLL_VARIABLE object_t* object_new_stringLen(const char *s, size_t len);
DLL_VARIABLE object_t* object_new_stringN(char c, size_t n);
DLL_VARIABLE object_t* object_new_string_zerocopy(char *s, size_t len);
DLL_VARIABLE object_t* object_new_zerocopy(string_t str);
/**
 * 删除对象
 */
DLL_VARIABLE void object_destroy(object_t *obj);

/** @} */

/**
 * @defgroup to 对象的值转换函数
 * 
 * @{
 */
DLL_VARIABLE mu_boolean object_isNaN(object_t *obj);
DLL_VARIABLE mu_boolean object_isNil(object_t *obj);
DLL_VARIABLE mu_boolean object_isMinusInfinity(object_t *obj);
DLL_VARIABLE mu_boolean object_isPositiveInfinity(object_t *obj);
DLL_VARIABLE mu_boolean object_to_boolean(object_t *obj, mu_boolean defaultValue);
DLL_VARIABLE int64      object_to_int(object_t *obj, int64 defaultValue);
DLL_VARIABLE uint64     object_to_uint(object_t *obj, uint64 defaultValue);
DLL_VARIABLE double     object_to_double(object_t *obj, double defaultValue);
DLL_VARIABLE cstring_t  object_to_string(object_t *obj, const char* defaultValue, size_t len);

/** @} */


/**
 * @defgroup put put 系列函数
 * 
 * 向 map 类型的对象添加一个字段
 * 
 * @{
 *
 * 注意: 这个操作后 val 对象的所有者变为 obj
 *
 * @param[in] obj map 类型对象的实例
 * @param[in] key 新字段的字段名, 注意如果已经有的话会覆盖旧值
 * @param[in] val 新字段的值
 */
DLL_VARIABLE mu_boolean object_put_object(object_t* obj, const char *key, object_t *val);
DLL_VARIABLE mu_boolean object_put_int(object_t *obj, const char *key, int64 val);
DLL_VARIABLE mu_boolean object_put_uint(object_t *obj, const char *key, uint64 val);
DLL_VARIABLE mu_boolean object_put_double(object_t *obj, const char *key, double val);
DLL_VARIABLE mu_boolean object_put_boolean(object_t *obj, const char *key, mu_boolean val);
DLL_VARIABLE mu_boolean object_put_string(object_t *obj, const char *key, const char* val);
DLL_VARIABLE mu_boolean object_put_stringLen(object_t *obj, const char *key, const char* val, size_t len);
DLL_VARIABLE mu_boolean object_put_stringN(object_t *obj, const char *key, char val, size_t n);
DLL_VARIABLE mu_boolean object_put_string_zerocopy(object_t *obj, const char *key, char* val, size_t len);
DLL_VARIABLE mu_boolean object_put_zerocopy(object_t *obj, const char *key, string_t val);

/** @} */

/**
 * @defgroup get get 系列函数
 * 
 * 获取对象实例中指定的字段
 * 
 * @{
 *
 * @param[in] obj 对象的实例
 * @param[in] key 字段名
 * @returns 返回指定字段的值
 */
DLL_VARIABLE object_t*  object_get_object(object_t* obj, const char *key);
DLL_VARIABLE mu_boolean object_get_boolean(object_t *obj, const char *key, mu_boolean defaultValue);
DLL_VARIABLE int64      object_get_int(object_t *obj, const char *key, int64 defaultValue);
DLL_VARIABLE uint64     object_get_uint(object_t *obj, const char *key, uint64 defaultValue);
DLL_VARIABLE double     object_get_double(object_t *obj, const char *key, double defaultValue);
DLL_VARIABLE cstring_t  object_get_string(object_t *obj, const char *key, const char* defaultValue, size_t len);

/** @} */

/** 
 * 删除对象实例中指定的字段
 * @param[in] obj 对象的实例
 * @param[in] key 字段名
 * @returns 删除总是成功的, 返回值为 true 表示找到并删除
 * , 为 false 时表示没有找到这个字段
 */
DLL_VARIABLE mu_boolean object_del_object(object_t* obj, const char *key);


/** 
 * 获取对象内部子对象的个数
 * 
 * 注意, 只有 object_type_object 和 object_type_array 两种类型的对象
 * 有 length 属性,其它类型的对象返回 0
 * 
 * @param[in] obj 对象的实例
 */
DLL_VARIABLE size_t object_length(object_t *obj);

/** 
 * 删除对象内部所有子对象或字段
 * 
 * @param[in] obj 对象的实例
 */
DLL_VARIABLE void object_clear(object_t *obj);

/**
 * @defgroup push push 系列函数
 * 
 * 在类型为 object_type_array 的对象实例的尾部增加一个子对象
 * 
 * @{
 *
 * @param[in] obj 对象的实例
 * @param[in] val 被添加的子对象
 */
DLL_VARIABLE mu_boolean object_element_push(object_t *obj, object_t *val);
DLL_VARIABLE mu_boolean object_element_push_boolean(object_t *obj, mu_boolean val);
DLL_VARIABLE mu_boolean object_element_push_int(object_t *obj, int64 val);
DLL_VARIABLE mu_boolean object_element_push_uint(object_t *obj, uint64 val);
DLL_VARIABLE mu_boolean object_element_push_double(object_t *obj, double val);
DLL_VARIABLE mu_boolean object_element_push_string(object_t *obj, const char* val);
DLL_VARIABLE mu_boolean object_element_push_stringLen(object_t *obj, const char* val, size_t len);
DLL_VARIABLE mu_boolean object_element_push_stringN(object_t *obj, char val, size_t n);
DLL_VARIABLE mu_boolean object_element_push_string_zerocopy(object_t *obj, char* val, size_t len);
DLL_VARIABLE mu_boolean object_element_push_zerocopy(object_t *obj, string_t val);


/** @} */

/** 
 * @defgroup pop pop 系列函数
 * 
 * 从类型为 object_type_array 的对象实例的头部移出一个子对象
 *
 * 注意, 当 pop 出一个对象时, 在你使用完成后, 需要你调用 object_destroy 清理它
 *
 * @{
 *
 * @param[in] obj 对象的实例
 * @param[in] val 被添加的子对象
 */
DLL_VARIABLE object_t*  object_element_pop(object_t *obj);
DLL_VARIABLE mu_boolean object_element_pop_boolean(object_t *obj, mu_boolean defaultValue);
DLL_VARIABLE int64      object_element_pop_int(object_t *obj, int64 defaultValue);
DLL_VARIABLE uint64     object_element_pop_uint(object_t *obj, uint64 defaultValue);
DLL_VARIABLE double     object_element_pop_double(object_t *obj, double defaultValue);
DLL_VARIABLE string_t   object_element_pop_string(object_t *obj);

/** @} */

/** 
 *  @defgroup set set 系列函数
 * 
 * 将类型为 object_type_array 的对象实例的第 idx 个对象更改为指定的子对象
 *
 * 注意如果原位置有子对象时,会自动清理
 *
 * @{
 *
 * @param[in] obj 对象的实例
 * @param[in] idx 位置
 * @param[in] val 被添加的子对象
 */
DLL_VARIABLE mu_boolean  object_element_set(object_t *obj, size_t idx, object_t *val);
DLL_VARIABLE mu_boolean  object_element_set_boolean(object_t *obj, size_t idx, mu_boolean value);
DLL_VARIABLE mu_boolean  object_element_set_int(object_t *obj, size_t idx, int64 value);
DLL_VARIABLE mu_boolean  object_element_set_uint(object_t *obj, size_t idx, uint64 value);
DLL_VARIABLE mu_boolean  object_element_set_double(object_t *obj, size_t idx, double value);
DLL_VARIABLE mu_boolean  object_element_set_string(object_t *obj, size_t idx, const char* str);
DLL_VARIABLE mu_boolean  object_element_set_stringLen(object_t *obj, size_t idx, const char* str, size_t len);
DLL_VARIABLE mu_boolean  object_element_set_stringN(object_t *obj, size_t idx, char c, size_t n);
DLL_VARIABLE mu_boolean  object_element_set_string_zerocopy(object_t *obj, size_t idx, char* str, size_t len);
DLL_VARIABLE mu_boolean  object_element_set_zerocopy(object_t *obj, size_t idx, string_t str);

/** @} */

/** 
 * @defgroup at at 系列函数
 * 
 * 获取类型为 object_type_array 或 object_type_object 的对象实例中第 idx 个对象
 *
 * 注意如果对象为 object_type_object 时, 返回的是一个 key/value 对
 *
 * @{
 *
 * @param[in] obj 对象的实例
 * @param[in] idx 位置
 */
DLL_VARIABLE object_t*  object_element_at(object_t *obj, size_t idx);
DLL_VARIABLE int64      object_element_at_int(object_t *obj, size_t idx, int64 defaultValue);
DLL_VARIABLE uint64     object_element_at_uint(object_t *obj, size_t idx, uint64 defaultValue);
DLL_VARIABLE double     object_element_at_double(object_t *obj, size_t idx, double defaultValue);
DLL_VARIABLE mu_boolean object_element_at_boolean(object_t *obj, size_t idx, mu_boolean defaultValue);
DLL_VARIABLE cstring_t  object_element_at_string(object_t *obj, size_t idx, const char* val, size_t len);

/** @} */

/**
 * 删除类型为 object_type_array 的对象实例的第 idx 个对象
 *
 * 注意如果原位置有子对象时,会自动清理
 *
 * @param[in] obj 对象的实例
 * @param[in] idx 位置
 */
DLL_VARIABLE mu_boolean object_element_del(object_t *obj, size_t idx);


#ifdef __cplusplus
}
#endif

#endif
