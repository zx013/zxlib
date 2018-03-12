#include "queue.h"


//max<=0时，队列没有上限
queue *queue_init(int max)
{
	queue *q;
	q = (queue *)lib_calloc(sizeof(queue), queue_destroy);
	if(!q)
		error_goto(queue_init_error_1);

	q->max = max;
	q->lock = 0;

	q->list = list_new();
	if(!q->list)
		error_goto(queue_init_error_2);

	return q;

queue_init_error_2:
	lib_free(q);
queue_init_error_1:
	return NULL;
}

int queue_put(queue *q, void *data, __int64 tm)
{
	if(lock(q->lock, q->max <= 0 || q->max > 0 && q->list->size < q->max, tm) == FALSE)
		return FALSE;

	list_push(q->list, data);

	unlock(q->lock);
	return TRUE;
}

int _queue_get(queue *q, void **data, __int64 tm)
{
	if(lock(q->lock, q->list->size > 0, tm) == FALSE)
		return FALSE;

	*data = list_pop(q->list);

	unlock(q->lock);
	return TRUE;
}
#define queue_get(q, s, t) _queue_get(q, (void *)&s, t)

int queue_empty(queue *q)
{
	int empty;
	lock(q->lock, TRUE, 0);
	empty = q->list->size > 0 ? FALSE : TRUE;
	unlock(q->lock);
	return empty;
}

inline int queue_size(queue *q)
{
	return q->list->size;
}

void queue_destroy(queue *q)
{
	list_free(q->list);
	lib_free(q);
}


#ifdef TEST

#include "pool.h"
#include "test.h"

static queue *queue_test_q;

void queue_func11()
{
	int *num;
	int i;
	for(i = 0; i < 5; i++)
	{
		num = (int *)lib_calloc(sizeof(int));
		*num = i;
		queue_put(queue_test_q, num, -1);
	}
}

void queue_func12()
{
	int *num;
	int i;
	for(i = 0; i < 5; i++)
	{
		queue_get(queue_test_q, num, -1);
		test_assert(*num, i, 'd', TEST_EQUAL);
		lib_free(num);
	}
}

void queue_test_1()
{
	pool *p;
	p = pool_init(2);
	pool_start(p, queue_func12, NULL);
	tools_sleep(100 MS);
	pool_start(p, queue_func11, NULL);
	pool_join(p, NULL);
	pool_destroy(p);
}

void queue_test_2()
{
	int *num;
	int i;

	queue_get(queue_test_q, num, 1 S); //1 sec

	for(i = 0; i < 5; i++)
	{
		queue_put(queue_test_q, num, 1 S);
		queue_get(queue_test_q, num, 1 S);
	}

	queue_put(queue_test_q, num, 1 S);
	queue_put(queue_test_q, num, 1 S); //1 sec
}

void queue_test_3()
{
	int *num;
	int i;

	//大约1.5s
	for(i = 0; i < 1000 * 10000; i++)
	{
		queue_put(queue_test_q, num, 1 S);
		queue_get(queue_test_q, num, 1 S);
	}
}

void queue_test()
{
	queue_test_q = queue_init(1);
	queue_test_1();
	queue_test_2();
	queue_test_3();
	queue_destroy(queue_test_q);
}

#endif
