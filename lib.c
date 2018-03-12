#include "lib.h"


inline void *__cast(int i, ...)
{
	va_list ap;
	union
	{
		void *v;
		double d;
	} u;

	va_start(ap, i);
	u.d = (double)va_arg(ap, double);
	va_end(ap);

	return u.v;
}

inline double __castdouble(int i, ...)
{
	va_list ap;
	union
	{
		void *v;
		double d;
	} u;

	va_start(ap, i);
	u.v = (void *)va_arg(ap, void *);
	va_end(ap);

	return u.d;
}


static lib_global *global = NULL;

#ifdef TEST

__int64 lib_memory_total()
{
	return global->memory_total;
}

__int64 lib_memory_time()
{
	return global->memory_time;
}
#endif

static lib_page *libmap = NULL;
static char liblock = 0;

static inline int lib_page_insert(void *ptr)
{
	lib_page *__libmap;
	lib_dpage *__libdmap;
	int mask;
	int i;

	ptr = (void *)(((unsigned __int64)ptr) >> LIB_PAGE_OFFSET);

	//仅仅在未创建时进入加锁状态，提高速度
	if(unlikely(!libmap))
	{
		lock(liblock);
		if(likely(!libmap))
			libmap = calloc(1, sizeof(lib_page));
		unlock(liblock);
	}

	__libmap = libmap;
	for(i = 0; i < LIB_PAGE_NUM - 1; i++)
	{
		mask = LIB_PAGE_MASK(ptr, i);
		if(unlikely(!__libmap->map[mask]))
		{
			lock(__libmap->lock);
			if(likely(!__libmap->map[mask]))
				__libmap->map[mask] = calloc(1, sizeof(lib_page));
			unlock(__libmap->lock);
		}
		//__libmap->num++;
		__libmap = (lib_page *)__libmap->map[mask];
	}
	mask = LIB_PAGE_MASK(ptr, i);
	if(!__libmap->map[mask])
	{
		lock(__libmap->lock);
		if(likely(!__libmap->map[mask]))
			__libmap->map[mask] = calloc(1, sizeof(lib_dpage));
		unlock(__libmap->lock);
	}
	//__libmap->num++;
	__libdmap = (lib_dpage *)__libmap->map[mask];
	//__libdmap->num++;

	mask = LIB_PAGE_DMASK(ptr);
	__sync_fetch_and_or(&__libdmap->map[mask >> 3], 1 << (mask & 0x7));
}

static inline int lib_page_select(void *ptr)
{
	lib_page *__libmap;
	lib_dpage *__libdmap;
	int mask;
	int i;

	if(unlikely((__int64)ptr == -1))
		return FALSE;

	ptr = (void *)(((unsigned __int64)ptr) >> LIB_PAGE_OFFSET);

	if(unlikely(!libmap))
		return FALSE;

	__libmap = libmap;
	for(i = 0; i < LIB_PAGE_NUM - 1; i++)
	{
		mask = LIB_PAGE_MASK(ptr, i);
		if(unlikely(!__libmap->map[mask]))
			return FALSE;
		__libmap = (lib_page *)__libmap->map[mask];
	}
	mask = LIB_PAGE_MASK(ptr, i);
	if(!__libmap->map[mask])
		return FALSE;
	__libdmap = (lib_dpage *)__libmap->map[mask];

	mask = LIB_PAGE_DMASK(ptr);
	if((__int64)__libdmap->map[mask >> 3] & (1 << (mask & 0x7)))
		return TRUE;
	return FALSE;
}

