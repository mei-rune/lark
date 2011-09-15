
#include "json_private.h"

#define PARSE(X, FPC)                                           \
        json->current_nesting++;                                \
        p = _parse_JSON_##X(json, FPC, pe, result);             \
        json->current_nesting--;                                \
        if (p == NULL) goto failure;

%%{
    machine JSON;
    include JSON_common "json_common.rl";
   

    action parse_string { PARSE(string, fpc);  fexec p;}
    action parse_mapping { PARSE(mapping, fpc);  fexec p; }
    action parse_array { PARSE(array, fpc);  fexec p; }

    action push_true { *result = object_new_boolean(MU_TRUE); }
    action push_false { *result = object_new_boolean(MU_FALSE); }
    action push_null { *result = object_new_Nil(); }
    action push_NaN { *result = object_new_NaN(); }
    action parse_positiveInfinity { *result = object_new_PositiveInfinity(); }
    action parse_minusInfinity { *result = object_new_MinusInfinity(); }
    
    action parse_number {
        if(pe > fpc + 9 && !strncmp(MinusInfinity, fpc, 9))
        {
            if (json->allow_NaN)
            {
                *result = object_new_MinusInfinity();
			}
            fexec p + 10;           
            fhold; fbreak;
        }
        PARSE(number, fpc);
        fexec p;
    }
    

    main := ignore* . (
                begin_number >parse_number |
    			begin_string >parse_string |
    			json_true @push_true |
    			json_false @push_false |
    			json_null @push_null 
    			json_NaN @push_NaN |
                json_PositiveInfinity @parse_positiveInfinity |
                json_MinusInfinity @parse_minusInfinity |
                begin_mapping >parse_mapping |
                begin_array >parse_array
			 ) . ignore* %*{ fhold; fbreak; };
}%%

const char *_parse_JSON(JSON* json, const char *p, const char *pe, object_t** result)
{
    %% write data;
    
    int cs;

	DEBUG(json->current_nesting, "object begin");

    %% write init;
    %% write exec;

    if (cs >= JSON_first_final) {
   
		DEBUG(json->current_nesting, "object end");
		DEBUG(json->current_nesting, object_to_string(*result, 0, 0).str);
		return p;
    }

failure:
	DEBUG(json->current_nesting, "object error!");
    return NULL;
}

