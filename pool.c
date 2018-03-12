#include "pool.h"


static pool *g_pool[POOL_MAX] = {0};

pool *pool_init(int num)
{
	pool *p;
	int i;

	p = lib_calloc(sizeof(pool), pool_destroy);
	if(!p)
		error_goto(pool_init_error_1);

	p->t = lib_calloc(num * sizeof(task));
	if(!p->t)
		error_goto(pool_init_error_2);

	p->num = num;

	for(i = 0; i < POOL_MAX; i++)
		if(!g_pool[i])
		{
			g_pool[i] = p;
			p->id = i;
			break;
		}
	if(i >= POOL_MAX)
		error_goto(pool_init_error_3);

	p->taskinfo = queue_init(0);
	if(!p->taskinfo)
		error_goto(pool_init_error_3);

	p->ti1 = queue_init(0);
	if(!p->ti1)
		error_goto(pool_init_error_4);

	p->ti2 = queue_init(0);
	if(!p->ti2)
		error_goto(pool_init_error_5);

	return p;

pool_init_error_5:
	queue_destroy(p->ti1);
pool_init_error_4:
	queue_destroy(p->taskinfo);
pool_init_error_3:
	lib_free(p->t);
pool_init_error_2:
	lib_free(p);
pool_init_error_1:
	return NULL;
}

//查找pool中空的task
task *pool_find(pool *p)
{
	int i;
	for(i = 0; i < p->num; i++)
		if(p->t[i].exist == FALSE)
			return p->t + i;
	return NULL;
}

//detach类型的线程开启后不用管后续
task *pool_start(pool *p, void *func, void *args)
{
	task *t;

	t = pool_find(p);
	if(!t)
		error_goto(pool_start_error_1);

	t->info = lib_calloc(sizeof(task_info));
	if(!t->info)
		error_goto(pool_start_error_1);

	t->pid = thread_create(&t->t, func, args);
	if(t->pid == 0)
		error_goto(pool_start_error_2);

	t->exist = TRUE;
	t->quit = FALSE;
	t->cancel = FALSE;
	t->pause = FALSE;
	t->detach = FALSE;

	queue_put(p->taskinfo, t->info, -1);

	__sync_lock_test_and_set(&t->info->state, TASK_RUNNING);

	pool_clear(p);

	return t;

pool_start_error_2:
	lib_free(t->info);
pool_start_error_1:
	return NULL;
}

inline int pool_func(pool *p, task *t, int (*func)(thread))
{
	if(t)
		func(t->t);
	else
	{
		int i;
		for(i = 0; i < p->num; i++)
		{
			t = p->t + i;
			if(t->exist == TRUE)
			{
				func(t->t);
				if(func == thread_join)
				{
					t->exist = FALSE;
					if(t->quit == FALSE)
						__sync_lock_test_and_set(&t->info->state, TASK_SUCCESS);
				}
				else if(func == thread_detach)
					t->detach = TRUE;
			}
		}
	}
}

//获取当前线程的句柄
task *pool_task()
{
	unsigned long pid;
	int i, j;

	pid = thread_id();
	for(i = 0; i < POOL_MAX; i++)
	{
		if(g_pool[i])
		{
			for(j = 0; j < g_pool[i]->num; j++)
			{
				if(g_pool[i]->t[j].exist == TRUE && g_pool[i]->t[j].pid == pid)
					return g_pool[i]->t + j;
			}
		}
	}
	return NULL;
}

int pool_exit(int state)
{
	task *t = pool_task();

	if(t)
	{
		__sync_lock_test_and_set(&t->info->state, state);
		t->quit = TRUE;
		if(t->detach)
			t->exist = FALSE;
	}
	thread_exit();
}

int pool_clear(pool *p)
{
	if(queue_size(p->taskinfo) <= TASKINFO_MAX)
		return FALSE;

	queue *ti1, *ti2;
	task_info *info;

	if(queue_empty(p->ti2) == TRUE)
	{
		ti1 = p->ti1;
		ti2 = p->ti2;
	}
	else
	{
		ti1 = p->ti2;
		ti2 = p->ti1;
	}

	while(queue_size(ti1) > 0)
	{
		if(queue_get(ti1, info, 1 S) == FALSE)
			break;
		if(info->state == TASK_FAILURE || info->state == TASK_SUCCESS)
			lib_free(info);
		else
			queue_put(ti2, info, -1);
	}

	while(queue_size(p->taskinfo) > TASKINFO_MAX)
	{
		if(queue_get(p->taskinfo, info, 1 S) == FALSE)
			break;
		if(info->state == TASK_FAILURE || info->state == TASK_SUCCESS)
			lib_free(info);
		else
			queue_put(ti2, info, -1);
	}
}

