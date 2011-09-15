#include "object.h"
#include "unittest.h"

TEST(object, simple)
{

 object_t* nan = object_new_NaN();
 object_t* nil = object_new_Nil();;
 object_t* minusInfinity = object_new_MinusInfinity();
 object_t* positiveInfinity = object_new_PositiveInfinity();

 object_t* botrue = object_new_boolean(MU_TRUE);
 object_t* bofalse = object_new_boolean(MU_FALSE);
 object_t* oi = object_new_int(-4);
 object_t* oui = object_new_uint(4);
 object_t* od = object_new_double(0.9);
 object_t* os1 = object_new_string("aaa");
 object_t* os2 = object_new_stringLen("aaaaaaa", 3);
 object_t* os3 = object_new_stringN('a', 3);

 
 ASSERT_STREQ("NaN", object_to_string(nan,0,0).str);
 ASSERT_EQ(0, object_to_string(nil,0,0).str);

 ASSERT_STREQ("-Infinity", object_to_string(minusInfinity,0,0).str);
 ASSERT_STREQ("Infinity", object_to_string(positiveInfinity,0,0).str);
 
 ASSERT_STREQ("true", object_to_string(botrue,0,0).str);
 ASSERT_STREQ("false", object_to_string(bofalse,0,0).str);

 ASSERT_STREQ("-4", object_to_string(oi,0,0).str);
 ASSERT_STREQ("4", object_to_string(oui,0,0).str);
 ASSERT_STREQ("0.9", object_to_string(od,0,0).str);

 ASSERT_STREQ("aaa", object_to_string(os1,0,0).str);
 ASSERT_STREQ("aaa", object_to_string(os2,0,0).str);
 ASSERT_STREQ("aaa", object_to_string(os3,0,0).str);

 object_destroy(nan);
 object_destroy(nil);
 object_destroy(minusInfinity);
 object_destroy(positiveInfinity);
 object_destroy(botrue);
 object_destroy(bofalse);
 object_destroy(oi);
 object_destroy(oui);
 object_destroy(od);
 object_destroy(os1);
 object_destroy(os2);
 object_destroy(os3);
}

TEST(object, array_push_pop)
{
    object_t* obj;

    object_t* ar = object_new_array();
    ASSERT_EQ(0, object_length(ar));
    object_element_push_boolean(ar,  MU_TRUE);
    object_element_push_int(ar,  -23);
    object_element_push_uint(ar,  23);
    object_element_push_double(ar,  2.34);
    object_element_push_string(ar,  "23.34");
    object_element_push_stringLen(ar,  "23.34", 2);
    object_element_push_stringLen(ar,  "23.34dfs", -1);
    object_element_push_stringN(ar,  'a' , 10);

    ASSERT_EQ(8, object_length(ar));

    ASSERT_EQ(object_type_boolean, object_get_type(object_element_at(ar, 0)));
    ASSERT_TRUE(object_to_boolean(object_element_at(ar, 0), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 1)));
    ASSERT_TRUE(-23 == object_to_int(object_element_at(ar, 1), -1));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 2)));
    ASSERT_TRUE(23 == object_to_uint(object_element_at(ar, 2), -1));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 3)));
    //ASSERT_TRUE(23.34 == object_to_double(object_element_at(ar, 3)));

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 4)));
    ASSERT_STREQ("23.34", object_to_string(object_element_at(ar, 4), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 5)));
    ASSERT_STREQ("23", object_to_string(object_element_at(ar, 5), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 6)));
    ASSERT_STREQ("23.34dfs", object_to_string(object_element_at(ar, 6), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 7)));
    ASSERT_STREQ("aaaaaaaaaa", object_to_string(object_element_at(ar, 7), 0, 0).str);

    ASSERT_TRUE(NULL == object_element_at(ar, 3437));


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_boolean, object_get_type(obj));
    ASSERT_TRUE(object_to_boolean(obj, MU_FALSE));
    object_destroy(obj);


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_number, object_get_type(obj));
    ASSERT_TRUE(-23 == object_to_int(obj, 0));
    object_destroy(obj);


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_number, object_get_type(obj));
    ASSERT_TRUE(23 == object_to_uint(obj, -1));
    object_destroy(obj);

    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_number, object_get_type(obj));
    //ASSERT_TRUE(23.34 == object_to_double(obj));
    object_destroy(obj);


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23.34", object_to_string(obj, 0, 0).str);
    object_destroy(obj);


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23", object_to_string(obj, 0, 0).str);
    object_destroy(obj);

    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23.34dfs", object_to_string(obj, 0, 0).str);
    object_destroy(obj);


    obj = object_element_pop(ar);
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("aaaaaaaaaa", object_to_string(obj, 0, 0).str);
    object_destroy(obj);

    ASSERT_EQ(0, object_length(ar));

    object_destroy(ar);
}

