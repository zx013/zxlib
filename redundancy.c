#include "redundancy.h"


/*
可以进行数据流式的编码，效率会有所降低
根unit方式类似的方式可以写连续异或，可以提高速度
*/
int __redundancy_encode(redundancy *r);

int redundancy_reset(redundancy *r)
{
	int i;

	r->id++;
	if(r->id % REDUNDANCY_STEP == 0)
		r->id -= REDUNDANCY_STEP;
	r->dn = 0;
	r->cn = 0;
	//for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
	//	package_reset(r->cp[i]);
	for(i = 0; i < REDUNDANCY_LENGTH; i++)
		r->mark[i] = FALSE;
}

redundancy *redundancy_init()
{
	redundancy *r;
	int i;

	r = lib_calloc(sizeof(redundancy));
	if(r == NULL)
		error_goto(redundancy_init_error_1);

	for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
		r->cp[i] = package_malloc();
	redundancy_reset(r);

	r->id = rand();
	r->list = list_new();
	if(r->list == NULL)
		error_goto(redundancy_init_error_2);

	r->ps = package_stack_init();
	if(r->ps == NULL)
		error_goto(redundancy_init_error_3);

	return r;

redundancy_init_error_3:
	list_free(r->list);
redundancy_init_error_2:
	lib_free(r);
redundancy_init_error_1:
	return NULL;
}

int redundancy_destroy(redundancy *r)
{
	package_stack_destroy(r->ps);
	list_free(r->list);
	lib_free(r);
}

redundancy2 *redundancy2_init()
{
	redundancy2 *r;
	int i, j;

	r = lib_calloc(sizeof(redundancy2));
	if(r == NULL)
		error_goto(redundancy2_init_error_1);

	r->id = rand();
	r->i1 = 0;
	r->i2 = 0;

	r->list = list_new();
	if(r->list == NULL)
		error_goto(redundancy2_init_error_2);

	r->q1 = queue_init(0);
	if(r->q1 == NULL)
		error_goto(redundancy2_init_error_3);

	for(i = 0; i < REDUNDANCY_LENGTH; i++)
	{
		r->r1[i] = redundancy_init();
		if(r->r1 == NULL)
			error_goto(redundancy2_init_error_4);
		r->r1[i]->id = r->id;
		r->r1[i]->n = 0;
		r->r1[i]->i = i;

		r->r2[i] = redundancy_init();
		if(r->r1 == NULL)
			error_goto(redundancy2_init_error_5);
		r->r2[i]->id = r->id;
		r->r2[i]->n = 1;
		r->r2[i]->i = i;
	}

	return r;

redundancy2_init_error_5:
	redundancy_destroy(r->r1[i]);
redundancy2_init_error_4:
	for(j = 0; j < i; j++)
	{
		redundancy_destroy(r->r1[j]);
		redundancy_destroy(r->r2[j]);
	}
	queue_destroy(r->q1);
redundancy2_init_error_3:
	list_free(r->list);
redundancy2_init_error_2:
	lib_free(r);
redundancy2_init_error_1:
	return NULL;
}

int redundancy2_destroy(redundancy2 *r)
{
	int i;

	for(i = 0; i < REDUNDANCY_LENGTH; i++)
	{
		redundancy_destroy(r->r1[i]);
		redundancy_destroy(r->r2[i]);
	}

	queue_destroy(r->q1);
	list_free(r->list);
	lib_free(r);
}


//编码表
static const char *redundancy_table[] =
{
	"00111",
	"01011",
	"01101",
	"01110",
	"10011",
	"10101",
	"10110",
	"11001",
	"11010",
	"11100",
	"11111", //编码到这里为止
	"10000",
	"01000",
	"00100",
	"00010",
	"00001" //解码到这里为止
};

//将数据异或到对应的分组中，如果p在r->cp中，则设置except为p在r->cp中的位置
inline int redundancy_stack_xor(redundancy *r)
{
	int i, j;

	for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
	{
		for(j = 0; j < REDUNDANCY_LENGTH_DATA; j++)
		{
			if(redundancy_table[j][i] == '1')
				package_stack_push(r->ps, r->dp[j]);
		}
		package_stack_xor(r->ps, r->cp[i]);
	}
}

inline int redundancy_xor(redundancy *r, package *p, int num, int except)
{
	int i;
	for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
	{
		if(i == except)
			continue;
		if(redundancy_table[num][i] == '1')
			package_xor(r->cp[i], r->cp[i], p);
	}
	if(except >= 0 && except < REDUNDANCY_LENGTH_CODE)
		if(redundancy_table[num][except] == '1')
			package_xor(r->cp[except], r->cp[except], p);
}

inline int redundancy_put(redundancy *r, package *p)
{
	p->exist = TRUE;
	list_put(r->list, p);
}

