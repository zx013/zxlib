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
��־��ӡʱ�������д���ȼ�����־������һ����С�Ĺ����ڴ�����У��������ݴӶ������Ƴ�ʱ���ж���־��ӡ�ȼ�ȷ������־�Ƿ���Ҫ��ӡ�������ִ���ʱ��LOGGING_ERROR��LOGGING_CRITICAL�������ʹ�ӡ�ȼ���LOGGING_NOTSETʹ������־ȫ����ӡ��ֱ��������������־��¼û�д���󣬻ָ���־�ȼ���
�������˳�ʱ���������е���־ȫ��д���ļ���
���ṩ������print-all�ȣ��ٴ����г���./zx013.exe --print-all��������־��ӡ
print-immediately�����в�������־������ӡ
ͬʱҲ���Ը��������������ⲿ��̬�޸���־��ӡ����
*/

void logging_init(char *filename);

#define logging(message, level, ...) _logging(message, __FILE__, __FUNCTION__, __LINE__, level, ##__VA_ARGS__, NULL)

void _logging(char *message, const char *file, const char *func, const int line, int level, ...);


#endif