TEST(object, array_set_and_del)
{
    object_t* obj;

    object_t* ar = object_new_array();
    ASSERT_EQ(0, object_length(ar));
    object_element_set_boolean(ar,   0, MU_TRUE);
    object_element_set_int(ar,       1, -23);
    object_element_set_uint(ar,      2, 23);
    object_element_set_double(ar,    3, 23.34);
    object_element_set_string(ar,    4, "23.34");
    object_element_set_stringLen(ar, 5, "23.34", 2);
    object_element_set_stringLen(ar, 6, "23.34dfs", -1);
    object_element_set_stringN(ar,   7, 'a' , 10);

    ASSERT_EQ(8, object_length(ar));

    ASSERT_EQ(object_type_boolean, object_get_type(object_element_at(ar, 0)));
    ASSERT_TRUE(object_to_boolean(object_element_at(ar, 0), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 1)));
    ASSERT_TRUE(-23 == object_to_int(object_element_at(ar, 1), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 2)));
    ASSERT_TRUE(23 == object_to_uint(object_element_at(ar, 2), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 3)));
    //ASSERT_TRUE(23.34 == object_to_double(object_element_at(ar, 3)));

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 4)));
    ASSERT_STREQ("23.34", object_to_string(object_element_at(ar, 4), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 5)));
    ASSERT_STREQ("23", object_to_string(object_element_at(ar, 5), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 6)));
    ASSERT_STREQ("23.34dfs", object_to_string(object_element_at(ar, 6), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 7)));
    ASSERT_STREQ("aaaaaaaaaa", object_to_string(object_element_at(ar, 7), 0, 0).str);

    ASSERT_TRUE(NULL == object_element_at(ar, 3437));


    object_element_del(ar, 1);
    ASSERT_EQ(7, object_length(ar));

    ASSERT_EQ(object_type_boolean, object_get_type(object_element_at(ar, 0)));
    ASSERT_TRUE(object_to_boolean(object_element_at(ar, 0), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 1)));
    ASSERT_TRUE(23 == object_to_uint(object_element_at(ar, 1), 0));

    object_element_del(ar, 6);
    object_element_del(ar, 5);
    object_element_del(ar, 4);
    object_element_del(ar, 3);
    object_element_del(ar, 2);
    object_element_del(ar, 1);
    object_element_del(ar, 0);

    ASSERT_EQ(0, object_length(ar));

    object_destroy(ar);
}


TEST(object, array_set_and_del2)
{
    object_t* obj;

    object_t* ar = object_new_array();
    ASSERT_EQ(0, object_length(ar));
    object_element_set_stringN(ar,   7, 'a' , 10);


    ASSERT_TRUE(NULL == object_element_at(ar, 6));
    ASSERT_TRUE(NULL == object_element_at(ar, 5));
    ASSERT_TRUE(NULL == object_element_at(ar, 4));
    ASSERT_TRUE(NULL == object_element_at(ar, 3));
    ASSERT_TRUE(NULL == object_element_at(ar, 2));
    ASSERT_TRUE(NULL == object_element_at(ar, 1));
    ASSERT_TRUE(NULL == object_element_at(ar, 0));

    object_element_set_stringLen(ar, 6, "23.34dfs", -1);
    object_element_set_stringLen(ar, 5, "23.34", 2);
    object_element_set_string(ar,    4, "23.34");
    object_element_set_double(ar,    3, 2.34);
    object_element_set_uint(ar,      2, 23);
    object_element_set_int(ar,       1, -23);
    object_element_set_boolean(ar,   0, MU_TRUE);

    ASSERT_EQ(8, object_length(ar));

    ASSERT_EQ(object_type_boolean, object_get_type(object_element_at(ar, 0)));
    ASSERT_TRUE(object_to_boolean(object_element_at(ar, 0), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 1)));
    ASSERT_TRUE(-23 == object_to_int(object_element_at(ar, 1), 0));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 2)));
    ASSERT_TRUE(23 == object_to_uint(object_element_at(ar, 2), 0));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 3)));
    //ASSERT_TRUE(23.34 == object_to_double(object_element_at(ar, 3)));

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 4)));
    ASSERT_STREQ("23.34", object_to_string(object_element_at(ar, 4), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 5)));
    ASSERT_STREQ("23", object_to_string(object_element_at(ar, 5), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 6)));
    ASSERT_STREQ("23.34dfs", object_to_string(object_element_at(ar, 6), 0, 0).str);

    ASSERT_EQ(object_type_string, object_get_type(object_element_at(ar, 7)));
    ASSERT_STREQ("aaaaaaaaaa", object_to_string(object_element_at(ar, 7), 0, 0).str);

    ASSERT_TRUE(NULL == object_element_at(ar, 3437));


    object_element_del(ar, 1);
    ASSERT_EQ(7, object_length(ar));

    ASSERT_EQ(object_type_boolean, object_get_type(object_element_at(ar, 0)));
    ASSERT_TRUE(object_to_boolean(object_element_at(ar, 0), MU_FALSE));

    ASSERT_EQ(object_type_number, object_get_type(object_element_at(ar, 1)));
    ASSERT_TRUE(23 == object_to_uint(object_element_at(ar, 1), 0));

    object_element_del(ar, 6);
    object_element_del(ar, 5);
    object_element_del(ar, 4);
    object_element_del(ar, 3);
    object_element_del(ar, 2);
    object_element_del(ar, 1);
    object_element_del(ar, 0);

    ASSERT_EQ(0, object_length(ar));

    object_destroy(ar);
}



