
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>

#include "ecore/hash.h"

#ifdef __cplusplus
extern "C" {
#endif


/** hash���ڵ� entry */
typedef struct _hash_entry_internal {

	struct _hash_entry  ref;

	struct _hash_entry_internal* next;

} hash_entry_internal_t;


struct _hashtable {
	unsigned int capacity;
	
	/** hash���ڵ�ǰ������. */
	unsigned int used;

	hash_entry_internal_t **entries;

	/** hash ����. */
	unsigned int (*hash_func)(const void *key);

	/** 
	 * key �ıȽϺ���.
	 * @retval -1  key1 < key2
	 * @retval  0  key1 = key2
	 * @retval +1  key1 > key2
	 */
	int (*cmp_func)(const void *key1, const void *key2);

	/** 
	 * �����������.
	 */
	void (*free_func)(void* key, void *val);
};

struct _hashtable_iterator
{
	hashtable_t* table;

	/** entries ������. */
	unsigned int index;

	/** hash_entry ��������. */
	hash_entry_internal_t* next;

};


void _hashtable_resize(hashtable_t* hash);


DLL_VARIABLE int hashtable_strcmp(const void *key1, const void *key2)
{
	return strcmp((const char *) key1, (const char *) key2);
}

DLL_VARIABLE int hashtable_compare(const void *key1, const void *key2)
{
	return (((unsigned long) key1) != ((unsigned long) key2));
}

DLL_VARIABLE hashtable_t* hashtable_create(unsigned int (*hash_func)(const void *key),
                    int (*cmp_func)(const void *key1, const void *key2),
                    void (*free_func)(void* key, void *val), int default_size)
{
	hashtable_t* hash = 0;
	if(NULL == hash_func || NULL == cmp_func)
		return NULL;

	hash = (hashtable_t*) malloc(sizeof(hashtable_t));

	hash->capacity = default_size;
	hash->used = 0;

	hash->entries = (hash_entry_internal_t **)malloc(hash->capacity * sizeof(hash_entry_internal_t*));
	memset(hash->entries, 0, hash->capacity * sizeof(hash_entry_internal_t));

	hash->hash_func = hash_func;
	hash->cmp_func = cmp_func;

	hash->free_func = free_func;
	return hash;
}

DLL_VARIABLE void hashtable_free(hashtable_t* hash)
{
	hashtable_clear(hash);
	free(hash->entries);
	free(hash);
}


void _hashtable_set(hashtable_t* hash, void *key, void *val, hash_entry_internal_t* new_el)
{
	hash_entry_internal_t* el = NULL;
	hash_entry_internal_t* parent_el = NULL;
	
	unsigned int  index = (hash->hash_func)(key) % hash->capacity;
	hash_entry_internal_t*  root_el = hash->entries[index];



	/* ����Ƿ��Ѵ��� */
	el = root_el;
	while (el && (hash->cmp_func)(el->ref.key, key))
	{
		parent_el = el;
		el = el->next;
	}

	if (!val) /* ֵΪ null , ��Ч��ɾ����ֵ�󷵻�. */
	{
		if(el)  /* �Ѵ���, ��ɾ����ֵ */
		{
			if (hash->free_func)
				(hash->free_func)(el->ref.key, el->ref.val);

			if (parent_el)
				parent_el->next = el->next;
			else
				hash->entries[index] = el->next;

			free(el);
			hash->used--;
		}
		return;
	}
	
	if(!new_el)
	{
		if(el)
		{
			/* �Ѵ���, ��ʹ�þɶ��� */
			if (hash->free_func)
				hash->free_func((el->ref.key == key)?NULL:el->ref.key, el->ref.val);
			
			el->ref.key = key;
			el->ref.val = val;
			return;
		}
		
		new_el = (hash_entry_internal_t*) malloc(sizeof(hash_entry_internal_t));
		new_el->ref.key = key;
		new_el->ref.val = val;
	}
	
	new_el->next = NULL;

	
	if (parent_el)
		parent_el->next = new_el;
	else
		hash->entries[index] = new_el;
	
	if(el)/* ������ھ�ֵ */
	{
		new_el->next = el->next;
		free(el);
	}
	else
	{
		hash->used++;
		
		/* ���ʹ���ʴ��� HASHTABLE_MAX_USAGE ʱ�����½�, ��Ҫ������С */
		if ((double) hash->used / hash->capacity > HASHTABLE_MAX_USAGE)
			_hashtable_resize(hash);
	}
	return;
}



void _hashtable_resize(hashtable_t* hash)
{
	unsigned int old_size  = hash->capacity;
	hash_entry_internal_t** old_entries = hash->entries;

	// ���·����ڴ�
	hash->capacity *= 2;
	hash->entries = (hash_entry_internal_t**)malloc(hash->capacity * sizeof(hash_entry_internal_t*));
	memset(hash->entries, 0, hash->capacity * sizeof(hash_entry_internal_t*));

	hash->used = 0;

	{	
		unsigned int i;
		for (i = 0; i < old_size; i++)
		{
			hash_entry_internal_t* el = old_entries[i];
			while (el)
			{
				hash_entry_internal_t* next = el->next;
				_hashtable_set(hash, el->ref.key, el->ref.val, el);
				el = next;
			}
		}
	}

	/* free old data */
	free(old_entries);
}

DLL_VARIABLE void hashtable_set(hashtable_t* hash, void *key, void *val)
{
	_hashtable_set(hash, key, val, 0);
}

DLL_VARIABLE void * hashtable_get(const hashtable_t* hash, const void *key)
{
	int index = (hash->hash_func)(key) % hash->capacity;
	hash_entry_internal_t* el = hash->entries[index];
	while (el) {
		if ((hash->cmp_func)(el->ref.key, key) == 0) {
			return el->ref.val;
		}
		el = el->next;
	}

	return NULL;
}

DLL_VARIABLE void hashtable_clear(hashtable_t* hash)
{
	unsigned int i;

	for (i = 0; i < hash->capacity; i++) 
	{
		hash_entry_internal_t* el = hash->entries[i];
		hash->entries[i] = 0;
		while (el) 
		{
			hash_entry_internal_t* next = el->next;
			if (hash->free_func)
				(hash->free_func)(el->ref.key, el->ref.val);
			free(el);
			el = next;
		}
	}

	hash->used = 0;
}



DLL_VARIABLE unsigned int hashtable_count(hashtable_t* hash)
{
	return hash->used;
}


/**
 * ����һ�� hash �ĵ�����
 */
DLL_VARIABLE hashtable_iterator_t* hashtable_iterator_new(hashtable_t* hash)
{
	hashtable_iterator_t* it = (hashtable_iterator_t*)malloc(sizeof(hashtable_iterator_t));
	it->table = hash;
	it->index = -1;
	it->next = NULL;

	return it;
}

/**
 * ���� hash �ж������Ŀ
 */
DLL_VARIABLE void hashtable_iterator_free(hashtable_iterator_t* it)
{
	free(it);
}

/**
 * ����������ǰ��һλ
 */
DLL_VARIABLE bool hashtable_iterator_next(hashtable_iterator_t* it)
{
	if(NULL != it->next)
		it->next = it->next->next;
	
	if(NULL != it->next)
		return true;

	while( (++it->index) < it->table->capacity )
	{
		it->next = it->table->entries[it->index];;
		if(NULL != it->next)
			return true;
	}

	return false;
}

/**
 * ��ȡ�������ĵ�ǰ����
 */
DLL_VARIABLE hash_entry_t* hashtable_iterator_current(hashtable_iterator_t* it)
{
	return (NULL != it->next)?&(it->next->ref):NULL;
}

DLL_VARIABLE unsigned int hashtable_str_hash(const void *vkey)
{
	unsigned int hash = 5381;
	unsigned int i;
	const char *key = (char *) vkey;

	for (i = 0; key[i]; i++)
		hash = ((hash << 5) + hash) + key[i]; /* hash * 33 + char */

	return hash;
}

DLL_VARIABLE unsigned int hashtable_ptr_hash(const void *key)
{
	return ((unsigned int) key);
}

#ifdef __cplusplus
};
#endif

