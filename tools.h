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


//��ȡʱ���ַ������������Ϊ����
#define tools_datetime(buffer) _tools_datetime(buffer, sizeof(buffer), 0)

//��ȡһ��ʱ��ǰ��ʱ���ַ���
#define tools_datetime_ago(buffer, interval) _tools_datetime(buffer, sizeof(buffer), interval)

//��ѭ��ʵ�ֵ���ͣ����λĬ��Ϊ΢�룬ֻ���ڼ���ʱ�����ͣ
inline void tools_fast_sleep(__int64 t);

//��ͣ����λĬ��Ϊ΢��
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

//����hashֵ
inline int tools_hash(char *s);

//��ȡ�ļ�״̬
void tools_stat(char *filepath, struct stat *st);

//�ж��ļ��Ƿ�ΪĿ¼
int tools_isdir(char *filepath);

//��ȡ�����ָ�루lib_malloc���䣩�����飨�����С����Ϊsizeof(void *)�е�Ԫ�ظ���
//Ԫ�ص�����������ʱ�����ܸı����ͣ������鴫��Ϊָ�룩
//#define tools_size(ptr) (__builtin_types_compatible_p(typeof(ptr), typeof((ptr)[0]) *) ? (*(((int *)(ptr)) - 1) / sizeof((ptr)[0])) : (sizeof(ptr) / sizeof((ptr)[0])))
#define tools_size(ptr) \
	({ \
		lib_info *__ptr = lib_malloc_info(ptr); \
		(__ptr ? __ptr->size : sizeof(ptr)) / sizeof((ptr)[0]); \
	})


#endif
