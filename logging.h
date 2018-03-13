#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>
#include "config.h"
#include "dict.h"
#include "tools.h"

#define LOGGING_NOTSET									0
#define LOGGING_DEBUG									1
#define LOGGING_INFO 									2
#define LOGGING_WARNING								3
#define LOGGING_ERROR									4
#define LOGGING_CRITICAL								5

static const char *LOGGING_LEVEL[] =
{
	"NOTSET",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"CRITICAL"
};

/*
日志打印时，将所有错误等级的日志保存在一定大小的共享内存队列中，当有数据从队列中移出时，判断日志打印等级确定该日志是否需要打印。当出现错误时（LOGGING_ERROR，LOGGING_CRITICAL），降低打印等级到LOGGING_NOTSET使所有日志全部打印。直到连续若干条日志记录没有错误后，恢复日志等级。
当进程退出时，将队列中的日志全部写入文件。
可提供参数如print-all等，再次运行程序./zx013.exe --print-all将所有日志打印
print-immediately，所有产生的日志立即打印
同时也可以根据其他参数在外部动态修改日志打印配置
*/

void logging_init(char *filename);

#define logging(message, level, ...) _logging(message, __FILE__, __FUNCTION__, __LINE__, level, ##__VA_ARGS__, NULL)

void _logging(char *message, const char *file, const char *func, const int line, int level, ...);


#endif
