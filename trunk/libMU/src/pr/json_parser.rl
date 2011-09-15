#include "parser.h"

/* unicode */

static const char MinusInfinity[] = "-Infinity";
static const char PositiveInfinity[] = "Infinity";
                
static const char digit_values[256] = { 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1,
    -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1
};

static UTF32 unescape_unicode(const unsigned char *p)
{
    char b;
    UTF32 result = 0;
    b = digit_values[p[0]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | b;
    b = digit_values[p[1]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    b = digit_values[p[2]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    b = digit_values[p[3]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    return result;
}

static int convert_UTF32_to_UTF8(char *buf, UTF32 ch) 
{
    int len = 1;
    if (ch <= 0x7F) {
        buf[0] = (char) ch;
    } else if (ch <= 0x07FF) {
        buf[0] = (char) ((ch >> 6) | 0xC0);
        buf[1] = (char) ((ch & 0x3F) | 0x80);
        len++;
    } else if (ch <= 0xFFFF) {
        buf[0] = (char) ((ch >> 12) | 0xE0);
        buf[1] = (char) (((ch >> 6) & 0x3F) | 0x80);
        buf[2] = (char) ((ch & 0x3F) | 0x80);
        len += 2;
    } else if (ch <= 0x1fffff) {
        buf[0] =(char) ((ch >> 18) | 0xF0);
        buf[1] =(char) (((ch >> 12) & 0x3F) | 0x80);
        buf[2] =(char) (((ch >> 6) & 0x3F) | 0x80);
        buf[3] =(char) ((ch & 0x3F) | 0x80);
        len += 3;
    } else {
        buf[0] = '?';
    }
    return len;
}

%%{
    machine JSON_common;

    cr                  = '\n';
    cr_neg              = [^\n];
    ws                  = [ \t\r\n];
    c_comment           = '/*' ( any* - (any* '*/' any* ) ) '*/';
    cpp_comment         = '//' cr_neg* cr;
    comment             = c_comment | cpp_comment;
    ignore              = ws | comment;
    name_separator      = ':';
    value_separator     = ',';
    Vnull               = 'null';
    Vfalse              = 'false';
    Vtrue               = 'true';
    VNaN                = 'NaN';
    VInfinity           = 'Infinity';
    VMinusInfinity      = '-Infinity';
    begin_value         = [nft"\-[{NI] | digit;
    begin_object        = '{';
    end_object          = '}';
    begin_array         = '[';
    end_array           = ']';
    begin_string        = '"';
    begin_name          = begin_string;
    begin_number        = digit | '-';
}%%

%%{
    machine JSON_object;
    include JSON_common;

    write data;

    action parse_value {
        object_t* v = NULL;
        char *np = JSON_parse_value(json, fpc, pe, &v); 
        if (np == NULL) {
            fhold; fbreak;
        } else {
            object_put_object(result, string_data(&last_name), v);
            fexec np;
        }
    }

    action parse_name {
        char *np;
        json->parsing_name = 1;
        np = JSON_parse_string(json, fpc, pe, &last_name);
        json->parsing_name = 0;
        if (np == NULL) { fhold; fbreak; } 
        else { fexec np; }
    }

    action exit { fhold; fbreak; }

    kv_pair  = ignore* begin_name >parse_name
        ignore* name_separator ignore*
        begin_value >parse_value;

    main := begin_object
          (kv_pair (ignore* value_separator kv_pair)*)?
          ignore* end_object @exit;
}%%

static char *JSON_parse_object(JSON *json, char *p, char *pe, object_t** result)
{
    int cs = EVIL;
    string_buffer_t last_name;
    
    string_buffer_init(&last_name);

    if (json->max_nesting && json->current_nesting > json->max_nesting) {
        json_raise(eNestingError, "nesting of %d is too deep", json->current_nesting);
    }

    *result = object_new_object();

    %% write init;
    %% write exec;


	string_buffer_destroy(&last_name);
	
    if (cs >= JSON_object_first_final) {
        if (json->create_additions) {
            VALUE klassname = rb_hash_aref(*result, json->create_id);
            if (!NIL_P(klassname)) {
                VALUE klass = rb_funcall(mJSON, i_deep_const_get, 1, klassname);
                if (RTEST(rb_funcall(klass, i_json_creatable_p, 0))) {
                    *result = rb_funcall(klass, i_json_create, 1, *result);
                }
            }
        }
        return p + 1;
    } else {
        return NULL;
    }
}

%%{
    machine JSON_value;
    include JSON_common;

    write data;

    action parse_null {
        *result = NULL;
    }
    action parse_false {
        *result = object_new_boolean(MU_FALSE);
    }
    action parse_true {
        *result = object_new_boolean(MU_TRUE);
    }
    action parse_nan {
        if (json->allow_nan) {
            *result = object_new_NaN();
        } else {
            json_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p - 2);
        }
    }
    action parse_infinity {
        if (json->allow_nan) {
            *result = object_new_MinusInfinity();
        } else {
            json_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p - 8);
        }
    }
    
    action parse_string {
        char *np = JSON_parse_string(json, fpc, pe, result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_number {
        char *np;
        if(pe > fpc + 9 && !strncmp(MinusInfinity, fpc, 9)) {
            if (json->allow_nan) {
                *result = object_new_PositiveInfinity();
                fexec p + 10;
                fhold; fbreak;
            } else {
                json_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
            }
        }
        np = JSON_parse_float(json, fpc, pe, result);
        if (np != NULL) fexec np;
        np = JSON_parse_integer(json, fpc, pe, result);
        if (np != NULL) fexec np;
        fhold; fbreak;
    }

    action parse_array { 
        char *np;
        json->current_nesting++;
        np = JSON_parse_array(json, fpc, pe, result);
        json->current_nesting--;
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_object { 
        char *np;
        json->current_nesting++;
        np =  JSON_parse_object(json, fpc, pe, result);
        json->current_nesting--;
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action exit { fhold; fbreak; }

main := (
              Vnull @parse_null |
              Vfalse @parse_false |
              Vtrue @parse_true |
              VNaN @parse_nan |
              VInfinity @parse_infinity |
              begin_number >parse_number |
              begin_string >parse_string |
              begin_array >parse_array |
              begin_object >parse_object
        ) %*exit;
}%%

static char *JSON_parse_value(JSON *json, char *p, char *pe, object_t *result)
{
    int cs = EVIL;

    %% write init;
    %% write exec;

    if (cs >= JSON_value_first_final) {
        return p;
    } else {
        return NULL;
    }
}

%%{
    machine JSON_integer;

    write data;

    action exit { fhold; fbreak; }

    main := '-'? ('0' | [1-9][0-9]*) (^[0-9] @exit);
}%%

static char *JSON_parse_integer(JSON *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;

    %% write init;
    json->memo = p;
    %% write exec;

    if (cs >= JSON_integer_first_final) {
        long len = p - json->memo;
        *result = rb_Integer(rb_str_new(json->memo, len));
        return p + 1;
    } else {
        return NULL;
    }
}

%%{
    machine JSON_float;
    include JSON_common;

    write data;

    action exit { fhold; fbreak; }

    main := '-'? (
              (('0' | [1-9][0-9]*) '.' [0-9]+ ([Ee] [+\-]?[0-9]+)?)
              | (('0' | [1-9][0-9]*) ([Ee] [+\-]?[0-9]+))
             )  (^[0-9Ee.\-] @exit );
}%%

static char *JSON_parse_float(JSON *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;

    %% write init;
    json->memo = p;
    %% write exec;

    if (cs >= JSON_float_first_final) {
        long len = p - json->memo;
        *result = rb_Float(rb_str_new(json->memo, len));
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
    machine JSON_array;
    include JSON_common;

    write data;

    action parse_value {
        object_t* v = NULL;
        char *np = JSON_parse_value(json, fpc, pe, &v); 
        if (np == NULL) {
            fhold; fbreak;
        } else {
            object_element_push(result, v);
            fexec np;
        }
    }

    action exit { fhold; fbreak; }

    next_element  = value_separator ignore* begin_value >parse_value;

    main := begin_array ignore*
          ((begin_value >parse_value ignore*)
           (ignore* next_element ignore*)*)?
          end_array @exit;
}%%

static char *JSON_parse_array(JSON *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;
    VALUE array_class = json->array_class;

    if (json->max_nesting && json->current_nesting > json->max_nesting) {
        json_raise(eNestingError, "nesting of %d is too deep", json->current_nesting);
    }
    *result = NIL_P(array_class) ? rb_ary_new() : rb_class_new_instance(0, 0, array_class);

    %% write init;
    %% write exec;

    if(cs >= JSON_array_first_final) {
        return p + 1;
    } else {
        json_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}

static void json_string_unescape(string_buffer_t* result, char *string, char *stringEnd)
{
    char *p = string, *pe = string, *unescape;
    int unescape_len;

    while (pe < stringEnd)
     {
        if (*pe != '\\')
        {
            pe++;
            string_buffer_appendLen(result, pe, 1);
            continue;
        }
        
        unescape = (char *) "?";
        unescape_len = 1;
        
        if (pe > p)
             
        switch (*++pe)
        {
                case 'n':
                    unescape = (char *) "\n";
                    break;
                case 'r':
                    unescape = (char *) "\r";
                    break;
                case 't':
                    unescape = (char *) "\t";
                    break;
                case '"':
                    unescape = (char *) "\"";
                    break;
                case '\\':
                    unescape = (char *) "\\";
                    break;
                case 'b':
                    unescape = (char *) "\b";
                    break;
                case 'f':
                    unescape = (char *) "\f";
                    break;
                case 'u':
                    if (pe > stringEnd - 4) { 
                        return NULL;
                    } else {
                        char buf[4];
                        UTF32 ch = unescape_unicode((unsigned char *) ++pe);
                        pe += 3;
                        if (UNI_SUR_HIGH_START == (ch & 0xFC00)) {
                            pe++;
                            if (pe > stringEnd - 6) return Qnil;
                            if (pe[0] == '\\' && pe[1] == 'u') {
                                UTF32 sur = unescape_unicode((unsigned char *) pe + 2);
                                ch = (((ch & 0x3F) << 10) | ((((ch >> 6) & 0xF) + 1) << 16)
                                        | (sur & 0x3FF));
                                pe += 5;
                            } else {
                                unescape = (char *) "?";
                                break;
                            }
                        }
                        unescape_len = convert_UTF32_to_UTF8(buf, ch);
                        unescape = buf;
                    }
                    break;
                default:
                    p = pe;
                    continue;
            }
            string_buffer_appendLen(result, unescape, unescape_len);
            p = ++pe;
        
    }
    rb_str_buf_cat(result, p, pe - p);
    return result;
}

%%{
    machine JSON_string;
    include JSON_common;

    write data;

    action parse_string {
        json_string_unescape(result, json->memo + 1, p);
        if (NIL_P(*result)) {
            fhold;
            fbreak;
        } else {
            FORCE_UTF8(*result);
            fexec p + 1;
        }
    }

    action exit { fhold; fbreak; }

    main := '"' ((^(["\\] | 0..0x1f) | '\\'["\\/bfnrt] | '\\u'[0-9a-fA-F]{4} | '\\'^(["\\/bfnrtu]|0..0x1f))* %parse_string) '"' @exit;
}%%

static int
match_i(VALUE regexp, VALUE klass, VALUE memo)
{
    if (regexp == Qundef) return ST_STOP;
    if (RTEST(rb_funcall(klass, i_json_creatable_p, 0)) &&
      RTEST(rb_funcall(regexp, i_match, 1, rb_ary_entry(memo, 0)))) {
        rb_ary_push(memo, klass);
        return ST_STOP;
    }
    return ST_CONTINUE;
}

static char *JSON_parse_string(JSON *json, char *p, char *pe, string_buffer_t* result)
{
    int cs = EVIL;
    VALUE match_string;

    %% write init;
    json->memo = p;
    %% write exec;

    if (json->create_additions && RTEST(match_string = json->match_string)) {
          VALUE klass;
          VALUE memo = rb_ary_new2(2);
          rb_ary_push(memo, *result);
          rb_hash_foreach(match_string, match_i, memo);
          klass = rb_ary_entry(memo, 1);
          if (RTEST(klass)) {
              *result = rb_funcall(klass, i_json_create, 1, *result);
          }
    }

    if (json->symbolize_names && json->parsing_name) {
      *result = rb_str_intern(*result);
    }
    if (cs >= JSON_string_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
    machine JSON;

    write data;

    include JSON_common;

    action parse_object {
        char *np;
        json->current_nesting = 1;
        np = JSON_parse_object(json, fpc, pe, &result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_array {
        char *np;
        json->current_nesting = 1;
        np = JSON_parse_array(json, fpc, pe, &result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    main := ignore* (
            begin_object >parse_object |
            begin_array >parse_array
            ) ignore*;
}%%

/* 
 * Document-class: JSON::Ext::Parser
 *
 * This is the JSON parser implemented as a C extension. It can be configured
 * to be used by setting
 *
 *  JSON.parser = JSON::Ext::Parser
 *
 * with the method parser= in JSON.
 *
 */

static VALUE convert_encoding(VALUE source)
{
    char *ptr = RSTRING_PTR(source);
    long len = RSTRING_LEN(source);
    if (len < 2) {
        json_raise(eParserError, "A JSON text must at least contain two octets!");
    }
#ifdef HAVE_RUBY_ENCODING_H
    {
        VALUE encoding = rb_funcall(source, i_encoding, 0);
        if (encoding == CEncoding_ASCII_8BIT) {
            if (len >= 4 &&  ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0) {
                source = rb_str_dup(source);
                rb_funcall(source, i_force_encoding, 1, CEncoding_UTF_32BE);
                source = rb_funcall(source, i_encode_bang, 1, CEncoding_UTF_8);
            } else if (len >= 4 && ptr[0] == 0 && ptr[2] == 0) {
                source = rb_str_dup(source);
                rb_funcall(source, i_force_encoding, 1, CEncoding_UTF_16BE);
                source = rb_funcall(source, i_encode_bang, 1, CEncoding_UTF_8);
            } else if (len >= 4 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0) {
                source = rb_str_dup(source);
                rb_funcall(source, i_force_encoding, 1, CEncoding_UTF_32LE);
                source = rb_funcall(source, i_encode_bang, 1, CEncoding_UTF_8);
            } else if (len >= 4 && ptr[1] == 0 && ptr[3] == 0) {
                source = rb_str_dup(source);
                rb_funcall(source, i_force_encoding, 1, CEncoding_UTF_16LE);
                source = rb_funcall(source, i_encode_bang, 1, CEncoding_UTF_8);
            } else {
                FORCE_UTF8(source);
            }
        } else {
            source = rb_funcall(source, i_encode, 1, CEncoding_UTF_8);
        }
    }
#else
    if (len >= 4 &&  ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-32be"), source);
    } else if (len >= 4 && ptr[0] == 0 && ptr[2] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-16be"), source);
    } else if (len >= 4 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-32le"), source);
    } else if (len >= 4 && ptr[1] == 0 && ptr[3] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-16le"), source);
    }
#endif
    return source;
}

/*
 * call-seq: parse()
 *
 *  Parses the current JSON text _source_ and returns the complete data
 *  structure as a result.
 */
static object_t* cParser_parse(const char* data, size_t len)
{
    char *p, *pe;
    int cs = EVIL;
    object_t* result = NULL;
	JSON json;
	
	
    %% write init;
    p = data;
    pe = p + len;
    %% write exec;

    if (cs >= JSON_first_final && p == pe) {
        return result;
    } else {
        json_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}