static inline int lib_page_delete(void *ptr)
{
	lib_page *__libmap;
	lib_dpage *__libdmap;
	int mask;
	int i;

	ptr = (void *)(((unsigned __int64)ptr) >> LIB_PAGE_OFFSET);

	if(unlikely(!libmap))
		return FALSE;

	__libmap = libmap;
	for(i = 0; i < LIB_PAGE_NUM - 1; i++)
	{
		mask = LIB_PAGE_MASK(ptr, i);
		if(unlikely(!__libmap->map[mask]))
			return FALSE;
		//__libmap->num--;
		__libmap = (lib_page *)__libmap->map[mask];
	}
	mask = LIB_PAGE_MASK(ptr, i);
	if(!__libmap->map[mask])
		return FALSE;
	//__libmap->num--;
	__libdmap = (lib_dpage *)__libmap->map[mask];
	//__libdmap->num--;

	mask = LIB_PAGE_DMASK(ptr);
	__sync_fetch_and_and(&__libdmap->map[mask >> 3], ~(1 << (mask & 0x7)));
	return FALSE;
}


inline void *lib_malloc_1(int size)
{
	lib_malloc_2(size, lib_free);
}

inline void *lib_malloc_2(int size, void *free)
{
	lib_malloc_3(size, free, FALSE);
}

//分配空间时可设置释放函数，释放空间时用释放函数来释放
inline void *lib_malloc_3(int size, void *free, int reset)
{
	if(unlikely(!global))
	{
		global = malloc(sizeof(lib_global));
#ifdef TEST
		__sync_lock_test_and_set(&global->memory_total, 0);
		__sync_lock_test_and_set(&global->memory_time, 0);
#endif
	}

	lib_info *ptr;
	if(reset == TRUE)
		ptr = calloc(1, size + sizeof(lib_info));
	else
		ptr = malloc(size + sizeof(lib_info));

	if(unlikely(!ptr))
		return NULL;

#ifdef TEST
	__sync_fetch_and_add(&global->memory_total, size);
	__sync_fetch_and_add(&global->memory_time, 1);
#endif

	ptr->lock = 0;
	ptr->flag = 1;
	ptr->size = size;
	ptr->free = free;

	lib_page_insert(ptr);

	return ((lib_info *)ptr) + 1;
}

inline void *lib_calloc_1(int size)
{
	lib_calloc_2(size, lib_free);
}

inline void *lib_calloc_2(int size, void *free)
{
	lib_malloc_3(size, free, TRUE);
}

inline void *lib_realloc_2(void *ptr, int size)
{
	return lib_realloc_3(ptr, size, lib_free);
}

inline void *lib_realloc_3(void *ptr, int size, void *free)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;
	lib_info *__nptr;
	int __size;

	if(unlikely(!ptr))
		return lib_malloc(size, free);

	__size = __ptr->size;
	__nptr = realloc(__ptr, size + sizeof(lib_info));
	if(unlikely(!__nptr))
		return NULL;

#ifdef TEST
	__sync_fetch_and_add(&global->memory_total, size - __size);
#endif

	__nptr->size = size;

	if(__nptr == __ptr)
		return ((lib_info *)__nptr) + 1;

	lib_page_delete(__ptr);
	lib_page_insert(__nptr);

	return ((lib_info *)__nptr) + 1;
}

inline void __lib_free(void *ptr)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;

#ifdef TEST
	__sync_fetch_and_sub(&global->memory_total, __ptr->size);
	__sync_fetch_and_sub(&global->memory_time, 1);
#endif

	lib_page_delete(__ptr);

	free(__ptr);
}

inline void lib_free(void *ptr)
{
	lib_flag_sub(ptr);
}

//分配时可注册释放函数，释放函数需调用lib_free释放，则使用lib_autofree时，自动使用注册的释放函数进行释放。可通用的处理不同数据类型的释放问题
inline void lib_autofree(void *ptr)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;
	if(lib_page_select(__ptr) == TRUE)
		__ptr->free(ptr);
}

inline void lib_flag_add(void *ptr)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;
	if(lib_page_select(__ptr) == FALSE)
		return;

	lock(__ptr->lock);
	__ptr->flag++;
	unlock(__ptr->lock);
}

