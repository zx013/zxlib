#include "package.h"


inline package *package_malloc()
{
	package *p;
	p = (package *)lib_calloc(sizeof(package));
	return p;
}

inline int package_free(package *p)
{
	lib_free(p);
}

inline int package_reset(package *p)
{
	memset(p, 0, sizeof(package));
}

inline int package_copy(package *p1, package *p2)
{
	memcpy(p1->data, p2->data, PACKAGE_TOTAL_LENGTH * sizeof(__int64));
}


inline int package_xor2(package *p0, package *p1, package *p2)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i];
}

inline int package_xor3(package *p0, package *p1, package *p2, package *p3)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i];
}

inline int package_xor4(package *p0, package *p1, package *p2, package *p3, package *p4)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i] ^ p4->data[i];
}

inline int package_xor5(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i] ^ p4->data[i] ^ p5->data[i];
}

inline int package_xor6(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i] ^ p4->data[i] ^ p5->data[i] ^ p6->data[i];
}

inline int package_xor7(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6, package *p7)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i] ^ p4->data[i] ^ p5->data[i] ^ p6->data[i] ^ p7->data[i];
}

inline int package_xor8(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6, package *p7, package *p8)
{
	int i;
	for(i = 0; i < PACKAGE_OFFSET_CODE; i++)
		p0->data[i] = p1->data[i] ^ p2->data[i] ^ p3->data[i] ^ p4->data[i] ^ p5->data[i] ^ p6->data[i] ^ p7->data[i] ^ p8->data[i];
}


inline int _package_get_code(__int64 data, package_code *code)
{
	code->id = PACKAGE_GET_CODE(data, ID);
	code->sign = PACKAGE_GET_CODE(data, SIGN);
	code->num = PACKAGE_GET_CODE(data, NUM);
	code->i[0] = PACKAGE_GET_CODE(data, I0);
	code->i[1] = PACKAGE_GET_CODE(data, I1);
	code->i[2] = PACKAGE_GET_CODE(data, I2);
}

inline int package_get_code(package *p)
{
	_package_get_code(p->data[PACKAGE_OFFSET_CODE], &p->code);
	p->exist = 1;
	p->data[PACKAGE_OFFSET_SIGN] = PACKAGE_SIGN;
}

inline __int64 _package_set_code(package_code *code)
{
	return (code->id << PACKAGE_SET_CODE(ID)) +
			(code->sign << PACKAGE_SET_CODE(SIGN)) +
			(code->num << PACKAGE_SET_CODE(NUM)) +
			(code->i[0] << PACKAGE_SET_CODE(I0)) +
			(code->i[1] << PACKAGE_SET_CODE(I1)) +
			(code->i[2] << PACKAGE_SET_CODE(I2));
}

inline int package_set_code(package *p)
{
	p->data[PACKAGE_OFFSET_CODE] = _package_set_code(&p->code);
}


inline int _package_get_task(__int64 data, package_task *task)
{
	task->id = PACKAGE_GET_TASK(data, ID);
	task->empty = PACKAGE_GET_TASK(data, EMPTY);
	task->cache = PACKAGE_GET_TASK(data, CACHE);
	task->retrans = PACKAGE_GET_TASK(data, RETRANS);
	task->type = PACKAGE_GET_TASK(data, TYPE);
	task->compress = PACKAGE_GET_TASK(data, COMPRESS);
	task->encrypt = PACKAGE_GET_TASK(data, ENCRYPT);
	task->state = PACKAGE_GET_TASK(data, STATE);
}

inline int package_get_task(package *p)
{
	_package_get_task(p->data[PACKAGE_OFFSET_TASK], &p->task);
}

inline __int64 _package_set_task(package_task *task)
{
	return (task->id << PACKAGE_SET_TASK(ID)) +
			(task->empty << PACKAGE_SET_TASK(EMPTY)) +
			(task->cache << PACKAGE_SET_TASK(CACHE)) +
			(task->retrans << PACKAGE_SET_TASK(RETRANS)) +
			(task->type << PACKAGE_SET_TASK(TYPE)) +
			(task->compress << PACKAGE_SET_TASK(COMPRESS)) +
			(task->encrypt << PACKAGE_SET_TASK(ENCRYPT)) +
			(task->state << PACKAGE_SET_TASK(STATE));
}

inline int package_set_task(package *p)
{
	p->data[PACKAGE_OFFSET_TASK] = _package_set_task(&p->task);
}


inline int _package_get_info(__int64 data, package_info *info)
{
	info->id = PACKAGE_GET_INFO(data, ID);
	info->num = PACKAGE_GET_INFO(data, NUM);
}

inline int package_get_info(package *p)
{
	_package_get_info(p->data[PACKAGE_OFFSET_INFO], &p->info);
}

inline __int64 _package_set_info(package_info *info)
{
	return (info->id << PACKAGE_SET_INFO(ID)) +
			(info->num << PACKAGE_SET_INFO(NUM));
}

inline int package_set_info(package *p)
{
	p->data[PACKAGE_OFFSET_INFO] = _package_set_info(&p->info);
}


inline int package_get(package *p)
{
	package_get_code(p);
	package_get_task(p);
	package_get_info(p);
}

