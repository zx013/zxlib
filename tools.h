#ifndef TOOLS_H
#define TOOLS_H

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <sched.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "head.h"
#include "lib.h"


//获取时间字符串，传入参数为数组
#define tools_datetime(buffer) _tools_datetime(buffer, sizeof(buffer), 0)

//获取一段时间前的时间字符串
#define tools_datetime_ago(buffer, interval) _tools_datetime(buffer, sizeof(buffer), interval)

//用循环实现的暂停，单位默认为微秒，只用于极短时间的暂停
inline void tools_fast_sleep(__int64 t);

//暂停，单位默认为微秒
#ifdef _WIN32
#define tools_sleep(t) Sleep((t) TO(MS));
#else
#define tools_sleep(t) usleep(t);
#endif

#ifdef _WIN32
#define tools_yield() SwitchToThread()
#else
//#define tools_yield() sched_yield()
#define tools_yield() tools_sleep(0)
#endif

//计算hash值
inline int tools_hash(char *s);

//获取文件状态
void tools_stat(char *filepath, struct stat *st);

//判断文件是否为目录
int tools_isdir(char *filepath);

//获取分配的指针（lib_malloc分配）或数组（数组大小不能为sizeof(void *)中的元素个数
//元素当作参数传递时，不能改变类型（即数组传递为指针）
//#define tools_size(ptr) (__builtin_types_compatible_p(typeof(ptr), typeof((ptr)[0]) *) ? (*(((int *)(ptr)) - 1) / sizeof((ptr)[0])) : (sizeof(ptr) / sizeof((ptr)[0])))
#define tools_size(ptr) \
	({ \
		lib_info *__ptr = lib_malloc_info(ptr); \
		(__ptr ? __ptr->size : sizeof(ptr)) / sizeof((ptr)[0]); \
	})


#endif