inline package *redundancy_get(redundancy *r)
{
	package *p;
	if(list_get(r->list, p) == FALSE)
		return NULL;
	return p;
}

inline int redundancy2_put(redundancy2 *r, package *p)
{
	list_put(r->list, p);
}

inline package *redundancy2_get(redundancy2 *r)
{
	package *p;
	if(list_get(r->list, p) == FALSE)
		return NULL;
	return p;
}


//查找丢失包的位置
inline int redundancy_lost(redundancy *r, int num)
{
	int lost;
	int i;

	for(i = 0; i < REDUNDANCY_LENGTH; i++)
	{
		if(r->mark[i] == TRUE)
			continue;
		if(redundancy_table[i][num] == '1')
			return i;
	}
	return -1;
}

/*
根据cp和mark解码，可在还未接收完成时对丢失数据进行恢复
如丢失1个data包，则在收到1个解码包后就可以尝试恢复
*/
inline int _redundancy_imple(redundancy *r, package *p, int lost)
{
	redundancy_xor(r, p, lost, -1);

	r->mark[lost] = TRUE;

	if(lost < REDUNDANCY_LENGTH_DATA)
		r->dn++;
	else
		r->cn++;

	p->code.id = r->id;

	p->code.i[r->n] = r->i;
	p->code.i[(1 + r->n) % 2] = lost;

	package_set_code(p);
	redundancy_put(r, p);
	printf("fix %d %d %d %d\n", r->id % 1000, r->n, lost, r->i);
}

inline int redundancy_imple(redundancy *r)
{
	package *p;
	int num[REDUNDANCY_LENGTH_CODE]; //每个编码包对应出错的数量
	int lost; //丢失包的位置
	int i, j, k;

	for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
	{
		num[i] = 0;
		for(j = 0; j < REDUNDANCY_LENGTH; j++)
		{
			if(r->mark[j] == TRUE)
				continue;

			num[i] += redundancy_table[j][i] - '0';
		}
	}


	for(i = 0; i < REDUNDANCY_LENGTH_CODE; i++)
	{
		if(num[i] == 1)
		{
			lost = redundancy_lost(r, i);

			p = package_malloc();
			package_copy(p, r->cp[i]);
			_redundancy_imple(r, p, lost);
			if(r->dn == REDUNDANCY_LENGTH_DATA)
				return 0;

			for(j = 0; j < REDUNDANCY_LENGTH_CODE; j++)
				num[j] -= redundancy_table[lost][j] - '0';

			i = -1;
			continue;
		}

		for(j = i + 1; j < REDUNDANCY_LENGTH_CODE; j++)
		{
			switch(num[j] - num[i])
			{
			case 1:
			case -1:
				{
				int check = 0;
				for(k = 0; k < REDUNDANCY_LENGTH; k++)
				{
					if(r->mark[k] == TRUE)
						continue;
					if(redundancy_table[k][i] != redundancy_table[k][j])
					{
						lost = k;
						check++;
						if(check != 1)
							break;
					}
				}
				if(check != 1)
					break;

				p = package_malloc();
				package_copy(p, r->cp[i]);
				package_xor(p, p, r->cp[j]);

				_redundancy_imple(r, p, lost);
				if(r->dn == REDUNDANCY_LENGTH_DATA)
					return 0;
				for(j = 0; j < REDUNDANCY_LENGTH_CODE; j++)
					num[j] -= redundancy_table[lost][j] - '0';

				i = -1;
				j = REDUNDANCY_LENGTH_CODE;
				break;
				}
			}
		}
	}
}


inline int _redundancy_encode(redundancy *r, package *p, __int64 id)
{
	//redundancy_xor(r, p, r->dn, -1);
	r->dp[r->dn] = p;

	p->code.id = id;
	p->code.i[r->n] = r->dn + r->cn;
	package_set_code(p);

	if(++r->dn >= REDUNDANCY_LENGTH_DATA)
	{
		redundancy_stack_xor(r);
		do
		{
			p = package_malloc();
			package_copy(p, r->cp[r->cn]);
			p->code.id = id;

			p->code.i[r->n] = REDUNDANCY_LENGTH_DATA + r->cn;
			p->code.i[(r->n + 1) % 2] = r->i;

			package_set_code(p);

			redundancy_put(r, p);
		} while(++r->cn < REDUNDANCY_LENGTH_CODE);
		redundancy_reset(r);
	}
}


/*
将数据包编码后放入发送队列，可能需要加锁
*/
inline int redundancy_encode(redundancy *r, package *p)
{
	redundancy_put(r, p);
	_redundancy_encode(r, p, r->id);
}