TEST(object, dict_put_and_get)
{
    object_t* obj;

    object_t* ar = object_new_table();
    ASSERT_EQ(0, object_length(ar));
    object_put_boolean(ar,   "a0", MU_TRUE);
    object_put_int(ar,       "a1", -23);
    object_put_uint(ar,      "a2", 23);
    object_put_double(ar,    "a3", 2.34);
    object_put_string(ar,    "a4", "23.34");
    object_put_stringLen(ar, "a5", "23.34", 2);
    object_put_stringLen(ar, "a6", "23.34dfs", -1);
    object_put_stringN(ar,   "a7", 'a' , 10);

    ASSERT_EQ(8, object_length(ar));

    obj = object_get_object(ar, "a0");
    ASSERT_EQ(object_type_boolean, object_get_type(obj));
    ASSERT_TRUE(object_get_boolean(ar, "a0", MU_FALSE));


    obj = object_get_object(ar, "a1");
    ASSERT_EQ(object_type_number, object_get_type(obj));
    ASSERT_TRUE(-23 == object_get_int(ar, "a1", 0));

    obj = object_get_object(ar, "a2");
    ASSERT_EQ(object_type_number, object_get_type(obj));
    ASSERT_TRUE(23 == object_get_uint(ar, "a2", 0));


    obj = object_get_object(ar, "a3");
    ASSERT_EQ(object_type_number, object_get_type(obj));
    //ASSERT_TRUE(23.34 == object_get_double(ar, "a3")));


    obj = object_get_object(ar, "a4");
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23.34", object_get_string(ar, "a4", 0, 0).str);


    obj = object_get_object(ar, "a5");
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23", object_get_string(ar, "a5", 0, 0).str);



    obj = object_get_object(ar, "a6");
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("23.34dfs", object_get_string(ar, "a6", 0, 0).str);


    obj = object_get_object(ar, "a7");
    ASSERT_EQ(object_type_string, object_get_type(obj));
    ASSERT_STREQ("aaaaaaaaaa", object_get_string(ar, "a7", 0, 0).str);


    object_del_object(ar, "a3");

    ASSERT_TRUE(NULL != object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a2");
    ASSERT_TRUE(NULL != object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a7");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a6");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a5");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a4");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a1");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL != object_get_object(ar, "a0"));

    object_del_object(ar, "a0");
    ASSERT_TRUE(NULL == object_get_object(ar, "a7"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a6"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a5"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a4"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a3"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a2"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a1"));
    ASSERT_TRUE(NULL == object_get_object(ar, "a0"));

    ASSERT_EQ(0, object_length(ar));

    object_destroy(ar);
}



