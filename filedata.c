#include "filedata.h"


int filedata_client_send(queue *q, fileinfo *f)
{
	analysis_attr attr =
	{
		.task =
		{
			.cache = FALSE,
			.retrans = FALSE,
			.type = PACKAGE_TASK_TYPE_FILE //任务类型
		}
	};

	list_each(f->list, {
		tools_filenode *fn = val;

		FILE *fp;
		fp = fopen(fn->path, "rb");
		if(fp == NULL)
		{
			continue;
		}

		analysis_pack(q, &attr, 0, ANALYSIS_INIT);
		analysis_pack(q, f->name_send, strlen(f->name_send), 0);
		analysis_pack(q, f->name_recv, strlen(f->name_recv), 0);

		analysis_pack(q, fn->name, strlen(fn->name), 0);
		analysis_pack(q, NULL, fn->size, ANALYSIS_PREFIX);

		char *buf;
		int size = analysis_remain() - sizeof(__int64); //文件块的最大大小
		int sign = ANALYSIS_FINISH;

		buf = lib_calloc(size);

		while(!feof(fp))
		{
			fread(buf, size, 1, fp);
			if(ferror(fp))
			{
				sign = ANALYSIS_ERROR;
				break;
			}
			task_step({
				lib_free(buf);
				fclose(fp);
				analysis_pack(q, NULL, 0, ANALYSIS_CANCEL);
			});
			analysis_pack(q, buf, size, 0);
		}

		lib_free(buf);
		fclose(fp);
		analysis_pack(q, NULL, 0, sign);
	});
}

int _filedata_recv(queue *q)
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

int filedata_recv(queue *q)
{
	if(_filedata_recv(q) == FALSE)
		analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}

int _filedata_client_recv(queue *q,  fileinfo *f)
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

	//文件名
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;

	char filename[PATH_SIZE];
	sprintf(filename, "%s/%.*s", f->path, len, data);

	//文件长度
	if(analysis_unpack(q, &data, &len, ANALYSIS_INT) == FALSE)
		return FALSE;

	__int64 filesize = len;

	//文件内容
	if(analysis_unpack(q, &data, &len, ANALYSIS_STR) == FALSE)
		return FALSE;

	cache_client *cc = cache_client_init(filename, filesize, len);
	cache_client_write(cc, data, analysis_package()->info.id);
	return TRUE;
}

int filedata_client_recv(queue *q,  fileinfo *f)
{
	_filedata_client_recv(q, f);
	analysis_unpack(q, NULL, NULL, ANALYSIS_FREE);
}


#ifdef TEST

#include "test.h"

void filedata_test_1(queue *q)
{
	fileinfo f =
	{
		.name_send = "send",
		.name_recv = "recv",
		.path = "test/filedata/send" //test/filedata/send/librxe-dev-librxe-1.0.0.zip
	};

	f.list = list_new();
	tools_listdir(f.path, f.list);
	fileinfo_client_send(q, &f);
	filedata_client_send(q, &f);
	tools_listdir_clear(f.list);
	list_free(f.list);
}

void filedata_test_2(queue *q)
{
	int i;
	fileinfo_recv(q);
	for(i = 0; i < 10; i++)
		filedata_recv(q);
}

void filedata_test_3()
{
	fileinfo f =
	{
		.path = "test/filedata/recv"
	};
	user *u;

	u = user_get("recv");

	f.list = list_new();

	fileinfo_client_recv(u->q_recv, &f);
	int i;
	for(i = 0; i < 10; i++)
		filedata_client_recv(u->q_recv, &f);

	tools_listdir_clear(f.list);
	list_free(f.list);
}

void filedata_test()
{
	queue *q = queue_init(0);

	filedata_test_1(q);
	filedata_test_2(q);
	filedata_test_3();

	queue_destroy(q);
}

#endif
