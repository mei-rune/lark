
#line 1 "json_mapping.rl"
#include "json_private.h"


#line 51 "json_mapping.rl"


const char *_parse_JSON_mapping(JSON *json, const char *p, const char *pe, object_t** result)
{
    int cs;
    const char* np;

    string_buffer_t key;
	object_t* val;
	
	
    
#line 2 "json_mapping.c"
static const char _JSON_mapping_actions[] = {
	0, 1, 0, 1, 1, 1, 2
};

static const char _JSON_mapping_key_offsets[] = {
	0, 0, 1, 8, 14, 16, 17, 19, 
	20, 36, 43, 45, 46, 48, 49, 51, 
	52, 54, 55, 57, 58, 60, 61
};

static const char _JSON_mapping_trans_keys[] = {
	123, 13, 32, 34, 47, 125, 9, 10, 
	13, 32, 47, 58, 9, 10, 42, 47, 
	42, 42, 47, 10, 13, 32, 34, 45, 
	47, 73, 78, 91, 102, 110, 116, 123, 
	9, 10, 48, 57, 13, 32, 44, 47, 
	125, 9, 10, 42, 47, 42, 42, 47, 
	10, 42, 47, 42, 42, 47, 10, 42, 
	47, 42, 42, 47, 10, 0
};

static const char _JSON_mapping_single_lengths[] = {
	0, 1, 5, 4, 2, 1, 2, 1, 
	12, 5, 2, 1, 2, 1, 2, 1, 
	2, 1, 2, 1, 2, 1, 0
};

static const char _JSON_mapping_range_lengths[] = {
	0, 0, 1, 1, 0, 0, 0, 0, 
	2, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const char _JSON_mapping_index_offsets[] = {
	0, 0, 2, 9, 15, 18, 20, 23, 
	25, 40, 47, 50, 52, 55, 57, 60, 
	62, 65, 67, 70, 72, 75, 77
};

static const char _JSON_mapping_indicies[] = {
	0, 1, 0, 0, 2, 3, 4, 0, 
	1, 5, 5, 6, 7, 5, 1, 8, 
	9, 1, 10, 8, 10, 5, 8, 5, 
	9, 7, 7, 11, 11, 12, 11, 11, 
	11, 11, 11, 11, 11, 7, 11, 1, 
	13, 13, 0, 14, 4, 13, 1, 15, 
	16, 1, 17, 15, 17, 13, 15, 13, 
	16, 18, 19, 1, 20, 18, 20, 7, 
	18, 7, 19, 21, 22, 1, 23, 21, 
	23, 0, 21, 0, 22, 1, 0
};

static const char _JSON_mapping_trans_targs[] = {
	2, 0, 3, 18, 22, 3, 4, 8, 
	5, 7, 6, 9, 14, 9, 10, 11, 
	13, 12, 15, 17, 16, 19, 21, 20
};

static const char _JSON_mapping_trans_actions[] = {
	0, 0, 3, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char _JSON_mapping_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 5
};

static const int JSON_mapping_start = 1;
static const int JSON_mapping_first_final = 22;
static const int JSON_mapping_error = 0;

static const int JSON_mapping_en_main = 1;


#line 63 "json_mapping.rl"


	DEBUG(json->current_nesting, "map begin");
	string_buffer_init(&key);
	
	*result = object_new_table();

    
#line 78 "json_mapping.c"
	{
	cs = JSON_mapping_start;
	}

#line 71 "json_mapping.rl"
    
#line 81 "json_mapping.c"
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
	_acts = _JSON_mapping_actions + _JSON_mapping_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 2:
#line 50 "json_mapping.rl"
	{ p--; {p++; goto _out; } }
	break;
#line 100 "json_mapping.c"
		}
	}

	_keys = _JSON_mapping_trans_keys + _JSON_mapping_key_offsets[cs];
	_trans = _JSON_mapping_index_offsets[cs];

	_klen = _JSON_mapping_single_lengths[cs];
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

	_klen = _JSON_mapping_range_lengths[cs];
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
	_trans = _JSON_mapping_indicies[_trans];
	cs = _JSON_mapping_trans_targs[_trans];

	if ( _JSON_mapping_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JSON_mapping_actions + _JSON_mapping_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 8 "json_mapping.rl"
	{
    
		DEBUG(json->current_nesting, "value");
		json->current_nesting++;
		np = _parse_JSON(json, p, pe, &val);
		json->current_nesting--;

		if(NULL == np)
			goto failure;

		object_put_object(*result, string_data(&key), val);
		val = 0;
		string_clear(&key);
		
		{p = (( np))-1;}
    }
	break;
	case 1:
#line 26 "json_mapping.rl"
	{
		DEBUG(json->current_nesting, "key");
		json->current_nesting++;
		np = _parse_JSON_c_string(json, p, pe, &key);
		json->current_nesting--;

		if (np == NULL)
			goto failure;

		{p = (( np))-1;}
    }
	break;
#line 195 "json_mapping.c"
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

#line 72 "json_mapping.rl"

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

