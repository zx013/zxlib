#include "analysis.h"


static __thread analysis a;

//�������������package����ѹ����У�����ѹ����е�package������֧�ֲ���
//��ȷ������ṹ��ʱ����ͬƽ̨�µĽṹ�����ݸ�ʽ�Ƿ���ͬ
int _analysis_pack(queue *q, void *data, __int64 len, int sign)
{
	static int id = 0;

	if(sign & ANALYSIS_INIT)
	{
		__sync_fetch_and_add(&id, 1);
		a.attr = *(analysis_attr *)data;
		a.p = NULL;
		a.id = 0;
		a.offset_len = 0;
		a.remain_len = PACKAGE_SIZE;
		memset(a.prefix, 0, PACKAGE_SIZE);
		a.prefix_len = 0;
		return TRUE;
	}

	if(sign & ANALYSIS_NEW)
	{
		//����ǿ�ͷ�Ļ���������
		if(a.offset_len != a.prefix_len)
		{
			queue_put(q, a.p, -1);
			a.p = NULL;
		}
	}

	if(!a.p)
	{
		//����һ���µİ��������ǰ׺�������ǰ׺
		a.p = package_malloc();
		a.p->task.id = id;
		a.p->task.empty = FALSE;
		a.p->task.cache = a.attr.task.cache;
		a.p->task.retrans = a.attr.task.retrans;
		a.p->task.type = a.attr.task.type;
		a.p->task.compress = a.attr.task.compress;
		a.p->task.encrypt = a.attr.task.encrypt;
		a.p->task.state = PACKAGE_TASK_STATE_NORMAL;
		a.p->info.id = a.id++;
		a.p->info.num = a.attr.info.num;

		package_set_info(a.p);
		package_set_task(a.p);

		if(a.prefix_len > 0)
			memcpy(a.p->data, a.prefix, a.prefix_len);
		a.offset_len = a.prefix_len;
		a.remain_len = PACKAGE_SIZE - a.prefix_len;
	}

	if(len > 0)
	{
		memcpy((void *)a.p->data + a.offset_len, data, len);
		a.offset_len += len;
		a.remain_len -= len;
	}

	if(!a.prefix_len && sign & ANALYSIS_PREFIX)
	{
		//ǰ׺ֻ�ڵ�һ��������Ч
		a.prefix_len = PACKAGE_SIZE - a.remain_len;
		memcpy(a.prefix, a.p->data, a.prefix_len);
	}

	if(sign & ANALYSIS_END | sign & ANALYSIS_CANCEL | sign & ANALYSIS_ERROR)
	{
		//����ʱ����ѹ�봫�����
		if(sign & ANALYSIS_END)
			a.p->task.state = PACKAGE_TASK_STATE_FINISH;
		else if(sign & ANALYSIS_CANCEL)
			a.p->task.state = PACKAGE_TASK_STATE_CANCEL;
		else if(sign & ANALYSIS_ERROR)
			a.p->task.state = PACKAGE_TASK_STATE_ERROR;
		package_set_task(a.p);

		queue_put(q, a.p, -1);
		a.p = NULL;
		memset(a.prefix, 0, PACKAGE_SIZE);
		a.prefix_len = 0;
	}

	return TRUE;
}

//��Ҫ��֤���ܳ���package����
int analysis_pack(queue *q, void *data, __int64 len, int sign)
{
	__int64 d;

	if(sign & ANALYSIS_INIT)
		_analysis_pack(q, data, len, sign);
	else if(sign & ANALYSIS_FINISH)
		_analysis_pack(q, NULL, 0, ANALYSIS_END);
	else if(sign & ANALYSIS_CANCEL)
		_analysis_pack(q, NULL, 0, ANALYSIS_CANCEL);
	else if(sign &  ANALYSIS_ERROR)
		_analysis_pack(q, NULL, 0, ANALYSIS_ERROR);
	else
	{
		if(data)
		{
			//��������package�Ĵ�С
			if(sizeof(d) + len > PACKAGE_SIZE - a.prefix_len)
				return FALSE;
			//����packageʣ���С
			if(!(sign & ANALYSIS_NEW))
				if(sizeof(d) + len > a.remain_len)
					sign |= ANALYSIS_NEW;

			d = len + ((__int64)ANALYSIS_STR << ANALYSIS_MAX);
			_analysis_pack(q, &d, sizeof(d), sign & ANALYSIS_NEW);
			_analysis_pack(q, data, len, sign & (ANALYSIS_END | ANALYSIS_PREFIX));
		}
		else
		{
			//����packageʣ���С
			if(!(sign & ANALYSIS_NEW))
				if(sizeof(d) > a.remain_len)
					sign |= ANALYSIS_NEW;

			d = len + ((__int64)ANALYSIS_INT << ANALYSIS_MAX);
			_analysis_pack(q, &d, sizeof(d), sign);
		}
	}
	return TRUE;
}

