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

//�̳߳��������
#define POOL_MAX									128

#define TASK_RUNNING								0
#define TASK_PAUSE									1
#define TASK_CANCEL									2
#define TASK_FAILURE								3
#define TASK_SUCCESS								4

//������Ϣ�������
#define TASKINFO_MAX								1024
//���ȵ����ֵ
#define TASKINFO_PROGRESS							100

typedef struct task_info
{
	int state; //״̬
	int progress; //����
	char message[INFO_SIZE]; //��Ϣ
} task_info;

typedef struct task
{
	char exist; //�Ƿ����
	char quit; //�Ƿ��˳�
	char cancel; //�Ƿ�ȡ��
	char pause; //�Ƿ���ͣ
	char detach; //�Ƿ����
	unsigned long pid;
	thread t;
	task_info *info;
} task;

typedef struct pool
{
	int id;
	task *t;
	int num;
	queue *taskinfo; //������Ϣ����
	queue *ti1, *ti2; //����������Ϣ����ʱ��������δ������������Ϣ
} pool;

pool *pool_init(int num);

task *pool_start(pool *p, void *func, void *args);

inline int pool_func(pool *p, task *t, int (*func)(thread));

#define pool_alive(p, t) pool_func(p, t, thread_alive)

#define pool_suspend(p, t) pool_func(p, t, thread_suspend)

#define pool_resume(p, t) pool_func(p, t, thread_resume)
//��������ڴ�й©�������������
#define pool_kill(p, t) pool_func(p, t, thread_kill)

#define pool_join(p, t) pool_func(p, t, thread_join)

#define pool_detach(p, t) pool_func(p, t, thread_detach)

//���߳��л�ȡ�̵߳ľ��
task *pool_task();

int pool_exit(int state);

int pool_destroy(pool *p);

/*
�ֶ������̵߳���ͣ��ʼ
�����ʹ��pool_detach��֮����pool_join���߳̽�����ʹ��pool_exit�˳�
������ʹ��pool_join

task_step���öϵ㲢�����߳̽��ȣ��ϵ������ͣ���˳�
pool_exit�˳�ʱ����ΪTASK_FAILURE��TASK_SUCCESS
�����̷߳�����Ĭ��ΪTASK_SUCCESS
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
