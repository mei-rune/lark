

#ifndef _hashtable_h_
#define _hashtable_h_ 1

#include <stdlib.h>


#include "platform.h"


#ifdef __cplusplus
extern "C" {
#endif




/** hash表的最大使用率 */
#define HASHTABLE_MAX_USAGE 0.60

typedef struct hash_entry_s {

	void *key;
	size_t key_len;

	void *value;
} hash_entry_t;

/** hash表内的 entry */
typedef struct hash_thunk_s {

	hash_entry_t  ref;

	struct hash_thunk_s* next;

} hash_thunk_t;


typedef struct hashtable {

	unsigned int size;
	
	/** hash表内当前对象数. */
	unsigned int used;

	hash_thunk_t** entries;

	/** hash 函数. */
	unsigned int (*hash_func)(const void *key, size_t len);

	/** 
	 * key 的比较函数.
	 * @retval -1  key1 < key2
	 * @retval  0  key1 = key2
	 * @retval +1  key1 > key2
	 */
	int (*cmp_func)(const void *key1, size_t len1, const void *key2, size_t len2);

	/** 
	 * 对象的消理函数.
	 */
	void (*free_func)(void* key, size_t key_len, void *val);

} hashtable_t;

typedef struct hashtable_iterator  hashtable_iterator_t;

/** 
 * 初始化一个 hash 表
 * @param[in]  hash_func 哈希函数
 * @param[in]  cmp_func  key比较函数
 * @param[in]  free_func key和val的清理函数, 为null时将不清理
 */
DLL_VARIABLE void hashtable_init(hashtable_t* hash, unsigned int (*hash_func)(const void *key, size_t len)
                    , int (*cmp_func)(const void *key1, size_t len1, const void *key2, size_t len2)
                    , void (*free_func)(void* key, size_t key_len, void *val)
                    , int default_size);
/** 
 * 初始化一个 hash 表
 */
DLL_VARIABLE void hashtable_destroy(hashtable_t* hash);

/** 
 * 创建一个 hash 表
 * @param[in]  hash_func 哈希函数
 * @param[in]  cmp_func  key比较函数
 * @param[in]  free_func key和val的清理函数, 为null时将不清理
 */
DLL_VARIABLE hashtable_t* hashtable_create(unsigned int (*hash_func)(const void *key, size_t len)
                    , int (*cmp_func)(const void *key1, size_t len1, const void *key2, size_t len2)
                    , void (*free_func)(void* key, size_t key_len, void *val)
                    , int default_size);

/**
 * 销毁一个 hash 表
 */
DLL_VARIABLE void hashtable_free(hashtable_t* hash);

/** 
 * 清空一个 hash 表
 */
DLL_VARIABLE void hashtable_clear(hashtable_t* hash);

/** 
 * 按位置取得一个val.
 */
DLL_VARIABLE void *hashtable_at(const hashtable_t* hash, size_t idx);

/** 
 * 按 key 值取得一个val.
 */
DLL_VARIABLE void *hashtable_get(const hashtable_t* hash, const void *key, size_t len);

/** 
 * 添加一个key和value. 
 */
DLL_VARIABLE void hashtable_put(hashtable_t* hash, void *key, size_t key_len, void *val);

/** 
 * 删除一个key和value. 
 */
DLL_VARIABLE mu_boolean hashtable_del(hashtable_t* hash, const void *key, size_t len);

/**
 * 获取 hash 中对象的数目
 */
DLL_VARIABLE unsigned int hashtable_count(hashtable_t* hash);

/**
 * 创建一个 hash 的迭代器
 */
DLL_VARIABLE hashtable_iterator_t* hashtable_iterator_new(hashtable_t* hash);

/**
 * 销毁 hash 中对象的数目
 */
DLL_VARIABLE void hashtable_iterator_free(hashtable_iterator_t* it);

/**
 * 将迭代器向前移一位, 如果还有数据返回1, 否则返回0
 */
DLL_VARIABLE mu_boolean hashtable_iterator_next(hashtable_iterator_t* it);

/**
 * 获取迭代器的当前对象
 */
DLL_VARIABLE hash_entry_t* hashtable_iterator_current(hashtable_iterator_t* it);

DLL_VARIABLE unsigned int _hashtable_str_hash(const void *vkey, size_t len);

DLL_VARIABLE unsigned int _hashtable_ptr_hash(const void *key, size_t len);


DLL_VARIABLE size_t _hash_string(const char *vkey, size_t len);

DLL_VARIABLE int _hashtable_str_cmp(const void *key1, size_t len1, const void *key2, size_t len2);

DLL_VARIABLE int _hashtable_ptr_cmp(const void *key1, size_t len1, const void *key2, size_t len2);

DLL_VARIABLE void _hashtable_null_free(void* key, size_t key_len, void* val);

DLL_VARIABLE void _hashtable_generic_free(void* key, size_t key_len, void* val);

#ifdef __cplusplus
};
#endif

#endif