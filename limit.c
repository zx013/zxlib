#include "limit.h"


inline limit_speed *limit_speed_init(limit_speed_attr *attr)
{
	limit_speed *ls;

	ls = lib_calloc(sizeof(limit_speed));
	if(!ls)
		error_goto(limit_speed_init_error_1);

	ls->limit = attr->limit;
	ls->size = attr->size;
	ls->interval = attr->interval ? attr->interval : 100 MS;
	ls->duration = attr->duration ? attr->duration : 1 S;
	ls->times = ls->duration / ls->interval + 1;

	ls->count = 0;
	ls->index = 0;
	ls->data = lib_calloc(ls->times * sizeof(__int64));
	if(!ls->data)
		error_goto(limit_speed_init_error_2);

	gettimeofday(&ls->time_point, NULL);

	return ls;

limit_speed_init_error_2:
	lib_free(ls);
limit_speed_init_error_1:
	return NULL;
}

//速度校验，超速返回FALSE
inline int limit_speed_check(limit_speed *ls)
{
	struct timeval tv;
	__int64 interval;
	__int64 duration;
	int index;

	gettimeofday(&tv, NULL);
	interval = (tv.tv_sec - ls->time_point.tv_sec) S + (tv.tv_usec - ls->time_point.tv_usec) + 1; //防止为0

	if(interval > ls->interval)
	{
		ls->index = (ls->index + 1) % ls->times;

		ls->data[ls->index] = ls->count;

		ls->time_point.tv_usec += ls->interval;
		ls->time_point.tv_sec += ls->time_point.tv_usec TO(S);
		ls->time_point.tv_usec %= 1 S;
		interval -= ls->interval;
	}

	//最后一个数据是否为0
	if(ls->data[ls->times - 1])
	{
		index = (ls->index + 1) % ls->times;
		duration = ls->duration;
	}
	else
	{
		index = 0;
		duration = ls->interval * (ls->index - index);
	}

	//将除法换成乘法，提高效率
	if((ls->count - ls->data[index]) * ls->size * 1 S > ls->limit * (duration + interval))
		return FALSE;

	return TRUE;
}

inline void limit_speed_add(limit_speed *ls)
{
	ls->count++;
}

inline int limit_speed_destroy(limit_speed *ls)
{
	lib_free(ls->data);
	lib_free(ls);
}


#ifdef TEST

#include "test.h"

void limit_test_speed()
{
	limit_speed_attr attr =
	{
		.limit = 1 KB,
		.size = 1 KB
	};
	limit_speed *ls;

	ls = limit_speed_init(&attr);
	test_assert(limit_speed_check(ls), TRUE, 'd', TEST_EQUAL);
	limit_speed_add(ls);
	test_assert(limit_speed_check(ls), FALSE, 'd', TEST_EQUAL);
	limit_speed_add(ls);
	limit_speed_destroy(ls);
}

void limit_test()
{
	limit_test_speed();
}

#endif