inline void lib_flag_sub(void *ptr)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;
	if(lib_page_select(__ptr) == FALSE)
		return;

	lock(__ptr->lock);
	__ptr->flag--;
	if(__ptr->flag <= 0)
	{
		unlock(__ptr->lock);
		__lib_free(ptr);
	}
	else
		unlock(__ptr->lock);
}

inline lib_info *lib_malloc_info(void *ptr)
{
	lib_info *__ptr = ((lib_info *)ptr) - 1;
	return lib_page_select(__ptr) == TRUE ? __ptr : NULL;
}

inline char *lib_strdup(char *s)
{
	char *t;
	int l;

	l = strlen(s);
	t = lib_malloc(l + 1);
	if(t)
	{
		strncpy(t, s, l);
		t[l] = 0;
	}

	return t;
}


inline void *lib_fastmalloc(int size)
{
}

inline void *lib_fastfree(void *ptr)
{
}

#ifdef TEST

#include "test.h"

void lib_page_test()
{
	void *ptr;

	ptr = (void *)0x123456789abcdef0;

	int i;
	for(i = 0; i < sizeof(void *) << 3; i++)
	{
		lshift(ptr);
		test_assert(lib_page_select(ptr), FALSE);
	}

	for(i = 0; i < sizeof(void *) << 3; i++)
	{
		lshift(ptr);
		lib_page_insert(ptr);
	}

	for(i = 0; i < sizeof(void *) << 3; i++)
	{
		lshift(ptr);
		test_assert(lib_page_select(ptr), TRUE);
	}

	for(i = 0; i < sizeof(void *) << 3; i++)
	{
		lshift(ptr);
		lib_page_delete(ptr);
	}

	for(i = 0; i < sizeof(void *) << 3; i++)
	{
		lshift(ptr);
		test_assert(lib_page_select(ptr), FALSE);
	}


	for(i = 0; i < (1 << 8); i++)
	{
		ptr = (void *)(i << 8);
		test_assert(lib_page_select(ptr), FALSE);
	}

	for(i = 0; i < (1 << 8); i++)
	{
		ptr = (void *)(i << 8);
		lib_page_insert(ptr);
	}

	for(i = 0; i < (1 << 8); i++)
	{
		ptr = (void *)(i << 8);
		test_assert(lib_page_select(ptr), TRUE);
	}

	for(i = 0; i < (1 << 8); i++)
	{
		ptr = (void *)(i << 8);
		lib_page_delete(ptr);
	}

	for(i = 0; i < (1 << 8); i++)
	{
		ptr = (void *)(i << 8);
		test_assert(lib_page_select(ptr), FALSE);
	}
}

void lib_malloc_test()
{
	void *ptr;
	int i;

	for(i = 0; i < 1000; i++)
	{
		test_assert(lib_malloc_info(ptr), NULL, 'p');
		ptr = lib_malloc(i);
		test_assert(lib_malloc_info(ptr), NULL, 'p', TEST_UNEQUAL);
		lib_free(ptr);
		test_assert(lib_malloc_info(ptr), NULL, 'p');
	}
}

void lib_flag_test()
{
	void *ptr;
	int i;

	test_assert(lib_malloc_info(ptr), NULL, 'p');

	ptr = lib_malloc(1);
	test_assert(lib_malloc_info(ptr), NULL, 'p', TEST_UNEQUAL);

	for(i = 0; i < 100; i++)
	{
		lib_flag_add(ptr);
		test_assert(lib_malloc_info(ptr), NULL, 'p', TEST_UNEQUAL);
	}

	for(i = 0; i < 100; i++)
	{
		lib_flag_sub(ptr);
		test_assert(lib_malloc_info(ptr), NULL, 'p', TEST_UNEQUAL);
	}

	lib_free(ptr);
	test_assert(lib_malloc_info(ptr), NULL, 'p');
}

void lib_test()
{
	lib_page_test();
	lib_malloc_test();
	lib_flag_test();
}

#endif
