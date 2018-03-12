#include "file.h"


/*
可创建大文件，空间不足时创建一个空的文件并返回失败
*/

inline int file_create_dir(char *filename, __int64 filesize)
{
	char *name;
	char *s1, *s2;

	s1 = lib_calloc(strlen(filename) + 1);
	if(!s1)
		error_goto(file_create_dir_error_1);

	sprintf(s1, "%s", filename);
	s2 = s1;

	while(name = strchr(s2, '/'))
	{
		*name = 0;

		if(access(s1, 0) == -1)
		{
#ifdef _WIN32
			if(mkdir(s1) == -1);
#else
			if(mkdir(s1, S_IRUSR | S_IWUSR) == -1)
#endif
				error_goto(file_create_dir_error_2);
		}
		*name = '/';
		s2 = name + 1;
	}
	lib_free(s1);
	return TRUE;

file_create_dir_error_2:
	lib_free(s1);
file_create_dir_error_1:
	return FALSE;
}

//不足则补齐，超过则截取
inline int file_create_file(char *filename, __int64 filesize)
{
#ifdef _WIN32
	HANDLE hd;
	hd = CreateFile(filename, GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hd == INVALID_HANDLE_VALUE)
		error_goto(file_create_file_error_1);

	DWORD high, low;
	high = filesize >> 32;
	low = filesize & 0xFFFFFFFF;
	if(SetFilePointer(hd, low, &high, FILE_BEGIN) == HFILE_ERROR)
		error_goto(file_create_file_error_2);

	if(SetEndOfFile(hd) == 0)
		error_goto(file_create_file_error_2);

	CloseHandle(hd);
#else
	int fd;
	fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd < 0)
		error_goto(file_create_file_error_1);

	//似乎可以通过参数控制，fallocate指令可以识别空间不足
	ftruncate(fd, filesize);
	if(fallocate(fd, 0, 0, filesize) == -1)
		error_goto(file_create_file_error_2);

	close(fd);
#endif
	return TRUE;

file_create_file_error_2:
#ifdef _WIN32
	CloseHandle(hd);
#else
	ftruncate(fd, 0);
	close(fd);
#endif
file_create_file_error_1:
	return FALSE;
}

inline int file_create(char *filename, __int64 filesize)
{
	if(file_create_dir(filename, filesize) == FALSE)
		error_goto(file_create_error_1);

	if(file_create_file(filename, filesize) == FALSE)
		error_goto(file_create_error_1);

	return TRUE;

file_create_error_1:
	return FALSE;
}

inline file *file_open(char *name)
{
	file *f;

	f = lib_calloc(sizeof(file));
	if(!f)
		error_goto(file_open_error_1);

#ifdef _WIN32
	f->hd = CreateFile(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!f->hd)
		error_goto(file_open_error_2);
#else
	f->fd = open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(f->fd < 0)
		error_goto(file_open_error_2);
#endif

	f->name = lib_calloc(strlen(name) + 1);
	if(!f->name)
		error_goto(file_open_error_3);
	sprintf(f->name, "%s", name);

	f->buf = NULL;
	return f;

file_open_error_3:
#ifdef _WIN32
	CloseHandle(f->hd);
#else
	close(f->fd);
#endif
file_open_error_2:
	lib_free(f);
file_open_error_1:
	return NULL;
}

inline int file_read(file *f, void *buf, int size)
{
#ifdef _WIN32
	int len;
	if(ReadFile(f->hd, buf, size, (LPDWORD)&len, NULL) == 0)
		return NONE;
	return len;
#else
	int result;
	result = read(f->fd, buf, size);
	if(result == -1)
		return NONE;
	return result;
#endif
}

inline int file_write(file *f, void *buf, int size)
{
#ifdef _WIN32
	int len;
	if(WriteFile(f->hd, buf, size, (LPDWORD)&len, NULL) == 0)
		return NONE;
	return len;
#else
	int result;
	result = write(f->fd, buf, size);
	if(result == -1)
		return NONE;
	return result;
#endif
}

inline __int64 file_seek(file *f, __int64 offset, int sign)
{
	__int64 result;
#ifdef _WIN32
	DWORD high, low;
	high = offset >> 32;
	low = offset & 0xFFFFFFFF;
	result = SetFilePointer(f->hd, low, &high, sign);
	if(result == HFILE_ERROR)
		return NONE;
	return result;
#else
	result = llseek(f->fd, offset, sign);
	if(result == -1)
		return NONE;
	return result;
#endif
}

inline __int64 file_size(file *f)
{
#ifdef _WIN32
	DWORD high, low;
	low = GetFileSize(f->hd, &high);
	if(low == INVALID_FILE_SIZE)
		return NONE;
	return ((__int64)high << 32) + low;
#else
	int result;
	result = llseek(f->fd, 0, SEEK_END);
	if(result == -1)
		return NONE;
	return result;
#endif
}

inline int file_map(file *f)
{
	if(f->buf)
		return TRUE;

	__int64 size = file_size(f);
#ifdef _WIN32
	f->mhd = CreateFileMapping(f->hd, NULL, PAGE_READWRITE, 0, 0, f->name);
	if(f->mhd == 0)
		error_goto(file_map_error_1);

	f->buf = MapViewOfFile(f->mhd, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if(!f->buf)
		error_goto(file_map_error_2);
	return TRUE;

file_map_error_2:
	CloseHandle(f->mhd);
file_map_error_1:
	return FALSE;
#else
	//msync(f->buf, size, MS_SYNC)
	f->buf = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, f->fd, 0);
	if(!f->buf)
		return FALSE;
	return TRUE;
#endif
}

inline int file_unmap(file *f)
{
#ifdef _WIN32
	FlushViewOfFile(f->buf, file_size(f));
	UnmapViewOfFile(f->buf);
	CloseHandle(f->mhd);
#else
	munmap(f->buf, file_size(f));
#endif
	f->buf = NULL;
}

inline int file_close(file *f)
{
	if(f)
	{
		lib_free(f->name);
#ifdef _WIN32
		CloseHandle(f->hd);
#else
		close(f->fd);
#endif
		lib_free(f);
	}
}

inline int file_delete(char *filename)
{
	remove(filename);
}


#ifdef TEST

#include "test.h"

void file_test()
{
	file *f;
	char buf[16];
	char name[] = "test/file/test";

	file_delete(name);

	file_create(name, 8);
	f = file_open(name);
	file_map(f);
	sprintf(f->buf, "abc");
	((char *)(f->buf))[3] = 'd';

	memset(buf, 0, 16);
	test_assert(file_seek(f, 0, FILE_SEEK_SET), 0, 'd', TEST_EQUAL);
	test_assert(file_read(f, buf, 16), 8, 'd', TEST_EQUAL);
	test_assert(buf, "abcd", 's', TEST_EQUAL);

	test_assert(file_seek(f, 4, FILE_SEEK_SET), 4, 'd', TEST_EQUAL);
	test_assert(file_write(f, "efghij", 6), 6, 'd', TEST_EQUAL);
	test_assert(f->buf, "abcdefghij", 's', TEST_EQUAL);

	memset(buf, 0, 16);
	test_assert(file_seek(f, 0, FILE_SEEK_SET), 0, 'd', TEST_EQUAL);
	test_assert(file_read(f, buf, 16), 10, 'd', TEST_EQUAL);
	test_assert(buf, "abcdefghij", 's', TEST_EQUAL);

	file_unmap(f);
	file_close(f);
	file_delete(name);
}

#endif
