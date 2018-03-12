#include "fileinfo.h"


int fileinfo_client_send(queue *q, fileinfo *f)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = PACKAGE_TASK_TYPE_FILEINFO //任务类型
		}
	};

	analysis_pack(q, &attr, 0, ANALYSIS_INIT);
	analysis_pack(q, f->name_send, strlen(f->name_send), 0);
	analysis_pack(q, f->name_recv, strlen(f->name_recv), 0);
	analysis_pack(q, NULL, f->list->size, ANALYSIS_PREFIX);

	list_each(f->list, {
		tools_filenode *fn = val;
		int len = strlen(fn->name);

		//保证文件信息数据不被分割
		analysis_pack(q, fn->name, len, 2 * sizeof(__int64) + len > analysis_remain() ? ANALYSIS_NEW : 0);
		analysis_pack(q, NULL, fn->size, 0);
	});
	analysis_pack(q, NULL, 0, ANALYSIS_FINISH);
}


int _fileinfo_recv(queue *q)
{
	user *u;
	char name[USERNAME_SIZE];

	void *data;
	__int64 len;

	//源用户名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;

	//目标用户名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(name, "%.*s", len, data);
	u = user_get(name);

	analysis_unpack(u->q_recv, NULL, NULL, ANALYSIS_RESET);
	return TRUE;
}

int fileinfo_recv(queue *q)
{
	if(_fileinfo_recv(q) == FALSE)
		analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}

int _fileinfo_client_recv(queue *q,  fileinfo *f)
{
	void *data;
	__int64 len;

	//源用户名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(f->name_send, "%.*s", len, data);

	//目标用户名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;
	sprintf(f->name_recv, "%.*s", len, data);

	//文件数量
	if(analysis_unpack(q, &data, &len, ANALYSIS_INT) == FALSE)
		return FALSE;
	f->number = len;

	int path_size = strlen(f->path);
	//文件名
	while(analysis_unpack(q, &data, &len, ANALYSIS_STR) == TRUE)
	{
		tools_filenode *fn = lib_calloc(sizeof(tools_filenode));
		fn->path = lib_calloc(path_size + len + 1);
		fn->name = fn->path + path_size;
		sprintf(fn->path, "%s%.*s", f->path, len, data);

		//文件长度
		if(analysis_unpack(q, &data, &len, ANALYSIS_INT) == FALSE)
		{
			lib_free(fn->path);
			lib_free(fn);
			return FALSE;
		}
		fn->size = len;
		list_push(f->list, fn);
	}

	return TRUE;
}

int fileinfo_client_recv(queue *q,  fileinfo *f)
{
	_fileinfo_client_recv(q, f);
	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}


#ifdef TEST

#include "test.h"

void fileinfo_test_1(queue *q)
{
	fileinfo f =
	{
		.name_send = "send",
		.name_recv = "recv",
		.path = "test/fileinfo"
	};

	f.list = list_new();
	tools_listdir(f.path, f.list);
	fileinfo_client_send(q, &f);
	fileinfo_client_send(q, &f);
	tools_listdir_clear(f.list);
	list_free(f.list);
}

void fileinfo_test_2(queue *q)
{
	test_assert(queue_size(user_get("recv")->q_recv), 0, 'd', TEST_EQUAL);
	fileinfo_recv(q);
	test_assert(queue_size(user_get("recv")->q_recv), 1, 'd', TEST_EQUAL);
	fileinfo_recv(q);
	test_assert(queue_size(user_get("recv")->q_recv), 2, 'd', TEST_EQUAL);
}

void fileinfo_test_3()
{
	fileinfo f;
	user *u;
	
	u = user_get("recv");

	f.list = list_new();

	fileinfo_client_recv(u->q_recv, &f);
	fileinfo_client_recv(u->q_recv, &f);
	list_each(f.list, {
		tools_filenode *fn = val;
		char s[32];
		sprintf(s, "file%d", fn->size - 1);
		test_assert(fn->name, s, 's', TEST_EQUAL);
	});

	tools_listdir_clear(f.list);
	list_free(f.list);
}

void fileinfo_test()
{
	queue *q = queue_init(0);
	fileinfo_test_1(q);
	fileinfo_test_2(q);
	fileinfo_test_3();
	queue_destroy(q);
}

#endif
