#include "json_private.h"

%%{
    machine JSON_mapping;
    include JSON_common "json_common.rl";

    action parse_value
    {
    
		DEBUG(json->current_nesting, "value");
		json->current_nesting++;
		np = _parse_JSON(json, fpc, pe, &val);
		json->current_nesting--;

		if(NULL == np)
			goto failure;

		object_put_object(*result, string_data(&key), val);
		val = 0;
		string_clear(&key);
		
		fexec np;
    }
    
    action parse_key
    {
		DEBUG(json->current_nesting, "key");
		json->current_nesting++;
		np = _parse_JSON_c_string(json, fpc, pe, &key);
		json->current_nesting--;

		if (np == NULL)
			goto failure;

		fexec np;
    }

    main := '{' . ignore* . (
					start: (
						'}' -> final |
						'"' >parse_key . ignore* . ':' -> value
					),
					value: (
						ignore* . begin_value >parse_value . ignore* -> repeat
						),
					repeat: (
						',' . ignore* -> start |
						'}' -> final
					)
		       ) %*{ fhold; fbreak; };
}%%

const char *_parse_JSON_mapping(JSON *json, const char *p, const char *pe, object_t** result)
{
    int cs;
    const char* np;

    string_buffer_t key;
	object_t* val;
	
	
    %% write data;


	DEBUG(json->current_nesting, "map begin");
	string_buffer_init(&key);
	
	*result = object_new_table();

    %% write init;
    %% write exec;

	string_buffer_destroy(&key);

    if (cs >= JSON_mapping_first_final)
    {
		DEBUG(json->current_nesting, "map end");
		return p;
	}

failure:

	DEBUG(json->current_nesting, "map error");
    object_destroy(*result);
    *result = 0;
    return NULL;
}

