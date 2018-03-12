#ifndef LIB_H
#define LIB_H

#include <stdlib.h>
#include <stdarg.h>
#include "head.h"
#include "lock.h"


//编译优化
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

//循环左移
#define lshift(...) __VA_FUNC__(lshift, __VA_ARGS__)
#define lshift_1(__ptr) lshift_2(__ptr, 1)
#define lshift_2(__ptr, __num) ({__ptr = (typeof(__ptr))(((unsigned __int64)__ptr << __num) | ((unsigned __int64)__ptr >> ((sizeof(void *) << 3) - __num)));})
//循环右移
#define rshift(...) __VA_FUNC__(rshift, __VA_ARGS__)
#define rshift_1(__ptr) rshift_2(__ptr, 1)
#define rshift_2(__ptr, __num) ({__ptr = (typeof(__ptr))(((unsigned __int64)__ptr >> __num) | ((unsigned __int64)__ptr << ((sizeof(void *) << 3) - __num)));})

//lambda
#define lambda(...) __VA_FUNC__(lambda, __VA_ARGS__)

#define lambda_1(__body) lambda_2((), __body)

#define lambda_2(__args, __body) lambda_3(int, __args, __body)

#define lambda_3(__type, __args, __body) \
	({ \
		__type __lambda __args \
		{ \
			__body; \
			return (__type)TRUE; \
		} \
		__lambda; \
	})

//次类型的偏移大小
#define IS_OFFSET						4

//是否是整数
#define is_int64(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), char) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), signed char) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), unsigned char) ? 3 : \
		__builtin_types_compatible_p(typeof(__data), short) ? 4 : \
		__builtin_types_compatible_p(typeof(__data), unsigned short) ? 5 : \
		__builtin_types_compatible_p(typeof(__data), int) ? 6 : \
		__builtin_types_compatible_p(typeof(__data), unsigned int) ? 7 : \
		__builtin_types_compatible_p(typeof(__data), long) ? 8 : \
		__builtin_types_compatible_p(typeof(__data), unsigned long) ? 9 :\
		__builtin_types_compatible_p(typeof(__data), __int64) ? 10 : \
		__builtin_types_compatible_p(typeof(__data), unsigned __int64) ? 11 : \
		0) << IS_OFFSET; \
	})

//是否是浮点数
#define is_double(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), float) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), double) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), long double) ? 3 : \
		0) << IS_OFFSET; \
	})

//是否是字符串
#define is_str(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), char *) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), char []) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), const char *) ? 3 : \
		__builtin_types_compatible_p(typeof(__data), const char []) ? 4 : \
		0) << IS_OFFSET; \
	})

//获取主类型
#define major_type(__type) (__type & (((__int64)1 << IS_OFFSET) - 1))

//获取次类型
#define minor_type(__type) (__type >> IS_OFFSET)

//将类型转换为void *
inline void *__cast(int i, ...);
#define cast(__data) (is_double(__data) ? __cast(0, __data) : (void *)(__int64)(__data))

inline double __castdouble(int i, ...);
#define castdouble(__data) __castdouble(0, __data)


#ifdef TEST
__int64 lib_memory_total();
__int64 lib_memory_time();
#endif

//类似多级页表，用来记录是否是lib_malloc分配的数据

//因为malloc对齐，可以减少位数，6会出现问题
#define LIB_PAGE_OFFSET			5

#define LIB_PAGE_SIZE			((sizeof(void *) << 3) - LIB_PAGE_OFFSET)
#define LIB_PAGE_BIT			11
#define LIB_PAGE_NUM			(LIB_PAGE_SIZE / LIB_PAGE_BIT - 1)
#define LIB_PAGE_MASK(p, x)		(((__int64)(p) >> ((LIB_PAGE_NUM - (x) - 1) * LIB_PAGE_BIT + LIB_PAGE_DBIT)) & (((__int64)1 << LIB_PAGE_BIT) - 1))
#define LIB_PAGE_DBIT			(LIB_PAGE_SIZE - LIB_PAGE_NUM * LIB_PAGE_BIT)
#define LIB_PAGE_DMASK(p)		((__int64)(p) & (((__int64)1 << LIB_PAGE_DBIT) - 1))

//增加num会增加5%-10%左右的开销
typedef struct lib_page
{
	char lock;
	//int num;
	void **map[1 << LIB_PAGE_BIT];
} lib_page;

typedef struct lib_dpage
{
	char lock;
	//int num;
	char *map[1 << (LIB_PAGE_DBIT - 3)];
} lib_dpage;

static inline int lib_page_insert(void *ptr);
static inline int lib_page_select(void *ptr);
static inline int lib_page_delete(void *ptr);


//1000W次操作时间为0.55-0.60s，比malloc和free提高了一倍左右的时间
typedef struct lib_global
{
#ifdef TEST
	__int64 memory_total;
	__int64 memory_time;
#endif
} lib_global;

typedef struct lib_info
{
	char lock; //原子锁
	char flag; //引用计数
	int size; //大小
	void (*free)(void *); //释放指针
} lib_info;

#define lib_malloc(...) __VA_FUNC__(lib_malloc, __VA_ARGS__)
inline void *lib_malloc_1(int size);
inline void *lib_malloc_2(int size, void *free);
inline void *lib_malloc_3(int size, void *free, int reset);

#define lib_calloc(...) __VA_FUNC__(lib_calloc, __VA_ARGS__)
inline void *lib_calloc_1(int size);
inline void *lib_calloc_2(int size, void *free);

#define lib_realloc(...) __VA_FUNC__(lib_realloc, __VA_ARGS__)
inline void *lib_realloc_2(void *ptr, int size);
inline void *lib_realloc_3(void *ptr, int size, void *free);

inline void lib_free(void *ptr);

inline void lib_autofree(void *ptr);

//增加引用计数
inline void lib_flag_add(void *ptr);

//减少引用计数
inline void lib_flag_sub(void *ptr);

inline lib_info *lib_malloc_info(void *ptr);

inline char *lib_strdup(char *s);


#endif
