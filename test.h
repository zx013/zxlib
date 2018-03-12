#ifndef TEST_H
#define TEST_H

#ifdef TEST

#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "head.h"
#include "dict.h"

#define TEST_EQUAL						"equal"
#define TEST_UNEQUAL					"unequal"

int test_assert_imple(char *s1, char *ss1, char *s2, char *ss2, int type, char *equal, const char *funcname);

#define test_assert(s1, ...) __VA_FUNC__(test_assert, s1, #s1, __VA_ARGS__)

//防止多次展开时打印出来的额外括号
#define test_assert_3(s1, ss1, s2) test_assert_imple(cast(s1), ss1, cast(s2), #s2, 'd', TEST_EQUAL, __FUNCTION__)
#define test_assert_4(s1, ss1, s2, type) test_assert_imple(cast(s1), ss1, cast(s2), #s2, type, TEST_EQUAL, __FUNCTION__)
#define test_assert_5(s1, ss1, s2, type, equal) test_assert_imple(cast(s1), ss1, cast(s2), #s2, type, equal, __FUNCTION__)


#endif

#endif
