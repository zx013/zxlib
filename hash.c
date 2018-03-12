#include "hash.h"


#ifdef TEST

#include "test.h"

void hash_test()
{
	hash_t *hash;
	hash = hash_new();

	char *k1 = "1";
	char k2[] = "2";
	int k3 = 3;
	__int64 k4 = 4;

	hash_set(hash, k1, "test1");
	test_assert(hash_size(hash), 1, 'd', TEST_EQUAL);
	test_assert(hash_get(hash, k1), "test1", 's', TEST_EQUAL);

	hash_set(hash, k2, "test2");
	test_assert(hash_size(hash), 2, 'd', TEST_EQUAL);
	test_assert(hash_get(hash, k2), "test2", 's', TEST_EQUAL);

	hash_set(hash, k3, "test3");
	test_assert(hash_size(hash), 3, 'd', TEST_EQUAL);
	test_assert(hash_get(hash, k3), "test3", 's', TEST_EQUAL);

	hash_set(hash, k4, "test4");
	test_assert(hash_size(hash), 4, 'd', TEST_EQUAL);
	test_assert(hash_get(hash, k4), "test4", 's', TEST_EQUAL);


	hash_del(hash, k1);
	test_assert(hash_size(hash), 3, 'd', TEST_EQUAL);

	hash_del(hash, k2);
	test_assert(hash_size(hash), 2, 'd', TEST_EQUAL);

	hash_del(hash, k3);
	test_assert(hash_size(hash), 1, 'd', TEST_EQUAL);

	hash_del(hash, k4);
	test_assert(hash_size(hash), 0, 'd', TEST_EQUAL);

	hash_free(hash);
}

#endif
