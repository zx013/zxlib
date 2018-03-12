#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lib.h"
#include "thread.h"
#include "queue.h"
#include "error.h"

//线程池最大数量
#define POOL_MAX									128

#define TASK_RUNNING								0
#define TASK_PAUSE									1
#define TASK_CANCEL									2
#define TASK_FAILURE								3
#define TASK_SUCCESS								4

//任务信息最大数据
#define TASKINFO_MAX								1024
//进度的最大值
#define TASKINFO_PROGRESS							100

typedef struct task_info
{
	int state; //状态
	int progress; //进度
	char message[INFO_SIZE]; //信息
} task_info;

typedef struct task
{
	char exist; //是否存在
	char quit; //是否退出
	char cancel; //是否取消
	char pause; //是否暂停
	char detach; //是否分离
	unsigned long pid;
	thread t;
	task_info *info;
} task;

typedef struct pool
{
	int id;
	task *t;
	int num;
	queue *taskinfo; //任务信息队列
	queue *ti1, *ti2; //清理任务信息队列时，缓存尚未结束的任务信息
} pool;

pool *pool_init(int num);

task *pool_start(pool *p, void *func, void *args);

inline int pool_func(pool *p, task *t, int (*func)(thread));

#define pool_alive(p, t) pool_func(p, t, thread_alive)

#define pool_suspend(p, t) pool_func(p, t, thread_suspend)

#define pool_resume(p, t) pool_func(p, t, thread_resume)
//可能造成内存泄漏或者死锁等情况
#define pool_kill(p, t) pool_func(p, t, thread_kill)

#define pool_join(p, t) pool_func(p, t, thread_join)

#define pool_detach(p, t) pool_func(p, t, thread_detach)

//在线程中获取线程的句柄
task *pool_task();

int pool_exit(int state);

int pool_destroy(pool *p);

/*
手动控制线程的暂停开始
分离的使用pool_detach，之后不用pool_join，线程结束需使用pool_exit退出
正常的使用pool_join

task_step设置断点并更新线程进度，断点可以暂停或退出
pool_exit退出时参数为TASK_FAILURE或TASK_SUCCESS
正常线程返回是默认为TASK_SUCCESS
*/
int task_progress(int progress, char *fmt, ...);

#define task_step(block) \
	do \
	{ \
		task *t = pool_task(); \
		if(t) \
		{ \
			while(__sync_bool_compare_and_swap(&t->pause, TRUE, TRUE)) \
				tools_yield(); \
			if(__sync_bool_compare_and_swap(&t->cancel, TRUE, TRUE)) \
			{ \
				block; \
				pool_exit(TASK_CANCEL); \
			} \
		} \
	} while(0)

int task_suspend(task *t);

int task_resume(task *t);

int task_cancel(task *t);


#endif
