
#line 1 "json_number.rl"

#include "json_private.h"


#line 17 "json_number.rl"


const char *_parse_JSON_number(JSON *json, const char *p, const char *pe, object_t** val)
{
    int cs;
    mu_boolean is_float = MU_FALSE;
    mu_boolean is_minus = MU_FALSE;
    
    char* np;
    const char* old = p;
	uint64 ud;
	int64 d;

    
#line 2 "json_number.c"
static const char _JSON_number_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3
};

static const char _JSON_number_key_offsets[] = {
	0, 0, 5, 8, 10, 12, 13, 15, 
	16, 20, 22, 34, 34, 47, 58
};

static const char _JSON_number_trans_keys[] = {
	45, 46, 48, 49, 57, 48, 49, 57, 
	48, 57, 42, 47, 42, 42, 47, 10, 
	43, 45, 48, 57, 48, 57, 13, 32, 
	44, 46, 47, 58, 69, 93, 101, 125, 
	9, 10, 13, 32, 44, 47, 58, 69, 
	93, 101, 125, 9, 10, 48, 57, 13, 
	32, 44, 47, 58, 93, 125, 9, 10, 
	48, 57, 13, 32, 44, 46, 47, 58, 
	69, 93, 101, 125, 9, 10, 48, 57, 
	0
};

static const char _JSON_number_single_lengths[] = {
	0, 3, 1, 0, 2, 1, 2, 1, 
	2, 0, 10, 0, 9, 7, 10
};

static const char _JSON_number_range_lengths[] = {
	0, 1, 1, 1, 0, 0, 0, 0, 
	1, 1, 1, 0, 2, 2, 2
};

static const char _JSON_number_index_offsets[] = {
	0, 0, 5, 8, 10, 13, 15, 18, 
	20, 24, 26, 38, 39, 51, 61
};

static const char _JSON_number_indicies[] = {
	0, 2, 3, 4, 1, 5, 6, 1, 
	7, 1, 8, 9, 1, 10, 8, 10, 
	11, 8, 11, 9, 12, 12, 13, 1, 
	13, 1, 14, 14, 14, 2, 15, 14, 
	16, 14, 16, 14, 14, 1, 1, 14, 
	14, 14, 15, 14, 16, 14, 16, 14, 
	14, 7, 1, 14, 14, 14, 15, 14, 
	14, 14, 14, 13, 1, 14, 14, 14, 
	2, 15, 14, 16, 14, 16, 14, 14, 
	6, 1, 0
};

static const char _JSON_number_trans_targs[] = {
	2, 0, 3, 10, 14, 10, 14, 12, 
	5, 7, 6, 11, 9, 13, 11, 4, 
	8
};

static const char _JSON_number_trans_actions[] = {
	7, 0, 5, 7, 7, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 1, 1, 
	3
};

static const int JSON_number_start = 1;
static const int JSON_number_first_final = 10;
static const int JSON_number_error = 0;

static const int JSON_number_en_main = 1;


#line 31 "json_number.rl"
    
#line 71 "json_number.c"
	{
	cs = JSON_number_start;
	}

#line 32 "json_number.rl"
    
#line 74 "json_number.c"
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
	_keys = _JSON_number_trans_keys + _JSON_number_key_offsets[cs];
	_trans = _JSON_number_index_offsets[cs];

	_klen = _JSON_number_single_lengths[cs];
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

	_klen = _JSON_number_range_lengths[cs];
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
	_trans = _JSON_number_indicies[_trans];
	cs = _JSON_number_trans_targs[_trans];

	if ( _JSON_number_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JSON_number_actions + _JSON_number_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 9 "json_number.rl"
	{ {p++; goto _out; } }
	break;
	case 1:
#line 14 "json_number.rl"
	{is_float = MU_TRUE;}
	break;
	case 2:
#line 15 "json_number.rl"
	{is_float = MU_TRUE;}
	break;
	case 3:
#line 16 "json_number.rl"
	{is_minus = MU_TRUE;}
	break;
#line 159 "json_number.c"
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

#line 33 "json_number.rl"


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

