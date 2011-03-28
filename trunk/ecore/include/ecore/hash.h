#ifndef _ecore_hashtable_h_
#define _ecore_hashtable_h_ 1

#include "ecore_config.h"

#include <stdlib.h>

#ifdef _MSC_VER
#include "win32/stdbool.h"
#else
#include <stdbool.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** hash表的最大使用率 */
#define HASHTABLE_MAX_USAGE 0.60



typedef struct _hash_entry {

	void *key;

	void *val;
} hash_entry_t;

typedef struct _hashtable hashtable_t;

typedef struct _hashtable_iterator  hashtable_iterator_t;

/** 
 * 创建一个 hash 表
 * @param[in]  hash_func 哈希函数
 * @param[in]  cmp_func  key比较函数
 * @param[in]  free_func key和val的清理函数, 为null时将不清理
 */
DLL_VARIABLE hashtable_t* hashtable_create(unsigned int (*hash_func)(const void *key),
                    int (*cmp_func)(const void *key1, const void *key2),
                    void (*free_func)(void* key, void *val), int default_size);

/**
 * 销毁一个 hash 表
 */
DLL_VARIABLE void hashtable_free(hashtable_t* hash);

/** 
 * 清空一个 hash 表
 */
DLL_VARIABLE void hashtable_clear(hashtable_t* hash);

/** 
 * 按 key 值取得一个val.
 */
DLL_VARIABLE void *hashtable_get(const hashtable_t* hash, const void *key);

/** 
 * 添加一个key和value. 
 */
DLL_VARIABLE void hashtable_set(hashtable_t* hash, void *key, void *val);

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
 * 将迭代器向前移一位
 */
DLL_VARIABLE bool hashtable_iterator_next(hashtable_iterator_t* it);

/**
 * 获取迭代器的当前对象
 */
DLL_VARIABLE hash_entry_t* hashtable_iterator_current(hashtable_iterator_t* it);

DLL_VARIABLE unsigned int hashtable_str_hash(const void *vkey);

DLL_VARIABLE unsigned int hashtable_ptr_hash(const void *key);

DLL_VARIABLE int hashtable_strcmp(const void *key1, const void *key2);

DLL_VARIABLE int hashtable_compare(const void *key1, const void *key2);

#ifdef __cplusplus
};
#endif

#endif //_ecore_hashtable_h_