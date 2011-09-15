
#include "json_private.h"

#define IS_NUNICODE(x)	((x) < 0 || ((x) > 0xd7ff && (x) < 0xe000) || (x) > 0x10ffff)
#define IS_NUNICODE1(x)	((x) < 0 || ((x) > 0xd7ff && (x) < 0xe000))
#define HEX2DEC(x) ((x) <= '9' ? (x) - '0' : ((x) < 'G') ? (x) - 'A' + 10 : (x) - 'a' + 10)


%%{
    machine JSON_string;
    include JSON_common "json_common.rl";

    action hex0 {
		temp = HEX2DEC(fc);
    }

    action hex1 {
		temp *= 16;
		temp += HEX2DEC(fc);
    }

    action end_hex {
		if (IS_NUNICODE(temp))
			goto failure;

		string_buffer_appendLen(result, (const char*)&temp, sizeof(temp));
    }

    action add_unquote
    {
		switch(fc)
		{
			case '"':
			case '/':
			case '\\':      string_buffer_appendN(result, fc,   1); break;
			case 'b':       string_buffer_appendN(result, '\b', 1); break;
			case 'f':       string_buffer_appendN(result, '\f', 1); break;
			case 'n':       string_buffer_appendN(result, '\n', 1); break;
			case 'r':       string_buffer_appendN(result, '\r', 1); break;
			case 't':       string_buffer_appendN(result, '\t', 1); break;
		}
    }

    action string_begin_mark
    {
		mark = fpc;
    }

    action mark_next { mark = fpc + 1; }

    action string_end_mark
    {
		if (fpc - mark > 0)
			string_buffer_appendLen(result, mark, (fpc - mark));
        
    }

    action utf_2_1 { unicode = ((wchar_t)(fc & (0xdf-0xc0))) << 6; }
    action utf_2_2 { unicode |= (wchar_t)(fc & (0xbf-0x80)); }

    action utf_3_1 { unicode = ((wchar_t)(fc & 0x0f)) << 12; }
    action utf_3_2 { unicode |= ((wchar_t)(fc & (0xbf-0x80))) << 6; }
    action utf_3_3 { 
	unicode |= (wchar_t)(fc & (0xbf-0x80));
		if ((unicode < 0x0800 || unicode > 0xd7ff) && (unicode < 0xe000 || unicode > 0xffff)) {
			goto failure;	
		}
    }

    action utf_4_1 { unicode = ((wchar_t)(fc & 0x07)) << 18; }
    action utf_4_2 { unicode |= ((wchar_t)(fc & (0xbf-0x80))) << 12; }

	#action utf_4_3 { unicode |= ((wchar_t)(fc & (0xbf-0x80))) << 6; }
    
    action utf_4_4 { 
		unicode |= (wchar_t)(fc & (0xbf-0x80));
		if (unicode < 0x010000 || unicode > 0x10ffff) {
			goto failure;
		}
    }

    action finish {
			string_buffer_appendLen(result, (const char*)&unicode, sizeof(unicode)); 
    }

    main := '"' . (
			   start: (
				   '"' >string_end_mark -> final |
				   '\\' >string_end_mark -> unquote |
				   (0..33 | 35..91 | 93..127) -> start
			   ),
			   unquote: (
				   ["\\/bfnrt] >add_unquote -> start
			   ) @mark_next
		  ) >string_begin_mark %*{ fhold; fbreak; };
}%%

const char *_parse_JSON_c_string(JSON* json, const char *p, const char *pe, string_buffer_t* result)
 {
    const char *mark = 0;
    int cs;
    wchar_t temp;
    wchar_t unicode;

    %% write data;
    %% write init;
    %% write exec;


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
