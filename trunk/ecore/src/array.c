
#include "ecore_config.h"
#include <stdlib.h>
#include <string.h>
#include "ecore/array.h"

#ifdef __cplusplus
extern "C" {
#endif

	struct _array
	{
		void **data;
		size_t length;
		size_t capacity;
		void (*free_fn) (void *data);
	};

	DLL_VARIABLE array_t* array_new(size_t default_size, void (*free_fn) (void *data))
	{
		array_t* arr = (array_t*)calloc(1, sizeof(array_t));
		if(!arr) return 0;
		arr->capacity = default_size;
		arr->length = 0;
		arr->free_fn = free_fn;
		if(!(arr->data = (void**)calloc(sizeof(void*), arr->capacity))) {
			free(arr);
			return 0;
		}
		return arr;
	}

	DLL_VARIABLE void array_free(array_t *arr)
	{
		size_t i;
		for(i = 0; i < arr->length; i++)
			if(arr->data[i]) 
				arr->free_fn(arr->data[i]);
		free(arr->data);
		free(arr);
	}

	DLL_VARIABLE void* array_get(array_t *arr, size_t i)
	{
		if(i >= arr->length) return 0;
		return arr->data[i];
	}

	void _array_expand_internal(array_t *arr, size_t new_size)
	{
		if(new_size < arr->capacity) 
			return ;
		{
		size_t max_size = arr->capacity*2;
		if(max_size < new_size)
			max_size = new_size;

		arr->data = (void**)realloc(arr->data, max_size*sizeof(void*));
		memset(arr->data + arr->length, 0, (max_size-arr->length)*sizeof(void*));
		arr->capacity = max_size;
		}
	}

	DLL_VARIABLE void array_set(array_t *arr, size_t idx, void *data)
	{
		_array_expand_internal(arr, idx);

		if(arr->data[idx]) 
			arr->free_fn(arr->data[idx]);

		arr->data[idx] = data;
		if(arr->length <= idx) 
			arr->length = idx + 1;
	}

	DLL_VARIABLE void array_add(array_t *arr, void *data)
	{
		_array_expand_internal(arr, arr->length);
		
		arr->data[arr->length++] = data;
	}

	DLL_VARIABLE void array_remove(array_t *arr, size_t idx)
	{  
		if(idx >= arr->length) 
			return;
		
		if(arr->data[idx]) 
			arr->free_fn(arr->data[idx]);

		memmove(arr->data + idx, arr->data + idx + 1, arr->length-idx-1);
		arr->length --;
	}

	DLL_VARIABLE size_t array_length(array_t *arr)
	{
		return arr->length;
	}

#ifdef __cplusplus
};
#endif
