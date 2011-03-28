
#include "ecore_config.h"
#include "internal.h"
#include "ecore/unittest.h"
#include "ecore/link.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TestCase{
	struct _TestCase* _next;
	void (*func)();
} TestCase;

TestCase g_unittestlist = {0, 0 };

DLL_VARIABLE void ADD_RUN_TEST(void (*func)())
{
	TestCase* tc = (TestCase*)my_malloc(sizeof(TestCase));
	tc->func = func;
	SLINK_Push(&g_unittestlist, tc);}

DLL_VARIABLE int RUN_ALL_TESTS()
{
	TestCase* next = g_unittestlist._next;
	while(0 != next)
	{
		TestCase* old = next;
		next->func();
		next = next->_next;
		my_free(old);
	}

  return 0;
}


#ifdef __cplusplus
}
#endif