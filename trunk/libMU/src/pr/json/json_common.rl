
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
    json_null               = 'null';
    json_false              = 'false';
    json_true               = 'true';
    json_NaN                = 'NaN';
    json_PositiveInfinity           = 'Infinity';
    json_MinusInfinity      = '-Infinity';
    begin_value         = [nft"\-[{NI] | digit;
    begin_mapping        = '{';
    end_object          = '}';
    begin_array         = '[';
    end_array           = ']';
    begin_string        = '"';
    begin_name          = begin_string;
    begin_number        = [\-+.] | digit;
}%%