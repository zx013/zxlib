#include "algorithm.h"


int algorithm_init(int array[], int len)
{
	int i;
	for(i = 0; i < len; i++)
		array[i] = rand() % 1000;
}

int algorithm_show(int array[], int len)
{
	int i;
	for(i = 0; i < len; i++)
		printf("%3d ", array[i]);
	printf("\n");
}

int algorithm_check(int array[], int len)
{
	int check = TRUE;
	int i;
	for(i = 0; i < len; i++)
	{
		if(i + 1 < len)
		{
			if(array[i] > array[i + 1])
				check = FALSE;
		}
	}

	return check;
}

int algorithm_insert_sort(int array[], int len)
{
	int tmp;
	int i, j;

	for(i = 0; i < len; i++)
	{
		tmp = array[i];
		for(j = i; j > 0 && tmp < array[j - 1]; j--)
			array[j] = array[j - 1];
		array[j] = tmp;
	}
}

int algorithm_bisearch(int array[], int len, int num)
{
	int mid;

	mid = len / 2;
	if(len <= 0)
		return 0;
	else
	{
		if(array[mid] < num)
			return mid + 1 + algorithm_bisearch(array + mid + 1, len - mid - 1, num);
		else if(array[mid] > num)
			return algorithm_bisearch(array, mid - 1, num);
		else
			return mid;
	}
}

int algorithm_bisearch_insert_sort(int array[], int len)
{
	int tmp;
	int i, j;
	int start, end, mid;

	for(i = 1; i < len; i++)
	{
		tmp = array[i];
		start = 0;
		end = i - 1;

		while(start <= end)
		{
			mid = (start + end) / 2;
			if(array[mid] > tmp)
				end = mid - 1;
			else
				start = mid + 1;
		}
		for(j = i; j > end + 1; j--)
			array[j] = array[j - 1];
		array[end + 1] = tmp;
	}
}

int algorithm_shell_sort(int array[], int len)
{
	int d;
	int i, j;
	int tmp;

	for(d = len; d > 0; d /= 2)
	{
		for(i = 0; i < len; i += d)
		{
			tmp = array[i];
			for(j = i; j >= d && tmp < array[j - d]; j -= d)
				array[j] = array[j - d];
			array[j] = tmp;
		}
	}
}

#define algorithm_swap(a, b) \
	do \
	{ \
		typeof(a) __tmp = (a); \
		(a) = (b); \
		(b) = __tmp; \
	} while(0)

int _algorithm_fast_sort(int array[], int start, int end)
{
	int i, j;
	int tmp;

	if(start >= end)
		return;

	i = start;
	j = end - 1;
	tmp = array[end];

	while(i <= j)
	{
		while(array[i] < tmp)
			i++;
		while(array[j] > tmp)
			j--;
		if(i <= j)
		{
			algorithm_swap(array[i], array[j]);
			i++;
			j--;
		}
	}
	algorithm_swap(array[i], array[end]);

	_algorithm_fast_sort(array, start, i - 1);
	_algorithm_fast_sort(array, i + 1, end);
}

int algorithm_fast_sort(int array[], int len)
{
	_algorithm_fast_sort(array, 0, len - 1);
}

int algorithm_select_sort(int array[], int len)
{
	int i, j;
	int min;

	for(i = 0; i < len; i++)
	{
		min = i;
		for(j = i; j < len; j++)
		{
			if(array[j] < array[min])
				min = j;
		}
		algorithm_swap(array[i], array[min]);
	}
}

int algorithm_heap_max(int array[], int head, int len)
{
	int i;

	for(i = head * 2 + 1; i < len; i = i * 2 + 1) //loop while left exist
	{
		head = (i - 1) / 2; //get head

		if(i + 1 < len && array[i + 1] > array[i]) //if right great than left, choose right
			i++;
		if(array[head] >= array[i]) //if head great than left, stop loop
			break;
		algorithm_swap(array[head], array[i]); //exchange head and large leaf
	}
}

int algorithm_heap_sort(int array[], int len)
{
	int i;

	for(i = len - 1; i >= 0; i--)
		algorithm_heap_max(array, i, len);

	for(i = len - 1; i > 0; i--)
	{
		algorithm_swap(array[0], array[i]);
		algorithm_heap_max(array, 0, i);
	}
}

int _algorithm_merging_sort(int array[], int temp[], int start, int end)
{
	int mid;
	int i, j, k;

	if(start == end)
		return;

	mid = (start + end) / 2;
	_algorithm_merging_sort(array, temp, start, mid);
	_algorithm_merging_sort(array, temp, mid + 1, end);

	i = start;
	j = mid + 1;
	k = start;

	while(i <= mid && j <= end)
	{
		if(array[i] < array[j])
			temp[k++] = array[i++];
		else
			temp[k++] = array[j++];
	}

	while(i <= mid)
		temp[k++] = array[i++];

	while(j <= end)
		temp[k++] = array[j++];

	for(i = start; i <= end; i++)
		array[i] = temp[i];
}

int algorithm_merging_sort(int array[], int len)
{
	int temp[len];
	_algorithm_merging_sort(array, temp, 0, len - 1);
}

int algorithm_radix_sort(int array[], int len)
{
	int temp[10][len];
	int index[10];
	int i, j, k, l;

	for(i = 1; i < 1000; i *= 10)
	{
		for(j = 0; j < 10; j++)
			index[j] = 0;

		for(j = 0; j < len; j++)
		{
			k = (array[j] / i) % 10;
			temp[k][index[k]++] = array[j];
		}

		l = 0;
		for(j = 0; j < 10; j++)
		{
			for(k = 0; k < index[j]; k++)
				array[l++] = temp[j][k];
		}
	}
}

int algorithm_kmp_next(char *s, int *next)
{
	int n = strlen(s);
	int i, j;

	next[0] = 0;

	for(i = 1, j = 0; i < n; i++)
	{
		while(j > 0 && s[i] != s[j])
			j = next[j - 1];
		if(s[i] == s[j])
			j++;
		next[i] = j;
	}
}

int algorithm_kmp_search(char *src, char *dst)
{
	int m = strlen(src);
	int n = strlen(dst);
	int next[n];
	int i, j;

	algorithm_kmp_next(dst, next);

	for(i = 0, j = 0; i < m; i++)
	{
		while(j > 0 && src[i] != dst[j])
			j = next[j - 1];
		if(src[i] == dst[j])
			j++;
		if(j == n)
			printf("find %d\n", i - n + 1);
	}
}


#ifdef TEST

#include "test.h"
#include "tools.h"

#define TEST_NUMBER			30

#define algorithm_test_func(func, len) _algorithm_test_func(func, len, #func)
void _algorithm_test_func(int (*func)(int [], int), int len, char *name)
{
	int array[len];
	struct timeval tv_s, tv_e;
	double tv;

	algorithm_init(array, len);
	algorithm_show(array, len);
	gettimeofday(&tv_s, NULL);
	func(array, len);
	gettimeofday(&tv_e, NULL);
	algorithm_show(array, len);
	test_assert(algorithm_check(array, len), TRUE, 'd', TEST_EQUAL);

	tv = (tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec) * 1.0 TO(S);
	printf("%s %fs\n", name, tv);
}

void algorithm_kmp_test()
{
	algorithm_kmp_search("abc def ghi jkl abc ghi", "abc ");
}

void algorithm_test()
{
	algorithm_test_func(algorithm_insert_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_bisearch_insert_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_shell_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_fast_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_select_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_heap_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_merging_sort, TEST_NUMBER);
	algorithm_test_func(algorithm_radix_sort, TEST_NUMBER);
}

#endif
