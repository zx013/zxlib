#include "transqueue.h"


transqueue *transqueue_open(transqueue_attr *attr)
{
	transqueue *tq;

	tq = (transqueue *)lib_calloc(sizeof(transqueue));
	if(!tq)
		error_goto(transqueue_open_error_1);

	tq->side = attr->side;
	transfer_attr attr_t =
	{
		.side = tq->side,
		.ip = attr->ip,
		.port = attr->port
	};

	tq->t = transfer_open(&attr_t);
	if(!tq->t)
		error_goto(transqueue_open_error_2);

	tq->fq = queue_init(0);
	if(!tq->fq)
		error_goto(transqueue_open_error_3);

	tq->q = queue_init(0);
	if(!tq->q)
		error_goto(transqueue_open_error_4);

	tq->u = unit_malloc(tq->side);
	if(!tq->u)
		error_goto(transqueue_open_error_5);

	tq->lock = 0;
	tq->number = 0;
	gettimeofday(&tq->tv, NULL);

	tq->p = pool_init(2);
	if(!tq->p)
		error_goto(transqueue_open_error_6);

	switch(tq->side)
	{
	case SIDE_SEND:
		pool_start(tq->p, transqueue_send, tq);
		pool_start(tq->p, transqueue_monitor, tq);
		break;
	case SIDE_RECV:
		pool_start(tq->p, transqueue_fast, tq);
		pool_start(tq->p, transqueue_recv, tq);
		break;
	}
	pool_detach(tq->p, NULL);
	return tq;

transqueue_open_error_6:
	unit_free(tq->u, tq->side);
transqueue_open_error_5:
	queue_destroy(tq->q);
transqueue_open_error_4:
	queue_destroy(tq->fq);
transqueue_open_error_3:
	transfer_close(tq->t);
transqueue_open_error_2:
	lib_free(tq);
transqueue_open_error_1:
	return NULL;
}

int transqueue_close(transqueue *tq)
{
	pool_kill(tq->p, NULL);
	pool_destroy(tq->p);
	unit_free(tq->u, tq->side);
	queue_destroy(tq->q);
	queue_destroy(tq->fq);
	transfer_close(tq->t);
	lib_free(tq);
}

static int transqueue_monitor(transqueue *tq)
{
	package *p;
	struct timeval tv;
	__int64 usec = 5 MS; //微秒，带宽增加时该值可以降低
	while(1)
	{
		gettimeofday(&tv, NULL);
		if(queue_empty(tq->q) && ((tv.tv_sec - tq->tv.tv_sec) * 1000 * 1000 + (tv.tv_usec - tq->tv.tv_usec) > usec) && tq->number)
		{
			if(trylock(tq->lock) == TRUE)
			{
				p = package_malloc();
				p->task.empty = TRUE;
				package_set_task(p);
				queue_put(tq->q, p, -1);
				unlock(tq->lock);
			}
		}
		tools_sleep(usec);
	}
}

static int transqueue_send(transqueue *tq)
{
	package *p;
	static int id = 0;
	int i, j, k;

	while(1)
	{
		queue_get(tq->q, p, -1);
		lock(tq->lock, TRUE, -1);
		i = tq->number / ((UNIT_LENGTH - 1) * (UNIT_LENGTH - 1));
		j = tq->number / (UNIT_LENGTH - 1) % (UNIT_LENGTH - 1);
		k = tq->number % (UNIT_LENGTH - 1);

		p->code.id = id;
		p->code.sign = PACKAGE_CODE_SIGN_DATA;
		p->code.num = tq->number;
		p->code.i[0] = i;
		p->code.i[1] = j;
		p->code.i[2] = k;
		package_set_code(p);
		transfer_send(tq->t, p->data, sizeof(p->data));
		tools_fast_sleep(1000);

		tq->u[i][j][k] = p;

		tq->number++;
		if(tq->number >= UNIT_DATA_COUNT)
		{
			unit_encode(tq->u); //task, info也在编码范围
			for(i = 0; i < UNIT_LENGTH; i++)
				for(j = 0; j < UNIT_LENGTH; j++)
					for(k = 0; k < UNIT_LENGTH; k++)
					{
						p = tq->u[i][j][k];
						if(i == UNIT_LENGTH - 1 || j == UNIT_LENGTH - 1 || k == UNIT_LENGTH - 1)
						{
							p->code.id = id;
							p->code.sign = PACKAGE_CODE_SIGN_CODE;
							p->code.num = tq->number;
							p->code.i[0] = i;
							p->code.i[1] = j;
							p->code.i[2] = k;
							package_set_code(p); //只设置code
							transfer_send(tq->t, p->data, sizeof(p->data));
							tools_fast_sleep(1000);
							tq->number++;
							package_reset(p);
						}
						else
							//缓存数据包
							package_free(p);
					}
			tq->number = 0;
			__sync_fetch_and_add(&id, 1);
		}

		gettimeofday(&tq->tv, NULL);
		unlock(tq->lock);
	}
}

static int transqueue_fast(transqueue *tq)
{
	package *p;
	while(1)
	{
		p = package_malloc();
		transfer_recv(tq->t, p->data, sizeof(p->data));
		queue_put(tq->fq, p, -1);
	}
}

