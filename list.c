#include "list.h"


#ifdef TEST

#include "test.h"
#include "lib.h"

void list_test()
{
	list_t *list;
	char *s;

	list = list_new();

	test_assert(list->size, 0, 'd', TEST_EQUAL);

	s = lib_calloc(10);
	sprintf(s, "abc");
	list_push(list, s);
	test_assert(list->size, 1, 'd', TEST_EQUAL);

	s = lib_calloc(10);
	sprintf(s, "def");
	list_push(list, s);
	test_assert(list->size, 2, 'd', TEST_EQUAL);

	s = lib_calloc(10);
	sprintf(s, "ghi");
	list_push(list, s);
	test_assert(list->size, 3, 'd', TEST_EQUAL);

	s = list_pop(list);
	test_assert(s, "abc", 's', TEST_EQUAL);
	lib_free(s);
	test_assert(list->size, 2, 'd', TEST_EQUAL);

	s = list_pop(list);
	test_assert(s, "def", 's', TEST_EQUAL);
	lib_free(s);
	test_assert(list->size, 1, 'd', TEST_EQUAL);

	s = list_pop(list);
	test_assert(s, "ghi", 's', TEST_EQUAL);
	lib_free(s);
	test_assert(list->size, 0, 'd', TEST_EQUAL);

	int n;

	test_assert(list_put(list, 1), TRUE, 'd', TEST_EQUAL);
	test_assert(list_size(list), 1, 'd', TEST_EQUAL);
	test_assert(list_put(list, 2), TRUE, 'd', TEST_EQUAL);
	test_assert(list_size(list), 2, 'd', TEST_EQUAL);
	test_assert(list_put(list, 3), TRUE, 'd', TEST_EQUAL);
	test_assert(list_size(list), 3, 'd', TEST_EQUAL);

	test_assert(list_get(list, n), TRUE, 'd', TEST_EQUAL);
	test_assert(n, 1, 'd', TEST_EQUAL);
	test_assert(list_size(list), 2, 'd', TEST_EQUAL);
	test_assert(list_get(list, n), TRUE, 'd', TEST_EQUAL);
	test_assert(n, 2, 'd', TEST_EQUAL);
	test_assert(list_size(list), 1, 'd', TEST_EQUAL);
	test_assert(list_get(list, n), TRUE, 'd', TEST_EQUAL);
	test_assert(n, 3, 'd', TEST_EQUAL);
	test_assert(list_size(list), 0, 'd', TEST_EQUAL);
	test_assert(list_get(list, n), FALSE, 'd', TEST_EQUAL);

	list_free(list);
}

#endif
