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

//��__COUNTER__��չ��
#define _sort(array, count, block) __sort(array, count, block)

//ÿ��һ�α������ɵ��ļ���С����0.2K��ѭ����ֻ����һ��
//type_tΪ�������ͣ�arrayΪ���飬nΪ���ȣ�blockΪ�ȽϺ���������Ĭ��Ϊa, b
#define sort(array, block) _sort(array, __COUNTER__, block)


#endif