static int transqueue_recv(transqueue *tq)
{
	package *p, *q = NULL;
	int id_new = 0, id_old = -1;
	int number_new, number_old = UNIT_TOTAL_COUNT - 1; //初始化为最后一个
	int id, num;
	int i, j, k;
	while(1)
	{
		if(q)
		{
			p = q;
			q = NULL;
		}
		else
		{
			queue_get(tq->fq, p, -1);
			package_get(p);
		}
		if(p->code.id != id_new)
			unit_reset(tq->u);

		if(id_old < 0)
			id_old = id_new;
		id_new = p->code.id;
		number_new = p->code.num;

		if(id_new == id_old && (number_new == number_old + 1 || number_old >= UNIT_DATA_COUNT - 1) || id_new == id_old + 1 && number_new == 0 && number_old == UNIT_TOTAL_COUNT - 1)
		{
			if(p->task.empty == TRUE)
			{
				memcpy(tq->u[p->code.i[0]][p->code.i[1]][p->code.i[2]], p, sizeof(package));
				package_free(p);
			}
			else if(p->code.sign == PACKAGE_CODE_SIGN_DATA)
			{
				memcpy(tq->u[p->code.i[0]][p->code.i[1]][p->code.i[2]], p, sizeof(package));
				queue_put(tq->q, p, -1);
			}
			else
				package_free(p);
			id_old = id_new;
			number_old = number_new;
		}
		else
		{
			if(id_new != id_old)
			{
				//number_old < UNIT_DATA_COUNT - 1时该unit必定恢复不了（编码包全丢，还丢了数据包）
				//number_old >= UNIT_DATA_COUNT - 1时该unit必定不会出错（全为编码包）
				//number_new > 56时，必定丢包（一个面多出一条，但可修复一部分）
				if(number_old < UNIT_DATA_COUNT - 1)
					for(num = number_old + 1; num < UNIT_DATA_COUNT; num++)
						lost_package(id_old, num); //报一个id_old的unit出错
				for(id = id_old + 1; id < id_new; id++)
					lost_unit(id); //报一个id的unit出错
				number_old = 0;
			}

			memcpy(tq->u[p->code.i[0]][p->code.i[1]][p->code.i[2]], p, sizeof(package));
			if(number_new == 0)
				queue_put(tq->q, p, -1);
			else
			{
				while(queue_get(tq->fq, p, 1 S) == TRUE)
				{
					package_get(p);
					if(p->code.id != id_new)
					{
						q = p; //接收到下一个unit的包，下一次接收时直接用该包
						break;
					}
					memcpy(tq->u[p->code.i[0]][p->code.i[1]][p->code.i[2]], p, sizeof(package));
					if(p->code.num == UNIT_TOTAL_COUNT - 1)
					{
						package_free(p);
						break;
					}
					package_free(p);
				}
				unit_decode(tq->u);
				//将number_old + 1之后的正确的包加入队列，不正确的包报package出错
				for(num = number_old + 1; num < UNIT_DATA_COUNT; num++)
				{
					i = num / ((UNIT_LENGTH - 1) * (UNIT_LENGTH - 1));
					j = num / (UNIT_LENGTH - 1) % (UNIT_LENGTH - 1);
					k = num % (UNIT_LENGTH - 1);
					if(tq->u[i][j][k]->exist)
					{
						package_get(tq->u[i][j][k]);
						if(tq->u[i][j][k]->task.empty == FALSE)
						{
							p = package_malloc();
							memcpy(p, tq->u[i][j][k], sizeof(package));
							p->code.id = id_new;
							p->code.num = num;
							p->code.i[0] = i;
							p->code.i[1] = j;
							p->code.i[2] = k;
							queue_put(tq->q, p, -1);
						}
					}
					else
						lost_package(id_new, num);
				}
			}
			id_old = -1;
			number_old = UNIT_TOTAL_COUNT - 1;
		}

		gettimeofday(&tq->tv, NULL);
	}
}

int transqueue_push(transqueue *tq, package *p)
{
	package_set_info(p);
	package_set_task(p);
	queue_put(tq->q, p, -1);
}

package *transqueue_pop(transqueue *tq)
{
	package *p;
	queue_get(tq->q, p, -1);
	return p;
}


#ifdef TEST

#include "test.h"

void transqueue_test_1()
{
	transqueue_attr attr =
	{
		.side = SIDE_SEND,
		.ip = "127.0.0.1",
		.port = TRANSQUEUE_PORT_DEFAULT
	};
	transqueue *tq;

	tq = transqueue_open(&attr);
	test_assert(tq, NULL, 'p', TEST_UNEQUAL);

	package *p;
	int i;
	for(i = 0; i < 5; i++)
	{
		p = package_malloc();
		sprintf((char *)p->data, "1234567890");
		p->task.empty = FALSE;
		transqueue_push(tq, p);
		tools_fast_sleep(1000);
	}

	tools_sleep(500 MS); //保证在接收端开启后再关闭
	transqueue_close(tq);
}

void transqueue_test_2()
{
	transqueue_attr attr =
	{
		.side = SIDE_RECV,
		.port = TRANSQUEUE_PORT_DEFAULT,
	};
	transqueue *tq;

	tq = transqueue_open(&attr);
	test_assert(tq, NULL, 'p', TEST_UNEQUAL);
	package *p;
	int i;
	for(i = 0; i < 5; i++)
	{
		p = transqueue_pop(tq);
		test_assert((char *)p->data, "1234567890", 's', TEST_EQUAL);
		package_free(p);
	}

	transqueue_close(tq);
}

void transqueue_test()
{
	pool *p;
	p = pool_init(2);
	pool_start(p, transqueue_test_2, NULL);
	tools_sleep(100 MS);
	pool_start(p, transqueue_test_1, NULL);
	pool_join(p, NULL);
	pool_destroy(p);
}

#endif