inline int redundancy2_encode(redundancy2 *r, package *p)
{
	redundancy *r1, *r2, *r3;
	int i;

	r1 = r->r1[r->i1];
	r2 = r->r2[r->i2];

	_redundancy_encode(r1, p, r->id);
	_redundancy_encode(r2, p, r->id);

	redundancy2_put(r, p);
	while((p = redundancy_get(r2)) != NULL)
		redundancy2_put(r, p);

	for(i = REDUNDANCY_LENGTH_DATA; i < REDUNDANCY_LENGTH; i++)
	{
		if((p = redundancy_get(r1)) == NULL)
			break;

		r3 = r->r2[i];
		_redundancy_encode(r3, p, r->id);

		redundancy2_put(r, p);
		while((p = redundancy_get(r3)) != NULL)
			redundancy2_put(r, p);
	}

	if((r->i2 = (r->i2 + 1) % REDUNDANCY_LENGTH_DATA) == 0)
		r->i1 = (r->i1 + 1) %  REDUNDANCY_LENGTH_DATA;
}


int _redundancy_decode(redundancy *r, package *p, int num)
{
	package *p_cache;
	int i;

	package_get_code(p);
	if(r->id != p->code.id)
	{
		redundancy_reset(r);
		r->id = p->code.id;
	}

	redundancy_xor(r, p, num, -1);

	if(num < REDUNDANCY_LENGTH_DATA)
	{
		r->dn++;
	}
	else
	{
		r->cn++;
		if(r->dn == REDUNDANCY_LENGTH_DATA) //数据完整
			return 0;
		//for(i = 0; i < REDUNDANCY_LENGTH; i++)
		//	printf("%d ", r->mark[i]);
		//printf("n = %d, i = %d, num = %d\n", r->n, r->i, num);
		redundancy_imple(r);
	}
	return 0;
}

package *redundancy_decode(redundancy *r, queue *q)
{
	package *p;
	int i;

	while(1)
	{
		//解码序列中有数据，直接从序列中取
		while((p = redundancy_get(r)) != NULL)
		{
			if(p->code.i[0] < REDUNDANCY_LENGTH_DATA)
				return p;
		}

		queue_get(q, p, -1);

		redundancy_put(r, p);
		i = p->code.i[0];
		r->mark[i] = TRUE;
		_redundancy_decode(r, p, i);
	}
}

int _redundancy2_decode(redundancy2 *r, package *p)
{
	redundancy *r1, *r2;
	int i1, i2;

	redundancy2_put(r, p);
	if(p->exist == TRUE)
		return 0;

	i1 = p->code.i[0];
	i2 = p->code.i[1];

	r1 = r->r1[i1];
	r2 = r->r2[i2];

	r1->mark[i2] = TRUE;
	r2->mark[i1] = TRUE;

	_redundancy_decode(r1, p, i2);
	_redundancy_decode(r2, p, i1);

	while((p = redundancy_get(r1)) != NULL)
		_redundancy2_decode(r, p);
	while((p = redundancy_get(r2)) != NULL)
		_redundancy2_decode(r, p);
}

package *redundancy2_decode(redundancy2 *r, queue *q)
{
	package *p;
	static int i = 0;

	while(1)
	{
		while((p = redundancy2_get(r)) != NULL)
		{
			if(p->code.i[0] < REDUNDANCY_LENGTH_DATA && p->code.i[1] < REDUNDANCY_LENGTH_DATA)
				return p;
		}

		queue_get(q, p, -1);
		//if(rand() % 100 < 20)
		//	continue;
		if(i++ < 3)
			continue;

		_redundancy2_decode(r, p);
	}
}


int redundancy_multi_run(redundancy2 *r);

redundancy_multi *redundancy_multi_init(int num)
{
	redundancy_multi *rm;
	redundancy2 *r;
	int i, j;

	rm = lib_calloc(sizeof(redundancy_multi));
	if(rm == NULL)
		error_goto(redundancy_multi_init_error_1);

	rm->num = num;
	rm->p = pool_init(rm->num);
	if(rm->p == NULL)
		error_goto(redundancy_multi_init_error_2);

	rm->r = lib_calloc(sizeof(redundancy2 *) * rm->num);
	if(rm->r == NULL)
		error_goto(redundancy_multi_init_error_3);

	rm->q = queue_init(0);
	if(rm->q == NULL)
		error_goto(redundancy_multi_init_error_4);

	for(i = 0; i < rm->num; i++)
	{
		r = redundancy2_init();
		if(r == NULL)
			error_goto(redundancy_multi_init_error_5);

		r->id = i * REDUNDANCY_STEP + rand() % REDUNDANCY_STEP;
		for(j = 0; j < REDUNDANCY_LENGTH; j++)
		{
			r->r1[j]->id = r->id;
			r->r2[j]->id = r->id;
		}
		r->q2 = rm->q;

		rm->r[i] = r;
	}

	for(i = 0; i < rm->num; i++)
	{
		pool_start(rm->p, redundancy_multi_run, rm->r[i]);
	}
	return rm;

redundancy_multi_init_error_5:
	for(j = 0; j < i; j++)
		redundancy2_destroy(rm->r[j]);
	lib_free(rm->r);
redundancy_multi_init_error_4:
	queue_destroy(rm->q);
redundancy_multi_init_error_3:
	pool_destroy(rm->p);
redundancy_multi_init_error_2:
	lib_free(rm);
redundancy_multi_init_error_1:
	return NULL;
}