TEST(object, dict_element)
{
    object_t* obj;
    int a0 = 0;
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int a4 = 0;
    int a5 = 0;
    int a6 = 0;
    int a7 = 0;

    object_t* ar = object_new_table();
    ASSERT_EQ(0, object_length(ar));
    object_put_boolean(ar,   "a0", MU_TRUE);
    object_put_int(ar,       "a1", -23);
    object_put_uint(ar,      "a2", 23);
    object_put_double(ar,    "a3", 2.34);
    object_put_string(ar,    "a4", "23.34");
    object_put_stringLen(ar, "a5", "23.34", 2);
    object_put_stringLen(ar, "a6", "23.34dfs", -1);
    object_put_stringN(ar,   "a7", 'a' , 10);

    ASSERT_EQ(8, object_length(ar));


    for(size_t i = 0; i < 8; ++ i)
    {
        obj = object_element_at(ar, i);
		const char* key = object_element_at_string(obj, 0, 0, 0).str;
        if(0 == strcmp("a0",  key))
        {
            a0 = 1;
            obj = object_element_at(obj, 1);

            ASSERT_EQ(object_type_boolean, object_get_type(obj));
            ASSERT_TRUE(object_to_boolean(obj, MU_FALSE));
        }
        else if(0 == strcmp("a1",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a1 = 1;
            obj = object_element_at(obj, 1);

            ASSERT_EQ(object_type_number, object_get_type(obj));
            ASSERT_TRUE(-23 == object_to_int(obj, 0));

        }
        else if(0 == strcmp("a2",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a2 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_number, object_get_type(obj));
            ASSERT_TRUE(23 == object_to_uint(obj, 0));

        }
        else if(0 == strcmp("a3",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a3 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_number, object_get_type(obj));
            //ASSERT_TRUE(23.34 == object_to_double(obj)));

        }
        else if(0 == strcmp("a4",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a4 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23.34", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a5",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a5 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a6",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a6 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23.34dfs", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a7",  object_element_at_string(obj, 0, 0, 0).str))
        {
            a7 = 1;
            obj = object_element_at(obj, 1);
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("aaaaaaaaaa", object_to_string(obj, 0, 0).str);
        }
        else
        {
            CHECK_MESSAGE(false,"AAAA");
        }
    }



    ASSERT_EQ(a0 , 1);
    ASSERT_EQ(a1 , 1);
    ASSERT_EQ(a2 , 1);
    ASSERT_EQ(a3 , 1);
    ASSERT_EQ(a4 , 1);
    ASSERT_EQ(a5 , 1);
    ASSERT_EQ(a6 , 1);
    ASSERT_EQ(a7 , 1);

    a0 = 0;
    a1 = 0;
    a2 = 0;
    a3 = 0;
    a4 = 0;
    a5 = 0;
    a6 = 0;
    a7 = 0;


    for(size_t i = 0; i < 8; ++ i)
    {
        obj = object_element_at(ar, i);
        if(0 == strcmp("a0", object_get_string(obj, "key", 0, 0).str))
        {
            a0 = 1;
            obj = object_get_object(obj, "value");

            ASSERT_EQ(object_type_boolean, object_get_type(obj));
            ASSERT_TRUE(object_to_boolean(obj, MU_FALSE));
        }
        else if(0 == strcmp("a1", object_get_string(obj, "key", 0, 0).str))
        {
            a1 = 1;
            obj = object_get_object(obj, "value");

            ASSERT_EQ(object_type_number, object_get_type(obj));
            ASSERT_TRUE(-23 == object_to_int(obj, 0));

        }
        else if(0 == strcmp("a2", object_get_string(obj, "key", 0, 0).str))
        {
            a2 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_number, object_get_type(obj));
            ASSERT_TRUE(23 == object_to_uint(obj,0));

        }
        else if(0 == strcmp("a3", object_get_string(obj, "key", 0, 0).str))
        {
            a3 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_number, object_get_type(obj));
            //ASSERT_TRUE(23.34 == object_to_double(obj)));

        }
        else if(0 == strcmp("a4", object_get_string(obj, "key", 0, 0).str))
        {
            a4 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23.34", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a5", object_get_string(obj, "key", 0, 0).str))
        {
            a5 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a6", object_get_string(obj, "key", 0, 0).str))
        {
            a6 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("23.34dfs", object_to_string(obj, 0, 0).str);
        }
        else if(0 == strcmp("a7", object_get_string(obj, "key", 0, 0).str))
        {
            a7 = 1;
            obj = object_get_object(obj, "value");
            ASSERT_EQ(object_type_string, object_get_type(obj));
            ASSERT_STREQ("aaaaaaaaaa", object_to_string(obj, 0, 0).str);
        }
        else
        {
            CHECK_MESSAGE(MU_FALSE,"AAAA");
        }
    }

    
    ASSERT_EQ(a0 , 1);
    ASSERT_EQ(a1 , 1);
    ASSERT_EQ(a2 , 1);
    ASSERT_EQ(a3 , 1);
    ASSERT_EQ(a4 , 1);
    ASSERT_EQ(a5 , 1);
    ASSERT_EQ(a6 , 1);
    ASSERT_EQ(a7 , 1);

    object_destroy(ar);
}