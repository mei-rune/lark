
#include "ecore_config.h"
#include "internal.h"
#include "ecore/unittest.h"
#include "ecore/link.h"

#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct _TestCase{
	struct _TestCase* _next;
	void (*func)(out_fn_t fn);
} TestCase;

TestCase g_unittestlist = {0, 0 };

DLL_VARIABLE int ADD_RUN_TEST(void (*func)(out_fn_t fn))
{
	TestCase* tc = (TestCase*)malloc(sizeof(TestCase));
	tc->func = func;
	tc->_next = 0;
	ecore_slink_push(&g_unittestlist, tc);
	return 0;
}

DLL_VARIABLE int RUN_ALL_TESTS(void (*out_fn)(const char* buf, size_t len))
{
	TestCase* next = g_unittestlist._next;
	while(0 != next)
	{
		TestCase* old = next;
		next->func(out_fn);
		next = next->_next;
		free(old);
	}

  return 0;
}

#ifdef __cplusplus
}
#endif