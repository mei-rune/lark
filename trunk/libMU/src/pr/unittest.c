
#include "unittest.h"
#include "link.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TestCase{
	char* name;
	struct _TestCase* _next;
	void (*func)(out_fn_t fn);
} TestCase;

TestCase g_unittestlist = {0, 0 };
size_t name_max_len = 0;

DLL_VARIABLE int ADD_RUN_TEST(const char* nm, void (*func)(out_fn_t fn))
{
	TestCase* tc = (TestCase*)my_malloc(sizeof(TestCase));
	tc->name = my_strdup(nm);
	name_max_len = mu_max(name_max_len, strlen(nm));
	tc->func = func;
	tc->_next = 0;
	SLINK_Push(&g_unittestlist, tc);
	return 0;
}

void output(out_fn_t out_fn, const char* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	LOG_VPRINTF(fmt, argList);
	va_end( argList );
}

DLL_VARIABLE int RUN_ALL_TESTS(void (*out_fn)(const char* buf, size_t len))
{
	char fmt[512];
	TestCase* next;
	snprintf(fmt, 512, "[%%%us] OK\r\n", name_max_len);

	output(out_fn, "=============== unit test ===============\r\n");
	next = g_unittestlist._next;
	while(0 != next)
	{
		//TestCase* old = next;
		(*(next->func))(out_fn);
		output(out_fn, fmt, next->name);
		next = next->_next;
		//my_free(old);
	}

	output(out_fn, "=============== end ===============\r\n");
    return 0;
}


#ifdef __cplusplus
}
#endif