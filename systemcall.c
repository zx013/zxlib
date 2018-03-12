#include "systemcall.h"


int systemcall_send(queue *q, char *cmd, char *args[], int len)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = PACKAGE_TASK_TYPE_SYSTEMCALL, //任务类型
			.compress = 0, //压缩算法，没有时置空
			.encrypt = 0 //加密方式，没有时置空
		},
		.info =
		{
			.num = 1 //包的总数量，不能获得时置空
		}
	};

	analysis_pack(q, &attr, 0, ANALYSIS_INIT);
	analysis_pack(q, cmd, strlen(cmd), 0);
	int i;
	for(i = 0; i < len; i++)
		analysis_pack(q, args[i], strlen(args[i]), 0);
	analysis_pack(q, NULL, 0, ANALYSIS_FINISH);
}

int _systemcall_recv(queue *q)
{
	char cmd[COMMAND_SIZE] = {0};
	void *data;
	__int64 len;
	char *s1, *s2;

#ifdef _WIN32
	char c = '"';
	char s[] = "\"\"";
#else
	char c = '\'';
	char s[] = "'\"'\"'";
#endif

	//指令名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(cmd, "%.*s ", len, data);

	s1 = cmd + strlen(cmd);
	//参数
	while(analysis_unpack(q, &data, &len, ANALYSIS_STR) == TRUE)
	{
		*s1++ = c;

		s2 = data;
		while(len--)
		{
			if(*s2 == c)
			{
				sprintf(s1, s);
				s1 += sizeof(s) - 1;
				s2++;
			}
			else
				*s1++ = *s2++;
		}

		*s1++ = c;
		*s1++ = ' ';
	}
	system(cmd);

	return TRUE;
}

int systemcall_recv(queue *q)
{
	_systemcall_recv(q);
	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}


#ifdef TEST

#include "test.h"

void systemcall_test_1(queue *q)
{
	char *cmd = "echo";
	char *args[] = {"~!@#$%^&*()_+|{}:\"<>?`-=\[];',./"};
	systemcall_send(q, cmd, args, sizeof(args) / sizeof(char *));
}

void systemcall_test_2(queue *q)
{
	systemcall_recv(q);
}

void systemcall_test()
{
	queue *q = queue_init(0);
	systemcall_test_1(q);
	systemcall_test_2(q);
	queue_destroy(q);
}

#endif
