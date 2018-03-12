#include "remote.h"


int remote_send(queue *q, remote *r)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = PACKAGE_TASK_TYPE_REMOTE, //任务类型
			.compress = 0, //压缩算法，没有时置空
			.encrypt = 0 //加密方式，没有时置空
		},
		.info =
		{
			.num = 1 //包的总数量，不能获得时置空
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

	//函数名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(name, "%.*s", len, data);
	r = dict_get(d, name);
	if(!r->func)
		return FALSE;
	r->size = 0;

	//参数
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
	//将参数后的长度位设置为0，方便后续的字符串计算，长度不能太长
	*(__int64 *)(r->data[num] + r->len[num]) = 0;
	return r->data[num];
}


#ifdef TEST

#include "test.h"

//若传递结构体，结构体中指针指向的数据无法传输，只能传输非指针变量中的数据
typedef struct remote_test_struct
{
	char s[16];
	int i;
} remote_test_struct;

//remote_args获取参数，后面的num指明是第几个参数
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