inline int package_set(package *p)
{
	package_set_info(p);
	package_set_task(p);
	package_set_code(p);
}


#include "unit.h"
#include "queue.h"

inline int package_check_code(__int64 data)
{
	package_code code;

	_package_get_code(data, &code);

	if(code.sign > PACKAGE_CODE_SIGN_CODE)
		return FALSE;

	if(code.num >= UNIT_TOTAL_COUNT)
		return FALSE;

	if(code.i[0] >= UNIT_LENGTH)
		return FALSE;

	if(code.i[1] >= UNIT_LENGTH)
		return FALSE;

	if(code.i[2] >= UNIT_LENGTH)
		return FALSE;

	return TRUE;
}

inline int package_check_task(__int64 data)
{
	package_task task;

	_package_get_task(data, &task);

	if(task.type > PACKAGE_TASK_TYPE_FILE)
		return FALSE;

	if(task.compress > 8) //加密算法最大数量
		return FALSE;

	if(task.encrypt > 8) //压缩算法最大数量
		return FALSE;

	if(task.state > PACKAGE_TASK_STATE_ERROR)
		return FALSE;

	return TRUE;
}

inline int package_check_info(__int64 data)
{
	package_info info;

	_package_get_info(data, &info);

	if(info.id > info.num)
		return FALSE;

	return TRUE;
}


inline int package_check(queue *q)
{
	package *p = queue_head(q);
	if(!p)
		return FALSE;

	__int64 *data = p->data;
	//长度完整，sign标识正确，code, task, info位正常，则判断为完整的package
	if(p->len == PACKAGE_TOTAL_LENGTH * sizeof(__int64) &&
		data[PACKAGE_OFFSET_SIGN] == PACKAGE_SIGN &&
		package_check_code(data[PACKAGE_OFFSET_CODE]) == TRUE &&
		package_check_task(data[PACKAGE_OFFSET_TASK]) == TRUE &&
		package_check_info(data[PACKAGE_OFFSET_INFO]) == TRUE)
		return TRUE;

	int num = 0;
	int check = FALSE;
	int len = 0; //总长度
	int i = -1, j = 0;
	__int64 apart[2] = {0};
	queue_each(q, {
		p = val;
		len += p->len;
		if(len >= PACKAGE_TOTAL_LENGTH * sizeof(__int64))
		{
			if(i == -1)
				i = p->len + PACKAGE_TOTAL_LENGTH * sizeof(__int64) - len;
			else
				i = 0;

			if(apart[0] != 0)
			{
				apart[1] = p->data[0];
				for(j = 1; j < sizeof(__int64); j++)
				{
					if(*(__int64 *)(((char *)apart) + j) == PACKAGE_SIGN)
					{
						printf("package_sign 2 = %d %d\n", num, j);
					}
				}
				apart[0] = apart[1] = 0;
			}
			for(; i <= p->len - sizeof(__int64); i++)
			{
				if(*(__int64 *)(((char *)p->data) + i) == PACKAGE_SIGN)
				{
					printf("package_sign 1 = %d %d\n", num, i);
				}
			}
			//PACKAGE_SIGN被分割在两个包中
			if(i > p->len - sizeof(__int64))
				apart[0] = p->data[p->len / sizeof(__int64) - 1];
		}
		num++;
	});
	printf("%d %d\n", len, PACKAGE_TOTAL_LENGTH * sizeof(__int64));

	return FALSE;
}


inline package_stack *package_stack_init()
{
	package_stack *ps;
	ps = lib_calloc(sizeof(package_stack));
	ps->pl = lib_calloc(sizeof(package *) * 16);
	ps->num = 0;
	return ps;
}

inline int package_stack_destroy(package_stack *ps)
{
	lib_free(ps->pl);
	lib_free(ps);
}

inline int package_stack_push(package_stack *ps, package *p)
{
	ps->pl[ps->num] = p;
	ps->num++;
	ps->num %= 16;
}

inline int package_stack_xor(package_stack *ps, package *p)
{
	package **pl = ps->pl;
	switch(ps->num)
	{
	case 2:
		package_xor(p, pl[0], pl[1]);
		break;
	case 3:
		package_xor(p, pl[0], pl[1], pl[2]);
		break;
	case 4:
		package_xor(p, pl[0], pl[1], pl[2], pl[3]);
		break;
	case 5:
		package_xor(p, pl[0], pl[1], pl[2], pl[3], pl[4]);
		break;
	case 6:
		package_xor(p, pl[0], pl[1], pl[2], pl[3], pl[4], pl[5]);
		break;
	case 7:
		package_xor(p, pl[0], pl[1], pl[2], pl[3], pl[4], pl[5], pl[6]);
		break;
	case 8:
		package_xor(p, pl[0], pl[1], pl[2], pl[3], pl[4], pl[5], pl[6], pl[7]);
		break;
	}
	ps->num = 0;
}


#ifdef TEST

#include "test.h"