int redundancy_multi_destroy(redundancy_multi *rm)
{
	int i;
	pool_join(rm->p, NULL);
	for(i = 0; i < rm->num; i++)
		redundancy2_destroy(rm->r[i]);
	lib_free(rm->r);
	pool_destroy(rm->p);
	lib_free(rm);
}

int number = 0;
int redundancy_multi_run(redundancy2 *r)
{
	package *p;
	while(1)
	{
		if(queue_get(r->q1, p, 1 S) == FALSE)
			break;
		redundancy2_encode(r, p);
		while((p = redundancy2_get(r)) != NULL)
		{
			__sync_fetch_and_add(&number, 1);
			package_free(p);
			//queue_put(r->q2, p, -1);
		}
	}
}

#define redundancy_multi_encode(...) __VA_FUNC__(redundancy_multi_encode, __VA_ARGS__)

#define redundancy_multi_encode_2(rm, p) redundancy_multi_encode_3(rm, p, 0)

int redundancy_multi_encode_3(redundancy_multi *rm, package *p, int priority)
{
	static int num = 0;

	queue_put(rm->r[num]->q1, p, 0);
	num++;
	num %= rm->num;

	return 0;

	int i;

	for(i = priority; i < rm->num; i++)
	{
		queue_put(rm->r[i]->q1, p, 0);
	}
}


#ifdef TEST

#include "pool.h"
#include "test.h"


void redundancy_test_1(queue *q)
{
	redundancy *r;
	package *p;
	int i, j;

	r = redundancy_init();

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < REDUNDANCY_LENGTH_DATA; j++)
		{
			p = package_malloc();
			redundancy_encode(r, p);
			while((p = redundancy_get(r)) != NULL)
				queue_put(q, p, -1);
		}
	}

	redundancy_destroy(r);
}

void redundancy_test_2(queue *q)
{
	redundancy *r;
	package *p;

	r = redundancy_init();

	while(1)
	{
		p = redundancy_decode(r, q);
		package_free(p);
	}

	redundancy_destroy(r);
}

void redundancy_test_3(queue *q)
{
	redundancy2 *r;
	package *p;
	int i, j;

	r = redundancy2_init();

	for(i = 0; i < 1000; i++)
	{
		for(j = 0; j < REDUNDANCY_LENGTH_DATA * REDUNDANCY_LENGTH_DATA; j++)
		{
			p = package_malloc();
			p->task.id = j;
			package_set(p);
			redundancy2_encode(r, p);
			while((p = redundancy2_get(r)) != NULL)
			{
				package_free(p);
				//queue_put(q, p, -1);
			}
		}
	}

	redundancy2_destroy(r);
}

void redundancy_test_4(queue *q)
{
	redundancy2 *r;
	package *p;
	int i;

	r = redundancy2_init();

	i = 0;
	while(1)
	{
		queue_get(q, p, -1);
		//p = redundancy2_decode(r, q);
		package_get(p);

		//printf("<%d=%d, %d, %d> ", i++, p->task.id, p->code.i[0], p->code.i[1]);
		printf("%x %x %03d | ", p->code.i[0], p->code.i[1], p->task.id);
	}

	redundancy2_destroy(r);
}

void redundancy_test_5(queue *q)
{
	redundancy_multi *rm;
	rm = redundancy_multi_init(2);

	package *p;
	int i;
	for(i = 0; i < 1000 * REDUNDANCY_LENGTH_DATA * REDUNDANCY_LENGTH_DATA; i++)
	{
		p = package_malloc();
		redundancy_multi_encode(rm, p);
	}

	redundancy_multi_destroy(rm);
}

//opcontrol --reset && time ./zx013 -t redundancy 1 && opcontrol --dump && opreport -l zx013
void redundancy_test()
{
	pool *p;
	queue *q;

	p = pool_init(2);
	q = queue_init(0);
	//pool_start(p, redundancy_test_2, q);
	//tools_sleep(100 MS);
	//pool_start(p, redundancy_test_1, q);
	//pool_start(p, redundancy_test_4, q);
	//tools_sleep(100 MS);
	//pool_start(p, redundancy_test_3, q);
	//pool_start(p, redundancy_test_5, q);
	//tools_sleep(10 S);
	//pool_kill(p, NULL);

	pool_join(p, NULL);
	printf("number = %d\n", number);

	queue_destroy(q);
	pool_destroy(p);
}

#endif
