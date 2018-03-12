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
���ڴ��Ӳ�̹�ͬʵ�ָ��ٻ���
�����ܵĽ�����д��Ӳ�̣���ֹ�ڴ�����
����ж��Ӳ�̣��ֱ�д�벻ͬ��Ӳ��

ÿ��Ӳ�̴���һ��������У������������߳̽������е�����д�뵽Ӳ�̣�ÿ���ɸ����ݰ����浽һ���ļ�������¼������Ϣ����ʼ��ʱ�����������Ϣ��
��ȡ���еĳ��ȣ������ݼ��뵽��̵Ķ�����

��ʼ�����ٻ���
������д����ٻ���
�Ӹ��ٻ����������
������ٻ����е�����
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
	__int64 num; //��ǰ������
	__int64 max; //��������
	thread t;
} fastcache;

typedef struct fastcache_node
{
	queue *q;
	file *f;
	__int64 *s;
	char *name; //�������ƣ��ڴ�ΪNULL
	__int64 size; //�����С
	int block;
	int level; //�������ȼ�

	__int64 (*index)(__int64 *); //ͬfastcache��index

	file *index_f; //�������ļ����
	__int64 *index_s; //�������ַ���
	char *index_name; //�������ļ����ƣ��ڴ�ΪNULL

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
