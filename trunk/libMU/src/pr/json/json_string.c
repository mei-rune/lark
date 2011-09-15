
#line 1 "json_string.rl"

#include "json_private.h"

#define IS_NUNICODE(x)	((x) < 0 || ((x) > 0xd7ff && (x) < 0xe000) || (x) > 0x10ffff)
#define IS_NUNICODE1(x)	((x) < 0 || ((x) > 0xd7ff && (x) < 0xe000))
#define HEX2DEC(x) ((x) <= '9' ? (x) - '0' : ((x) < 'G') ? (x) - 'A' + 10 : (x) - 'a' + 10)



#line 96 "json_string.rl"


const char *_parse_JSON_c_string(JSON* json, const char *p, const char *pe, string_buffer_t* result)
 {
    const char *mark = 0;
    int cs;
    wchar_t temp;
    wchar_t unicode;

    
#line 2 "json_string.c"
static const char _JSON_string_actions[] = {
	0, 1, 1, 1, 3, 1, 4, 2, 
	0, 2, 2, 1, 3
};

static const char _JSON_string_key_offsets[] = {
	0, 0, 1, 5, 9, 17
};

static const char _JSON_string_trans_keys[] = {
	34, 34, 92, 0, 127, 34, 92, 0, 
	127, 34, 47, 92, 98, 102, 110, 114, 
	116, 0
};

static const char _JSON_string_single_lengths[] = {
	0, 1, 2, 2, 8, 0
};

static const char _JSON_string_range_lengths[] = {
	0, 0, 1, 1, 0, 0
};

static const char _JSON_string_index_offsets[] = {
	0, 0, 2, 6, 10, 19
};

static const char _JSON_string_indicies[] = {
	0, 1, 3, 4, 2, 1, 6, 7, 
	5, 1, 8, 8, 8, 8, 8, 8, 
	8, 8, 1, 1, 0
};

static const char _JSON_string_trans_targs[] = {
	2, 0, 3, 5, 4, 3, 5, 4, 
	3
};

static const char _JSON_string_trans_actions[] = {
	0, 0, 1, 10, 10, 0, 3, 3, 
	7
};

static const char _JSON_string_from_state_actions[] = {
	0, 0, 0, 0, 0, 5
};

static const int JSON_string_start = 1;
static const int JSON_string_first_final = 5;
static const int JSON_string_error = 0;

static const int JSON_string_en_main = 1;


#line 106 "json_string.rl"
    
#line 55 "json_string.c"
	{
	cs = JSON_string_start;
	}

#line 107 "json_string.rl"
    
#line 58 "json_string.c"
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
	_acts = _JSON_string_actions + _JSON_string_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 4:
#line 95 "json_string.rl"
	{ p--; {p++; goto _out; } }
	break;
#line 77 "json_string.c"
		}
	}

	_keys = _JSON_string_trans_keys + _JSON_string_key_offsets[cs];
	_trans = _JSON_string_index_offsets[cs];

	_klen = _JSON_string_single_lengths[cs];
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

	_klen = _JSON_string_range_lengths[cs];
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
	_trans = _JSON_string_indicies[_trans];
	cs = _JSON_string_trans_targs[_trans];

	if ( _JSON_string_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JSON_string_actions + _JSON_string_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 30 "json_string.rl"
	{
		switch((*p))
		{
			case '"':
			case '/':
			case '\\':      string_buffer_appendN(result, (*p),   1); break;
			case 'b':       string_buffer_appendN(result, '\b', 1); break;
			case 'f':       string_buffer_appendN(result, '\f', 1); break;
			case 'n':       string_buffer_appendN(result, '\n', 1); break;
			case 'r':       string_buffer_appendN(result, '\r', 1); break;
			case 't':       string_buffer_appendN(result, '\t', 1); break;
		}
    }
	break;
	case 1:
#line 45 "json_string.rl"
	{
		mark = p;
    }
	break;
	case 2:
#line 49 "json_string.rl"
	{ mark = p + 1; }
	break;
	case 3:
#line 52 "json_string.rl"
	{
		if (p - mark > 0)
			string_buffer_appendLen(result, mark, (p - mark));
        
    }
	break;
#line 171 "json_string.c"
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

#line 108 "json_string.rl"


	DEBUG(json->current_nesting, "string");
	
    if (cs >= JSON_string_first_final)
		return p;

failure:

    return NULL;
}

const char *_parse_JSON_string(JSON* json, const char *p, const char *pe, object_t** result)
{
	const char* np;
	string_buffer_t out;
	
	string_buffer_init(&out);
	np = _parse_JSON_c_string(json, p, pe, &out);
	*result = object_new_string(string_data(&out));
	string_buffer_destroy(&out);
	
    return np;
}

#undef HEX2DEC
