#include "btree.h"


#ifdef TEST

#include "test.h"

void btree_test()
{
	btree_t *btree;
	btree = btree_new();

	char *k1 = "1";
	char k2[] = "2";
	int k3 = 3;
	__int64 k4 = 4;

	test_assert(btree_get(btree, k1), 0, 'd', TEST_EQUAL);
	btree_set(btree, k1);
	test_assert(btree_get(btree, k1), 0, 'd', TEST_UNEQUAL);
	test_assert(btree_size(btree), 1, 'd', TEST_EQUAL);
	btree_del(btree, k1);
	test_assert(btree_get(btree, k1), 0, 'd', TEST_EQUAL);
	test_assert(btree_size(btree), 0, 'd', TEST_EQUAL);

	test_assert(btree_get(btree, k2), 0, 'd', TEST_EQUAL);
	btree_set(btree, k2);
	test_assert(btree_get(btree, k2), 0, 'd', TEST_UNEQUAL);
	test_assert(btree_size(btree), 1, 'd', TEST_EQUAL);
	btree_del(btree, k2);
	test_assert(btree_get(btree, k2), 0, 'd', TEST_EQUAL);
	test_assert(btree_size(btree), 0, 'd', TEST_EQUAL);

	test_assert(btree_get(btree, k3), 0, 'd', TEST_EQUAL);
	btree_set(btree, k3);
	test_assert(btree_get(btree, k3), 0, 'd', TEST_UNEQUAL);
	test_assert(btree_size(btree), 1, 'd', TEST_EQUAL);
	btree_del(btree, k3);
	test_assert(btree_get(btree, k3), 0, 'd', TEST_EQUAL);
	test_assert(btree_size(btree), 0, 'd', TEST_EQUAL);

	test_assert(btree_get(btree, k4), 0, 'd', TEST_EQUAL);
	btree_set(btree, k4);
	test_assert(btree_get(btree, k4), 0, 'd', TEST_UNEQUAL);
	test_assert(btree_size(btree), 1, 'd', TEST_EQUAL);
	btree_del(btree, k4);
	test_assert(btree_get(btree, k4), 0, 'd', TEST_EQUAL);
	test_assert(btree_size(btree), 0, 'd', TEST_EQUAL);

	btree_free(btree);
}

#endif