/*
���������쳣ʱ������package
����FALSEʱ˵����package����
�����ȶ�ȡ��һ��packageʱ�ͷ�
*/
int analysis_unpack(queue *q, void **data, __int64 *len, int sign)
{
	__int64 d;
	int type;

	if(sign & ANALYSIS_FREE)
	{
		if(a.p)
		{
			package_free(a.p);
			a.p = NULL;
		}
		return TRUE;
	}

	if(sign & ANALYSIS_RESET)
	{
		queue_put(q, a.p, -1);
		a.p = NULL;
		return TRUE;
	}

	if(!a.p)
	{
		queue_get(q, a.p, -1);
		a.offset_len = 0;
		a.remain_len = PACKAGE_SIZE;
	}

	if(a.remain_len < sizeof(d)) //��֤d������
		return FALSE;

	d = *(__int64 *)((void *)a.p->data + a.offset_len);
	a.offset_len += sizeof(d);
	a.remain_len -= sizeof(d);

	if(!d)
		return FALSE;
	
	type = d >> ANALYSIS_MAX;
	d &= PACKAGE_MASK(ANALYSIS_MAX);
	if(type != sign & (ANALYSIS_INT | ANALYSIS_STR))
		return FALSE;

	switch(type)
	{
	case ANALYSIS_INT:
		*data = NULL;
		*len = d;
		break;
	case ANALYSIS_STR:
		*data = (void *)a.p->data + a.offset_len;
		*len = d;
		a.offset_len += d;
		a.remain_len -= d;
		if(a.remain_len < 0)
			return FALSE;
		break;
	}
	return TRUE;
}

inline package *analysis_package()
{
	return a.p;
}

//��ȡ�������������ݳ���,���뱣֤����ó��ȵ����ݺ�package�պ�����
//��Ҫ���ڶ�ȡ�ļ�ʱ��ȡ�ļ���ĳ���
inline int analysis_remain()
{
	return a.remain_len;
}

//����type����ѹ���Ӧ�Ķ��У����д�����hash�У�keyΪtypeֵ��Ӧ���ַ������������򴴽�
//��̨����
static dict *d = NULL;

int analysis_allocate(queue *q)
{
	queue *tq;
	package *p;
	char key[16];

	if(d == NULL)
		d = dict_new();

	while(1)
	{
		queue_get(q, p, -1);

		sprintf(key, "%d", p->task.type);
		tq = dict_get(d, key);
		if(!tq)
		{
			tq = queue_init(0);
			dict_set(d, key, tq);
		}

		queue_put(tq, p, -1);
	}
}

queue *analysis_queue(int type)
{
	char key[16];

	if(d == NULL)
		return NULL;

	sprintf(key, "%d", type);
	return dict_get(d, key);
}


#ifdef TEST

#include "test.h"

void analysis_test_1(queue *q)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = 0, //��������
			.compress = 0, //ѹ���㷨��û��ʱ�ÿ�
			.encrypt = 0 //���ܷ�ʽ��û��ʱ�ÿ�
		},
		.info =
		{
			.num = 0 //���������������ܻ��ʱ�ÿ�
		}
	};
	analysis_pack(q, &attr, 0, ANALYSIS_INIT);
	analysis_pack(q, "abc", 3, ANALYSIS_PREFIX);
	analysis_pack(q, "def", 3, 0);
	analysis_pack(q, NULL, 123, ANALYSIS_NEW);
	analysis_pack(q, NULL, 0, ANALYSIS_FINISH);
}

void analysis_test_2(queue *q)
{
	char s[16];

	void *data;
	__int64 len;

	analysis_unpack(q, &data, &len, ANALYSIS_STR);
	sprintf(s, "%.*s", len, data);
	test_assert(s, "abc", 's', TEST_EQUAL);

	analysis_unpack(q, &data, &len, ANALYSIS_STR);
	sprintf(s, "%.*s", len, data);
	test_assert(s, "def", 's', TEST_EQUAL);

	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);

	analysis_unpack(q, &data, &len, ANALYSIS_STR);
	sprintf(s, "%.*s", len, data);
	test_assert(s, "abc", 's', TEST_EQUAL);

	analysis_unpack(q, &data, &len, ANALYSIS_INT);
	test_assert(len, 123, 'd', TEST_EQUAL);

	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}

void analysis_test()
{
	queue *q = queue_init(0);
	analysis_test_1(q);
	analysis_test_2(q);
	queue_destroy(q);
}

#endif
