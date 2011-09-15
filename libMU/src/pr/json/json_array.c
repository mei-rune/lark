
#line 1 "json_array.rl"
#include "json_private.h"


#line 32 "json_array.rl"


const char* _parse_JSON_array(JSON *json, const char *p, const char *pe, object_t** result)
 {
    int cs;
	object_t* val;
	const char *np;


    
#line 2 "json_array.c"
static const char _JSON_array_actions[] = {
	0, 1, 0, 1, 1
};

static const char _JSON_array_key_offsets[] = {
	0, 0, 1, 18, 25, 27, 28, 30, 
	31, 33, 34, 36, 37
};

static const char _JSON_array_trans_keys[] = {
	91, 13, 32, 34, 45, 47, 73, 78, 
	91, 93, 102, 110, 116, 123, 9, 10, 
	48, 57, 13, 32, 44, 47, 93, 9, 
	10, 42, 47, 42, 42, 47, 10, 42, 
	47, 42, 42, 47, 10, 0
};

static const char _JSON_array_single_lengths[] = {
	0, 1, 13, 5, 2, 1, 2, 1, 
	2, 1, 2, 1, 0
};

static const char _JSON_array_range_lengths[] = {
	0, 0, 2, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _JSON_array_index_offsets[] = {
	0, 0, 2, 18, 25, 28, 30, 33, 
	35, 38, 40, 43, 45
};

static const char _JSON_array_indicies[] = {
	0, 1, 0, 0, 2, 2, 3, 2, 
	2, 2, 4, 2, 2, 2, 2, 0, 
	2, 1, 5, 5, 0, 6, 4, 5, 
	1, 7, 8, 1, 9, 7, 9, 5, 
	7, 5, 8, 10, 11, 1, 12, 10, 
	12, 0, 10, 0, 11, 1, 0
};

static const char _JSON_array_trans_targs[] = {
	2, 0, 3, 8, 12, 3, 4, 5, 
	7, 6, 9, 11, 10
};

static const char _JSON_array_trans_actions[] = {
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _JSON_array_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 3
};

static const int JSON_array_start = 1;
static const int JSON_array_first_final = 12;
static const int JSON_array_error = 0;

static const int JSON_array_en_main = 1;


#line 42 "json_array.rl"
    
	DEBUG(json->current_nesting, "array begin");
	*result = object_new_array();
	
    
#line 64 "json_array.c"
	{
	cs = JSON_array_start;
	}

#line 47 "json_array.rl"
    
#line 67 "json_array.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _JSON_array_actions + _JSON_array_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 31 "json_array.rl"
	{ p--; {p++; goto _out; } }
	break;
#line 86 "json_array.c"
		}
	}

	_keys = _JSON_array_trans_keys + _JSON_array_key_offsets[cs];
	_trans = _JSON_array_index_offsets[cs];

	_klen = _JSON_array_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _JSON_array_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _JSON_array_indicies[_trans];
	cs = _JSON_array_trans_targs[_trans];

	if ( _JSON_array_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JSON_array_actions + _JSON_array_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 8 "json_array.rl"
	{
		json->current_nesting++;
		np = _parse_JSON(json, p, pe, &val);
		json->current_nesting--;

		if(NULL == np)
			goto failure;

		object_element_push(*result, val);
		val = 0;
		
		{p = (( np))-1;}
    }
	break;
#line 165 "json_array.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 48 "json_array.rl"

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
