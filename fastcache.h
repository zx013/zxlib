#ifndef FASTCACHE_H
#define FASTCACHE_H

#include "lib.h"
#include "package.h"
#include "hash.h"
#include "queue.h"
#include "file.h"
#include "thread.h"
#include "sort.h"
#include "limit.h"
#include "error.h"

/*
用内存和硬盘共同实现高速缓存
尽可能的将数据写入硬盘，防止内存的溢出
如果有多块硬盘，分别写入不同的硬盘

每块硬盘创建一个缓存队列，开启单独的线程将队列中的数据写入到硬盘（每若干个数据包缓存到一个文件，并记录缓存信息，初始化时读出缓存的信息）
获取队列的长度，将数据加入到最短的队列中

初始化高速缓存
将数据写入高速缓存
从高速缓存读出数据
清除高速缓存中的数据
*/

#define FASTCACHE_MEMORY_NAME						"__memory__"
#define FASTCACHE_INDEX							".__index__"

typedef struct fastcache
{
	hash_t *hash;
	int size;
	int count;
	__int64 (*index)(__int64 *);
	queue *q;
	__int64 num; //当前块数量
	__int64 max; //最大块数量
	thread t;
} fastcache;

typedef struct fastcache_node
{
	queue *q;
	file *f;
	__int64 *s;
	char *name; //缓存名称，内存为NULL
	__int64 size; //缓存大小
	int block;
	int level; //缓存优先级

	__int64 (*index)(__int64 *); //同fastcache的index

	file *index_f; //索引的文件句柄
	__int64 *index_s; //索引的字符串
	char *index_name; //索引的文件名称，内存为NULL

	hash_t *hash;
	__int64 count;
	thread t;
	limit_speed *ls;
} fastcache_node;


fastcache *fastcache_init();

int fastcache_add(fastcache *fc, char *name, int level, __int64 size, __int64 speed);

int fastcache_del(fastcache *fc, char *name, int level);

static int __fastcache_write(fastcache_node *fn);

static int _fastcache_write(fastcache *fc);

inline int fastcache_write(fastcache *fc, package *p);

inline package *fastcache_read(fastcache *fc, __int64 index);

inline int fastcache_clean(fastcache *fc, __int64 index);

int fastcache_destroy(fastcache *fc);

#endif
