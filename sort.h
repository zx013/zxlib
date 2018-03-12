#ifndef SORT_H
#define SORT_H

#include "tools.h"
#include "klib/ksort.h"

#define __sort(array, count, block) \
	{ \
		typedef typeof(array[0]) sort_##count; \
		inline int sort_cmp_##count(sort_##count a, sort_##count b) \
		block; \
		KSORT_INIT_INTROSORT(sort_##count, sort_##count, sort_cmp_##count) \
		ks_introsort(sort_##count, tools_size(array), array); \
	}

//把__COUNTER__宏展开
#define _sort(array, count, block) __sort(array, count, block)

//每用一次编译生成的文件大小增加0.2K，循环中只计算一次
//type_t为参数类型，array为数组，n为长度，block为比较函数，参数默认为a, b
#define sort(array, block) _sort(array, __COUNTER__, block)


#endif