void package_test()
{
	package *p;

	p = package_malloc();
	p->code.id = PACKAGE_MASK(PACKAGE_BIT_CODE_ID);
	p->code.sign = PACKAGE_MASK(PACKAGE_BIT_CODE_SIGN);
	p->code.num = PACKAGE_MASK(PACKAGE_BIT_CODE_NUM);
	p->code.i[0] = PACKAGE_MASK(PACKAGE_BIT_CODE_I0);
	p->code.i[1] = PACKAGE_MASK(PACKAGE_BIT_CODE_I1);
	p->code.i[2] = PACKAGE_MASK(PACKAGE_BIT_CODE_I2);

	p->task.id = PACKAGE_MASK(PACKAGE_BIT_TASK_ID);
	p->task.empty = PACKAGE_MASK(PACKAGE_BIT_TASK_EMPTY);
	p->task.cache = PACKAGE_MASK(PACKAGE_BIT_TASK_CACHE);
	p->task.retrans = PACKAGE_MASK(PACKAGE_BIT_TASK_RETRANS);
	p->task.type = PACKAGE_MASK(PACKAGE_BIT_TASK_TYPE);
	p->task.compress = PACKAGE_MASK(PACKAGE_BIT_TASK_COMPRESS);
	p->task.encrypt = PACKAGE_MASK(PACKAGE_BIT_TASK_ENCRYPT);
	p->task.state = PACKAGE_MASK(PACKAGE_BIT_TASK_STATE);

	p->info.id = PACKAGE_MASK(PACKAGE_BIT_INFO_ID);
	p->info.num = PACKAGE_MASK(PACKAGE_BIT_INFO_NUM);

	package_set(p);
	memset(&p->code, 0, sizeof(p->code));
	memset(&p->task, 0, sizeof(p->task));
	memset(&p->info, 0, sizeof(p->info));
	package_get(p);

	test_assert(p->code.id, PACKAGE_MASK(PACKAGE_BIT_CODE_ID), 'x', TEST_EQUAL);
	test_assert(p->code.sign, PACKAGE_MASK(PACKAGE_BIT_CODE_SIGN), 'x', TEST_EQUAL);
	test_assert(p->code.num, PACKAGE_MASK(PACKAGE_BIT_CODE_NUM), 'x', TEST_EQUAL);
	test_assert(p->code.i[0], PACKAGE_MASK(PACKAGE_BIT_CODE_I0), 'x', TEST_EQUAL);
	test_assert(p->code.i[1], PACKAGE_MASK(PACKAGE_BIT_CODE_I1), 'x', TEST_EQUAL);
	test_assert(p->code.i[2], PACKAGE_MASK(PACKAGE_BIT_CODE_I2), 'x', TEST_EQUAL);

	test_assert(p->task.id, PACKAGE_MASK(PACKAGE_BIT_TASK_ID), 'x', TEST_EQUAL);
	test_assert(p->task.empty, PACKAGE_MASK(PACKAGE_BIT_TASK_EMPTY), 'x', TEST_EQUAL);
	test_assert(p->task.cache, PACKAGE_MASK(PACKAGE_BIT_TASK_CACHE), 'x', TEST_EQUAL);
	test_assert(p->task.retrans, PACKAGE_MASK(PACKAGE_BIT_TASK_RETRANS), 'x', TEST_EQUAL);
	test_assert(p->task.type, PACKAGE_MASK(PACKAGE_BIT_TASK_TYPE), 'x', TEST_EQUAL);
	test_assert(p->task.compress, PACKAGE_MASK(PACKAGE_BIT_TASK_COMPRESS), 'x', TEST_EQUAL);
	test_assert(p->task.encrypt, PACKAGE_MASK(PACKAGE_BIT_TASK_ENCRYPT), 'x', TEST_EQUAL);
	test_assert(p->task.state, PACKAGE_MASK(PACKAGE_BIT_TASK_STATE), 'x', TEST_EQUAL);

	test_assert(p->info.id, PACKAGE_MASK(PACKAGE_BIT_INFO_ID), 'x', TEST_EQUAL);
	test_assert(p->info.num, PACKAGE_MASK(PACKAGE_BIT_INFO_NUM), 'x', TEST_EQUAL);

	test_assert(p->data[PACKAGE_OFFSET_SIGN] - PACKAGE_SIGN, 0, 'd', TEST_EQUAL);

	package_free(p);

	/*
	queue *q;

	q = queue_init(0);

	p = package_malloc();

	p->data[PACKAGE_OFFSET_SIGN - 3000] = PACKAGE_SIGN;
	p->len = 4000 * sizeof(__int64);
	queue_put(q, p, -1);

	p = package_malloc();
	p->len = 1000 * sizeof(__int64);
	queue_put(q, p, -1);

	__int64 i = PACKAGE_SIGN;
	p = package_malloc();
	memcpy((char *)(p->data + 2000) - 3, &i, 3);
	p->len = 2000 * sizeof(__int64);
	queue_put(q, p, -1);

	p = package_malloc();
	memcpy(p->data, (char *)(&i) + 3, sizeof(__int64) - 3);
	p->len = 2000 * sizeof(__int64);
	queue_put(q, p, -1);

	test_assert(package_check(q), TRUE, 'd', TEST_EQUAL);

	queue_destroy(q);
	*/
}

#endif
