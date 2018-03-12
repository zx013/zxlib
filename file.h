#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "head.h"
#include "lib.h"
#include "error.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#endif

#ifdef _WIN32
#define FILE_SEEK_SET								FILE_BEGIN
#define FILE_SEEK_CUR								FILE_CURRENT
#define FILE_SEEK_END								FILE_END
#else
#define FILE_SEEK_SET								SEEK_SET
#define FILE_SEEK_CUR								SEEK_CUR
#define FILE_SEEK_END								SEEK_END
#endif

typedef struct file
{
	char *name; //文件名
#ifdef _WIN32
	HANDLE hd; //文件句柄
	HANDLE mhd; //文件映射句柄
#else
	int fd; //文件句柄
#endif
	void *buf; //文件映射后的指针
} file;


inline int file_create(char *filename, __int64 filesize);

inline file *file_open(char *name);

inline int file_read(file *f, void *buf, int size);

inline int file_write(file *f, void *buf, int size);

inline __int64 file_seek(file *f, __int64 offset, int sign);

inline int file_map(file *f);

inline int file_unmap(file *f);

inline int file_close(file *f);


#endif
