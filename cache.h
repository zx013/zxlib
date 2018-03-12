#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include "head.h"
#include "lib.h"
#include "dict.h"
#include "file.h"

#define CACHE_CLIENT_CACHE							".__cache__"
#define CACHE_CLIENT_STATE							".__state__"

#define CACHE_CLIENT_OFFSET_NUM					(sizeof(__int64) + 1 * sizeof(int))
#define CACHE_CLIENT_OFFSET_BLOCK					(sizeof(__int64) + 2 * sizeof(int))

typedef struct cache_block
{
	unsigned char sign:1;
} cache_block;

typedef struct cache_client
{
	char *filename;
	char *cachefile;
	file *cachefp;
	char *statefile;
	file *statefp;
	int num;

	__int64 filesize;
	int blocksize;
	int blocknum;
	cache_block *block;
} cache_client;

static cache_client *cache_client_open(char *filename, __int64 filesize, int blocksize);

static int cache_client_close(cache_client *cc);
//打开缓存，没有则创建
cache_client *cache_client_init(char *filename, __int64 filesize, int blocksize);

int cache_client_write(cache_client *cc, void *data, __int64 id);


#endif
