
#ifndef _ecore_unittestting_h_
#define _ecore_unittestting_h_ 1

#include "ecore_config.h"

// Include files
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <errno.h> 

#ifdef __cplusplus
extern "C" {
#endif
	
extern int Test_Flags_Verbose;


#define WRITE_TO_STDERR(buf, len) if(0 != out_fn)LogPrintf(buf, len); else fwrite(buf, 1, len, stderr)

#define CHECK(condition)                                                \
  do {                                                                  \
    if (!(condition)) {                                                 \
      WRITE_TO_STDERR("Check failed: " #condition "\n",                 \
                      sizeof("Check failed: " #condition "\n")-1);      \
	  abort();                                                          \
      exit(1);                                                          \
    }                                                                   \
  } while (0)


#define RAW_CHECK(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      WRITE_TO_STDERR("Check failed: " #condition ": " message "\n",           \
                      sizeof("Check failed: " #condition ": " message "\n")-1);\
	  abort();                                                                 \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)


#define PCHECK(condition)                                               \
  do {                                                                  \
    if (!(condition)) {                                                 \
      const int err_no = errno;                                         \
      WRITE_TO_STDERR("Check failed: " #condition ": ",                 \
                      sizeof("Check failed: " #condition ": ")-1);      \
      WRITE_TO_STDERR(strerror(err_no), strlen(strerror(err_no)));      \
      WRITE_TO_STDERR("\n", sizeof("\n")-1);                            \
	  abort();                                                          \
      exit(1);                                                          \
    }                                                                   \
  } while (0)

#define CHECK_OP(op, val1, val2)                                        \
  do {                                                                  \
    if (!((val1) op (val2))) {                                          \
      fprintf(stderr, "Check failed: %s %s %s\n", #val1, #op, #val2);   \
	  abort();                                                          \
      exit(1);                                                          \
    }                                                                   \
  } while (0)

#define EXPECT_EQ(val1, val2) CHECK_OP(==, val1, val2)
#define EXPECT_NE(val1, val2) CHECK_OP(!=, val1, val2)
#define EXPECT_LE(val1, val2) CHECK_OP(<=, val1, val2)
#define EXPECT_LT(val1, val2) CHECK_OP(< , val1, val2)
#define EXPECT_GE(val1, val2) CHECK_OP(>=, val1, val2)
#define EXPECT_GT(val1, val2) CHECK_OP(> , val1, val2)

#define EXPECT_TRUE(cond)     CHECK(cond)
#define EXPECT_FALSE(cond)    CHECK(!(cond))
#define EXPECT_STREQ(a, b)    CHECK(strcmp(a, b) == 0)

#define CHECK_ERR(invocation)  PCHECK((invocation) != -1)


enum LogSeverity {INFO = -1, WARNING = -2, LOG_ERROR = -3, FATAL = -4};



#define VLOG_IS_ON(severity) (Test_Flags_Verbose >= severity)


#define LOG_PRINTF(severity, pat) do {							\
  if (VLOG_IS_ON(severity)) {									\
    va_list ap;													\
    va_start(ap, pat);											\
    char buf[600];												\
	vsnprintf(buf, sizeof(buf), pat, ap);						\
	if (buf[0] != '\0' && buf[strlen(buf)-1] != '\n') {			\
		assert(strlen(buf)+1 < sizeof(buf));					\
		strcat(buf, "\n");										\
	}															\
	WRITE_TO_STDERR(buf, strlen(buf));							\
	if ((severity) == FATAL)									\
		abort();												\
    va_end(ap);													\
  }																\
} while (0)




typedef void (*out_fn_t)(const char* buf, size_t len);

#define TEST(a, b)												\
void test_##a##_##b##_run(out_fn_t out_fn);						\
int test_##a##_##b##_var = ADD_RUN_TEST(&test_##a##_##b##_run);	\
void test_##a##_##b##_run(out_fn_t out_fn)

DLL_VARIABLE int ADD_RUN_TEST(void (*func)(out_fn_t fn));
DLL_VARIABLE int RUN_ALL_TESTS(void (*out_fn)(const char* buf, size_t len));

#ifdef __cplusplus
}
#endif

#endif // _ecore_unittestting_h_
