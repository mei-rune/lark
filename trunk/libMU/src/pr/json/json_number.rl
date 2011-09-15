
#include "json_private.h"

%%{
    machine JSON_number;
    include JSON_common "json_common.rl";
    
    
    action break { fbreak; }
    action mark_ { fbreak; }

    end   = [\]},:] | ignore;
    
    exp   = [eE] > {is_float = MU_TRUE;}. [+\-]? . digit+ . (end >break)?;
    float = '.' >{is_float = MU_TRUE;} . digit+ . (end >break | exp)?;
    main := '-'? >{is_minus = MU_TRUE;}  . (('0' | ([1-9][0-9]*)) . (end >break | float | exp)?) | float; 
}%%

const char *_parse_JSON_number(JSON *json, const char *p, const char *pe, object_t** val)
{
    int cs;
    mu_boolean is_float = MU_FALSE;
    mu_boolean is_minus = MU_FALSE;
    
    char* np;
    const char* old = p;
	uint64 ud;
	int64 d;

    %% write data;
    %% write init;
    %% write exec;


	DEBUG(json->current_nesting, "number");
	
    if (cs < JSON_number_first_final)
    	return NULL;



	p = old;
	if ( is_float )
	{
		double f = strtod(p, &np);
		if (0 == f)
			return NULL;

		*val = object_new_double(f);
		return np;
	}

	if(is_minus)
	{
			d = strtoll(p, &np, 10);
			if (0 != d)
			{
				*val = object_new_int(d);
				return np;
			}
	}

	ud = strtoull(p, &np, 10);
	if (0 == ud)
		return NULL;

	*val = object_new_uint(d);
	return np;
}