int pool_destroy(pool *p)
{
	task_info *info;
	g_pool[p->id] = NULL;

	while(queue_size(p->taskinfo) > 0)
		if(queue_get(p->taskinfo, info, 1 S) == TRUE)
			lib_free(info);

	while(queue_size(p->ti1) > 0)
		if(queue_get(p->ti1, info, 1 S) == TRUE)
			lib_free(info);

	while(queue_size(p->ti2) > 0)
		if(queue_get(p->ti2, info, 1 S) == TRUE)
			lib_free(info);

	queue_destroy(p->ti1);
	queue_destroy(p->ti2);
	queue_destroy(p->taskinfo);
	lib_free(p->t);
	lib_free(p);
}


//暂停时，程序会在执行处下一个task_step停下，恢复时继续开始
int task_progress(int progress, char *fmt, ...)
{
	task *t = pool_task();
	if(t)
	{
		t->info->progress += progress;
		if(t->info->progress > TASKINFO_PROGRESS)
			t->info->progress = TASKINFO_PROGRESS;

		va_list args;
		va_start(args, fmt);
		vsprintf(t->info->message, fmt, args);
		va_end(args);
	}
}

int task_suspend(task *t)
{
	if(t == NULL)
		t = pool_task();

	if(t->exist == TRUE)
	{
		__sync_lock_test_and_set(&t->pause, TRUE);
		__sync_lock_test_and_set(&t->info->state, TASK_PAUSE);
	}
}

int task_resume(task *t)
{
	if(t == NULL)
		t = pool_task();

	if(t->exist == TRUE)
	{
		__sync_lock_test_and_set(&t->pause, FALSE);
		__sync_lock_test_and_set(&t->info->state, TASK_RUNNING);
	}
}

int task_cancel(task *t)
{
	if(t == NULL)
		t = pool_task();

	if(t->exist == TRUE)
	{
		__sync_lock_test_and_set(&t->cancel, TRUE);
		__sync_lock_test_and_set(&t->pause, FALSE);
	}
}


#ifdef TEST

#include "test.h"

int pool_show(pool *p)
{
	tools_sleep(10 MS);
	task_info *info;
	queue_each(p->taskinfo, {
		info = val;
		printf("task state : %d\n", info->state);
		printf("task progress : %d\n", info->progress);
		printf("task message : %s\n", info->message);
	});
	printf("\n");
}

void pool_test_func()
{
	tools_sleep(20 MS);
	task_progress(30, "abc");
	task_step({});
	tools_sleep(20 MS);
	task_progress(30, "def");
	task_step({});
}

void pool_test()
{
	pool *p;
	task *t;
	task_info *info;

	p = pool_init(5);
	t = pool_start(p, pool_test_func, NULL);
	info = p->taskinfo->list->head->data;
	test_assert(pool_alive(p, t), TRUE, 'd', TEST_EQUAL);

	test_assert(info->state, TASK_RUNNING, 'd', TEST_EQUAL);
	test_assert(info->progress, 0, 'd', TEST_EQUAL);
	test_assert(info->message, "", 's', TEST_EQUAL);

	task_suspend(t);
	tools_sleep(50 MS);
	test_assert(info->state, TASK_PAUSE, 'd', TEST_EQUAL);
	test_assert(info->progress, 30, 'd', TEST_EQUAL);
	test_assert(info->message, "abc", 's', TEST_EQUAL);

	task_resume(t);
	test_assert(info->state, TASK_RUNNING, 'd', TEST_EQUAL);
	test_assert(info->progress, 30, 'd', TEST_EQUAL);
	test_assert(info->message, "abc", 's', TEST_EQUAL);

	pool_join(p, NULL);
	test_assert(info->state, TASK_SUCCESS, 'd', TEST_EQUAL);
	test_assert(info->progress, 60, 'd', TEST_EQUAL);
	test_assert(info->message, "def", 's', TEST_EQUAL);

	test_assert(pool_alive(p, t), FALSE, 'd', TEST_EQUAL);
	pool_destroy(p);
}

#endif
