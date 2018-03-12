#include "remote.h"


int remote_send(queue *q, remote *r)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = PACKAGE_TASK_TYPE_REMOTE, //��������
			.compress = 0, //ѹ���㷨��û��ʱ�ÿ�
			.encrypt = 0 //���ܷ�ʽ��û��ʱ�ÿ�
		},
		.info =
		{
			.num = 1 //���������������ܻ��ʱ�ÿ�
		}
	};

	analysis_pack(q, &attr, 0, ANALYSIS_INIT);
	analysis_pack(q, r->name, strlen(r->name), 0);
	int i;
	for(i = 0; i < r->size; i++)
	{
		if(r->len[i] > 0)
			analysis_pack(q, r->data[i], r->len[i], 0);
		else
			analysis_pack(q, r->data[i], strlen(r->data[i]), 0);
	}
	analysis_pack(q, NULL, 0, ANALYSIS_FINISH);
}

static dict *d = NULL;

int _remote_recv(queue *q)
{
	char name[PARAMETER_SIZE] = {0};
	remote *r;
	void *data;
	__int64 len;

	if(d == NULL)
		return FALSE;

	//������
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(name, "%.*s", len, data);
	r = dict_get(d, name);
	if(!r->func)
		return FALSE;
	r->size = 0;

	//����
	while(analysis_unpack(q, &data, &len, ANALYSIS_STR) == TRUE)
	{
		r->data[r->size] = data;
		r->len[r->size] = len;
		r->size++;
	}
	(*(void(*)())r->func)();

	return TRUE;
}

int remote_recv(queue *q)
{
	_remote_recv(q);
	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}

int remote_register(char *name, void *func)
{
	remote *r;
	if(d == NULL)
		d = dict_new();
	r = dict_get(d, name);
	if(!r)
	{
		r = lib_calloc(sizeof(remote));
		r->func = func;
		dict_set(d, name, r);
	}
}

void *_remote_args(int num, char *name)
{
	remote *r;
	r = dict_get(d, name);
	if(!r)
		return NULL;
	//��������ĳ���λ����Ϊ0������������ַ������㣬���Ȳ���̫��
	*(__int64 *)(r->data[num] + r->len[num]) = 0;
	return r->data[num];
}


#ifdef TEST

#include "test.h"

//�����ݽṹ�壬�ṹ����ָ��ָ��������޷����䣬ֻ�ܴ����ָ������е�����
typedef struct remote_test_struct
{
	char s[16];
	int i;
} remote_test_struct;

//remote_args��ȡ�����������numָ���ǵڼ�������
void remote_test_func()
{
	char *s;
	remote_test_struct *st;
	s = remote_args(0);
	st = remote_args(1);
	test_assert(s, "~!@#$%^&*()_+|{}:\"<>?`-=\[];',./", 's', TEST_EQUAL);
	test_assert(st->s, "abcdef", 's', TEST_EQUAL);
	test_assert(st->i, 123456, 'd', TEST_EQUAL);
}

void remote_test_1(queue *q)
{
	remote_test_struct st =
	{
		.s = "abcdef",
		.i = 123456
	};
	remote r = 
	{
		.name = "remote_test_func",
		.data = {"~!@#$%^&*()_+|{}:\"<>?`-=\[];',./", &st},
		.len = {0, sizeof(remote_test_struct)},
		.size = 2
	};
	remote_send(q, &r);
}

void remote_test_2(queue *q)
{
	remote_register("remote_test_func", remote_test_func);
	remote_recv(q);
}

void remote_test()
{
	queue *q = queue_init(0);
	remote_test_1(q);
	remote_test_2(q);
	queue_destroy(q);
}

#endif
