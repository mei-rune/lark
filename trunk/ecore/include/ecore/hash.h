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

/** hash������ʹ���� */
#define HASHTABLE_MAX_USAGE 0.60



typedef struct _hash_entry {

	void *key;

	void *val;
} hash_entry_t;

typedef struct _hashtable hashtable_t;

typedef struct _hashtable_iterator  hashtable_iterator_t;

/** 
 * ����һ�� hash ��
 * @param[in]  hash_func ��ϣ����
 * @param[in]  cmp_func  key�ȽϺ���
 * @param[in]  free_func key��val��������, Ϊnullʱ��������
 */
DLL_VARIABLE hashtable_t* hashtable_create(unsigned int (*hash_func)(const void *key),
                    int (*cmp_func)(const void *key1, const void *key2),
                    void (*free_func)(void* key, void *val), int default_size);

/**
 * ����һ�� hash ��
 */
DLL_VARIABLE void hashtable_free(hashtable_t* hash);

/** 
 * ���һ�� hash ��
 */
DLL_VARIABLE void hashtable_clear(hashtable_t* hash);

/** 
 * �� key ֵȡ��һ��val.
 */
DLL_VARIABLE void *hashtable_get(const hashtable_t* hash, const void *key);

/** 
 * ���һ��key��value. 
 */
DLL_VARIABLE void hashtable_set(hashtable_t* hash, void *key, void *val);

/**
 * ��ȡ hash �ж������Ŀ
 */
DLL_VARIABLE unsigned int hashtable_count(hashtable_t* hash);

/**
 * ����һ�� hash �ĵ�����
 */
DLL_VARIABLE hashtable_iterator_t* hashtable_iterator_new(hashtable_t* hash);

/**
 * ���� hash �ж������Ŀ
 */
DLL_VARIABLE void hashtable_iterator_free(hashtable_iterator_t* it);

/**
 * ����������ǰ��һλ
 */
DLL_VARIABLE bool hashtable_iterator_next(hashtable_iterator_t* it);

/**
 * ��ȡ�������ĵ�ǰ����
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