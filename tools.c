#include "tools.h"


__attribute__((constructor)) void tools_init()
{
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_sec + tv.tv_usec);
}

void _tools_datetime(char *buffer, int size, int interval)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	rawtime -= interval;
	timeinfo = localtime(&rawtime);
	strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

#define tools_datetime(buffer) _tools_datetime(buffer, sizeof(buffer), 0)

#define tools_datetime_ago(buffer, interval) _tools_datetime(buffer, sizeof(buffer), interval)

//用循环来进行微秒级的定时，少数情况下会有较大误差（可能是线程调度的原因），大多数情况下误差在2-3微秒之内
//只支持linux，windows下最小精确到毫秒
//误差在10%之内
inline void tools_fast_sleep(__int64 t)
{
	static struct timeval tv_s, tv_e;
	gettimeofday(&tv_s, NULL);
	while(1)
	{
		gettimeofday(&tv_e, NULL);
		//1微秒是代码运行时间
		if((tv_e.tv_sec - tv_s.tv_sec) * 1000 * 1000 + (tv_e.tv_usec - tv_s.tv_usec) > t - 1)
			break;
	}
}

inline int tools_hash(char *s)
{
	int hash = 5381;

	while(*s++)
		hash = ((hash << 5) + hash) + (*s);
	return hash;
}


void tools_stat(char *filepath, struct stat *st)
{
#ifdef _WIN32
	stat(filepath, st);
#else
	lstat(filepath, st);
#endif
}

int tools_isdir(char *filepath)
{
	struct stat st;
	tools_stat(filepath, &st);
	if(S_ISDIR(st.st_mode))
		return TRUE;
	return FALSE;
}


#ifdef TEST

#include "test.h"

void tools_test()
{
	char *ptr1;
	ptr1 = lib_calloc(1 * sizeof(char));
	test_assert(tools_size(ptr1), 1, 'd', TEST_EQUAL);
	lib_free(ptr1);

	int *ptr2;
	ptr2 = lib_calloc(2 * sizeof(int));
	test_assert(tools_size(ptr2), 2, 'd', TEST_EQUAL);
	lib_free(ptr2);
	
	char ptr3[3];
	test_assert(tools_size(ptr3), 3, 'd', TEST_EQUAL);

	int ptr4[4];
	test_assert(tools_size(ptr4), 4, 'd', TEST_EQUAL);
}

#endif
