#ifndef LIMIT_H
#define LIMIT_H

#include "head.h"
#include "lib.h"
#include "error.h"


/*
该接口为等大小数据包的计数
limit, size为必须参数，interval, duration为可选参数，interval默认为100MS，duration默认为1S
*/
typedef struct limit_speed_attr
{
	__int64 limit; //限速大小
	int size; //一次计数的大小
	__int64 interval; //计数间隔
	__int64 duration; //计数时间
} limit_speed_attr;

typedef struct limit_speed
{
	__int64 limit;
	int size; //一次计数的大小
	__int64 interval; //计数间隔
	__int64 duration; //计数时间
	int times; //计数次数

	__int64 count; //总计数

	int index;
	__int64 *data;

	struct timeval time_point; 
} limit_speed;

inline limit_speed *limit_speed_init(limit_speed_attr *attr);

inline int limit_speed_check(limit_speed *ls);

inline void limit_speed_add(limit_speed *ls);

inline int limit_speed_destroy(limit_speed *ls);


#endif
