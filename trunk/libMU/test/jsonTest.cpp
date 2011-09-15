#include "json_tokener.h"
#include "unittest.h"

const char simpleJson[] = "[ 12, -23, \"123\", true, false"
   ", [12, -23, \"123\", true, false, {\"a\": \"b\"}],"
   " {"
       "\"a1\":1, \"a2\":-2, \"a3\":true, \"a4\":false, \"a5\":\"str\", \"a6\":[12, -23, \"123\", true, false, {\"a\": \"b\"}], \"a11\":12 "
   "},"
   " {} ]"; 

TEST(json, simple)
{
	object_t* ar;
	object_t* ar2;
	object_t* map;
	object_t* map2;
	object_t* ret = json_parse(simpleJson, sizeof(simpleJson));
	ASSERT_EQ( MU_TRUE, object_is_type(ret, object_type_array));
	ASSERT_EQ( 8, object_length(ret));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ret, 0), object_type_number));
	ASSERT_EQ( 12, object_to_int(object_element_at(ret, 0), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ret, 1), object_type_number));
	ASSERT_EQ( -23, object_to_int(object_element_at(ret, 1), 0));

	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ret, 2), object_type_string));
	ASSERT_STREQ( "123", string_data(&object_to_string(object_element_at(ret, 2), 0, 0)));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ret, 3), object_type_boolean));
	ASSERT_EQ( MU_TRUE, object_to_boolean(object_element_at(ret, 3), MU_FALSE));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ret, 4), object_type_boolean));
	ASSERT_EQ( MU_FALSE, object_to_boolean(object_element_at(ret, 4), MU_FALSE));


	ar = object_element_at(ret, 5);
	ASSERT_EQ( MU_TRUE, object_is_type(ar, object_type_array));
	ASSERT_EQ( 6, object_length(ar));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar, 0), object_type_number));
	ASSERT_EQ( 12, object_to_int(object_element_at(ar, 0), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar, 1), object_type_number));
	ASSERT_EQ( -23, object_to_int(object_element_at(ar, 1), 0));

	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar, 2), object_type_string));
	ASSERT_STREQ( "123", string_data(&object_to_string(object_element_at(ar, 2), 0, 0)));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar, 3), object_type_boolean));
	ASSERT_EQ( MU_TRUE, object_to_boolean(object_element_at(ar, 3), MU_FALSE));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar, 4), object_type_boolean));
	ASSERT_EQ( MU_FALSE, object_to_boolean(object_element_at(ar, 4), MU_TRUE));


	
	map = object_element_at(ar, 5);
	ASSERT_EQ( MU_TRUE, object_is_type(map, object_type_table));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map, "a"), object_type_string));
	ASSERT_STREQ( "b", string_data(&object_to_string(object_get_object(map, "a"),0,0)));

	map2 = object_element_at(ret, 6);
	ASSERT_EQ( MU_TRUE, object_is_type(map2, object_type_table));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a1"), object_type_number));
	ASSERT_EQ( 1, object_to_int(object_get_object(map2, "a1"), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a2"), object_type_number));
	ASSERT_EQ( -2, object_to_int(object_get_object(map2, "a2"), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a3"), object_type_boolean));
	ASSERT_EQ( MU_TRUE, object_to_int(object_get_object(map2, "a3"), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a4"), object_type_boolean));
	ASSERT_EQ( MU_FALSE, object_to_int(object_get_object(map2, "a4"), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a5"), object_type_string));
	ASSERT_STREQ( "str",string_data(&object_to_string(object_get_object(map2, "a5"),0,0)));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a11"), object_type_number));
	ASSERT_EQ( 12, object_to_int(object_get_object(map2, "a11"), 0));

	
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map2, "a6"), object_type_array));
	ar2 = object_get_object(map2, "a6");

	ASSERT_EQ( MU_TRUE, object_is_type(ar2, object_type_array));
	ASSERT_EQ( 6, object_length(ar2));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar2, 0), object_type_number));
	ASSERT_EQ( 12, object_to_int(object_element_at(ar2, 0), 0));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar2, 1), object_type_number));
	ASSERT_EQ( -23, object_to_int(object_element_at(ar2, 1), 0));

	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar2, 2), object_type_string));
	ASSERT_STREQ( "123", string_data(&object_to_string(object_element_at(ar2, 2),0,0)));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar2, 3), object_type_boolean));
	ASSERT_EQ( MU_TRUE, object_to_boolean(object_element_at(ar2, 3), MU_FALSE));
	ASSERT_EQ( MU_TRUE, object_is_type(object_element_at(ar2, 4), object_type_boolean));
	ASSERT_EQ( MU_FALSE, object_to_boolean(object_element_at(ar2, 4), MU_TRUE));

	
	map2 = object_element_at(ar2, 5);
	ASSERT_EQ( MU_TRUE, object_is_type(map2, object_type_table));
	ASSERT_EQ( MU_TRUE, object_is_type(object_get_object(map, "a"), object_type_string));
	ASSERT_STREQ( "b", string_data(&object_to_string(object_get_object(map, "a"),0,0)));

}