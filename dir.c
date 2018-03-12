#include "dir.h"


//获取文件大小
__int64 tools_filesize(char *filepath)
{
	__int64 size;
	file *f = file_open(filepath);
	size = file_size(f);
	file_close(f);
	return size;
}

char *tools_filename(char *filepath)
{
	char *filename;

	filename = strrchr(filepath, '/');

	return filename ? filename + 1 : filepath;
}

void _tools_listdir(char *filepath, int pathsize, list_t *list)
{
	DIR *dir;
	struct dirent *ptr;
	char path[PATH_SIZE]; //目录下文件的路径

	dir = opendir(filepath);
	while((ptr = readdir(dir)) != NULL)
	{
		if(!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
			continue;

		memset(path, 0, PATH_SIZE);
		sprintf(path, "%s/%s", filepath, ptr->d_name);

		if(tools_isdir(path))
			_tools_listdir(path, pathsize, list);
		else
		{
			tools_filenode *fn = lib_calloc(sizeof(tools_filenode));
			fn->path = lib_calloc(strlen(path) + 1);
			strcpy(fn->path, path);

			fn->name = fn->path + pathsize + 1;
			fn->size = tools_filesize(path);
			list_push(list, fn);
		}
	}
	closedir(dir);
}

void tools_listdir(char *filepath, list_t *list)
{
	if(tools_isdir(filepath))
		_tools_listdir(filepath, strlen(filepath), list);
	else
	{
		tools_filenode *fn = lib_calloc(sizeof(tools_filenode));
		fn->path = lib_calloc(strlen(filepath) + 1);
		strcpy(fn->path, filepath);

		fn->name = tools_filename(filepath);
		fn->size = tools_filesize(filepath);
		list_push(list, fn);
	}
}

void tools_listdir_clear(list_t *list)
{
	list_each(list, {
		tools_filenode *fn = val;
		lib_free(fn->path);
		lib_free(fn);
	});
}


#ifdef TEST

#include "test.h"

void dir_test()
{
	list_t *list;

	list = list_new();
	tools_listdir("test/tools", list);
	list_each(list, {
		tools_filenode *fn = val;
		switch(fn->size)
		{
		case 2:
			test_assert(fn->name, "file1", 's', TEST_EQUAL);
			break;
		case 3:
			test_assert(fn->name, "file2", 's', TEST_EQUAL);
			break;
		case 4:
			test_assert(fn->name, "dir1/dir3/file3", 's', TEST_EQUAL);
			break;
		case 5:
			test_assert(fn->name, "dir1/file4", 's', TEST_EQUAL);
			break;
		case 6:
			test_assert(fn->name, "dir2/dir4/file5", 's', TEST_EQUAL);
			break;
		case 7:
			test_assert(fn->name, "dir2/file6", 's', TEST_EQUAL);
			break;
		}
	});
	test_assert(list->size, 6, 'd', TEST_EQUAL);
	tools_listdir_clear(list);
	list_free(list);

	list = list_new();
	tools_listdir("test/tools/file1", list);
	list_each(list, {
		tools_filenode *fn = val;
		test_assert(fn->name, "file1", 's', TEST_EQUAL);
		test_assert(fn->size, 2, 'd', TEST_EQUAL);
	});
	test_assert(list->size, 1, 'd', TEST_EQUAL);
	tools_listdir_clear(list);
	list_free(list);
}

#endif
