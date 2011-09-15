
#line 1 "json.rl"

#include "json_private.h"

#define PARSE(X, FPC)                                           \
        json->current_nesting++;                                \
        p = _parse_JSON_##X(json, FPC, pe, result);             \
        json->current_nesting--;                                \
        if (p == NULL) goto failure;


#line 53 "json.rl"


const char *_parse_JSON(JSON* json, const char *p, const char *pe, object_t** result)
{
    
#line 2 "json.c"
static const char _JSON_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10
};

static const char _JSON_key_offsets[] = {
	0, 0, 16, 18, 19, 21, 22, 23, 
	24, 25, 26, 27, 28, 29, 31, 32, 
	34, 35, 36, 37, 38, 39, 40, 41, 
	42, 43, 44, 45, 46, 47, 48, 49, 
	50, 51, 52, 53, 54, 55, 60
};

static const char _JSON_trans_keys[] = {
	13, 32, 34, 43, 45, 47, 73, 91, 
	102, 110, 116, 123, 9, 10, 46, 57, 
	42, 47, 42, 42, 47, 10, 110, 102, 
	105, 110, 105, 116, 121, 42, 47, 42, 
	42, 47, 10, 110, 102, 105, 110, 105, 
	116, 121, 97, 108, 115, 101, 117, 108, 
	108, 78, 97, 78, 114, 117, 101, 13, 
	32, 47, 9, 10, 13, 32, 47, 73, 
	9, 10, 0
};

static const char _JSON_single_lengths[] = {
	0, 12, 2, 1, 2, 1, 1, 1, 
	1, 1, 1, 1, 1, 2, 1, 2, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 3, 4
};

static const char _JSON_range_lengths[] = {
	0, 2, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 1
};

static const unsigned char _JSON_index_offsets[] = {
	0, 0, 15, 18, 20, 23, 25, 27, 
	29, 31, 33, 35, 37, 39, 42, 44, 
	47, 49, 51, 53, 55, 57, 59, 61, 
	63, 65, 67, 69, 71, 73, 75, 77, 
	79, 81, 83, 85, 87, 89, 94
};

static const char _JSON_indicies[] = {
	0, 0, 2, 3, 4, 5, 6, 7, 
	8, 9, 10, 11, 0, 3, 1, 12, 
	13, 1, 14, 12, 14, 15, 12, 15, 
	13, 16, 1, 17, 1, 18, 1, 19, 
	1, 20, 1, 21, 1, 22, 1, 23, 
	24, 1, 25, 23, 25, 0, 23, 0, 
	24, 26, 1, 27, 1, 28, 1, 29, 
	1, 30, 1, 31, 1, 32, 1, 33, 
	1, 34, 1, 35, 1, 36, 1, 37, 
	1, 38, 1, 39, 1, 40, 1, 41, 
	1, 42, 1, 43, 1, 44, 1, 45, 
	1, 15, 15, 46, 15, 1, 15, 15, 
	46, 47, 15, 1, 0
};

static const char _JSON_trans_targs[] = {
	1, 0, 37, 37, 38, 13, 17, 37, 
	24, 28, 34, 37, 3, 5, 4, 37, 
	7, 8, 9, 10, 11, 12, 37, 14, 
	16, 15, 18, 19, 20, 21, 22, 23, 
	37, 25, 26, 27, 37, 29, 30, 31, 
	32, 33, 37, 35, 36, 37, 2, 6
};

static const char _JSON_trans_actions[] = {
	0, 0, 1, 19, 19, 0, 0, 5, 
	0, 0, 0, 3, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 17, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	15, 0, 0, 0, 9, 0, 0, 11, 
	0, 0, 13, 0, 0, 7, 0, 0
};

static const char _JSON_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 21, 21
};

static const int JSON_start = 1;
static const int JSON_first_final = 37;
static const int JSON_error = 0;

static const int JSON_en_main = 1;


#line 58 "json.rl"
    
    int cs;

	DEBUG(json->current_nesting, "object begin");

    
#line 100 "json.c"
	{
	cs = JSON_start;
	}

#line 64 "json.rl"
    
#line 103 "json.c"
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
	_acts = _JSON_actions + _JSON_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 10:
#line 52 "json.rl"
	{ p--; {p++; goto _out; } }
	break;
#line 122 "json.c"
		}
	}

	_keys = _JSON_trans_keys + _JSON_key_offsets[cs];
	_trans = _JSON_index_offsets[cs];

	_klen = _JSON_single_lengths[cs];
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

	_klen = _JSON_range_lengths[cs];
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
	_trans = _JSON_indicies[_trans];
	cs = _JSON_trans_targs[_trans];

	if ( _JSON_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JSON_actions + _JSON_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 15 "json.rl"
	{ PARSE(string, p);  {p = (( p))-1;}}
	break;
	case 1:
#line 16 "json.rl"
	{ PARSE(mapping, p);  {p = (( p))-1;} }
	break;
	case 2:
#line 17 "json.rl"
	{ PARSE(array, p);  {p = (( p))-1;} }
	break;
	case 3:
#line 19 "json.rl"
	{ *result = object_new_boolean(MU_TRUE); }
	break;
	case 4:
#line 20 "json.rl"
	{ *result = object_new_boolean(MU_FALSE); }
	break;
	case 5:
#line 21 "json.rl"
	{ *result = object_new_Nil(); }
	break;
	case 6:
#line 22 "json.rl"
	{ *result = object_new_NaN(); }
	break;
	case 7:
#line 23 "json.rl"
	{ *result = object_new_PositiveInfinity(); }
	break;
	case 8:
#line 24 "json.rl"
	{ *result = object_new_MinusInfinity(); }
	break;
	case 9:
#line 26 "json.rl"
	{
        if(pe > p + 9 && !strncmp(MinusInfinity, p, 9))
        {
            if (json->allow_NaN)
            {
                *result = object_new_MinusInfinity();
			}
            {p = (( p + 10))-1;}           
            p--; {p++; goto _out; }
        }
        PARSE(number, p);
        {p = (( p))-1;}
    }
	break;
#line 228 "json.c"
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

#line 65 "json.rl"

    if (cs >= JSON_first_final) {
   
		DEBUG(json->current_nesting, "object end");
		DEBUG(json->current_nesting, object_to_string(*result, 0, 0).str);
		return p;
    }

failure:
	DEBUG(json->current_nesting, "object error!");
    return NULL;
}

