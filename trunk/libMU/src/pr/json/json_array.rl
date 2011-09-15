#include "json_private.h"

%%{
    machine JSON_array;
    include JSON_common "json_common.rl";

    action parse_value
    {
		json->current_nesting++;
		np = _parse_JSON(json, fpc, pe, &val);
		json->current_nesting--;

		if(NULL == np)
			goto failure;

		object_element_push(*result, val);
		val = 0;
		
		fexec np;
    }

    main := ('[' . ignore* . (
			      start: (
						']' -> final |
						begin_value >parse_value . ignore* -> more
			      ),
			      more: (
						']' -> final |
						',' . ignore* -> start 
			      )
			   ) %*{ fhold; fbreak; }) ;
}%%

const char* _parse_JSON_array(JSON *json, const char *p, const char *pe, object_t** result)
 {
    int cs;
	object_t* val;
	const char *np;


    %% write data;
    
	DEBUG(json->current_nesting, "array begin");
	*result = object_new_array();
	
    %% write init;
    %% write exec;

    if (cs >= JSON_array_first_final)
    {
    
		DEBUG(json->current_nesting, "array end");
		return p;
	}

failure:

	DEBUG(json->current_nesting, "array error");
    object_destroy(*result);
    *result = 0;
    return NULL;
}
