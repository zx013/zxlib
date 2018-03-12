#include "sort.h"


#ifdef TEST

#include "test.h"

typedef struct sort_test_struct
{
	int a;
	int b;
} sort_test_struct;

void sort_test()
{
	int array1[3] = {3, 2, 1};

	sort(array1, {return a < b;});
	test_assert(array1[0], 1, 'd', TEST_EQUAL);
	test_assert(array1[1], 2, 'd', TEST_EQUAL);
	test_assert(array1[2], 3, 'd', TEST_EQUAL);

	sort(array1, {return a > b;});
	test_assert(array1[0], 3, 'd', TEST_EQUAL);
	test_assert(array1[1], 2, 'd', TEST_EQUAL);
	test_assert(array1[2], 1, 'd', TEST_EQUAL);


	char *array2[] = {"c", "b", "a"};

	sort(array2, {return strcmp(a, b) < 0;});
	test_assert(array2[0], "a", 's', TEST_EQUAL);
	test_assert(array2[1], "b", 's', TEST_EQUAL);
	test_assert(array2[2], "c", 's', TEST_EQUAL);

	sort(array2, {return strcmp(a, b) > 0;});
	test_assert(array2[0], "c", 's', TEST_EQUAL);
	test_assert(array2[1], "b", 's', TEST_EQUAL);
	test_assert(array2[2], "a", 's', TEST_EQUAL);

	sort_test_struct *array3[3];
	int i;

	for(i = 0; i < 3; i++)
	{
		array3[i] = calloc(1, sizeof(sort_test_struct));
		array3[i]->a = 2 - i;
		array3[i]->b = i;
	}

	sort(array3, {return a->a < b->a;});
	test_assert(array3[0]->a, 0, 'd', TEST_EQUAL);
	test_assert(array3[1]->a, 1, 'd', TEST_EQUAL);
	test_assert(array3[2]->a, 2, 'd', TEST_EQUAL);

	sort(array3, {return a->b < b->b;});
	test_assert(array3[0]->b, 0, 'd', TEST_EQUAL);
	test_assert(array3[1]->b, 1, 'd', TEST_EQUAL);
	test_assert(array3[2]->b, 2, 'd', TEST_EQUAL);

	for(i = 0; i < 3; i++)
		free(array3[i]);
}

#endif
