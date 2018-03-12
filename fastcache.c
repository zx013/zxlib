#include "fastcache.h"


fastcache *fastcache_init()
{
	fastcache *fc;

	fc = lib_calloc(sizeof(fastcache));
	if(!fc)
		error_goto(fastcache_init_error_1);

	fc->hash = hash_new();
	if(!fc->hash)
		error_goto(fastcache_init_error_2);

	fc->size = sizeof(__int64);
	fc->count = sizeof(((package *)0)->data) / sizeof(__int64);
	__sync_lock_test_and_set(&fc->num, 0);
	__sync_lock_test_and_set(&fc->max, 0);

	inline __int64 index(__int64 *data)
	{
		//task.id, info.id
		return ((data[PACKAGE_OFFSET_TASK] & PACKAGE_MASK(PACKAGE_BIT_TASK_ID)) << PACKAGE_BIT_INFO_ID) + (data[PACKAGE_OFFSET_INFO] & PACKAGE_MASK(PACKAGE_BIT_INFO_ID));
	}
	fc->index = index;

	fc->q = queue_init(0);
	if(!fc->q)
		error_goto(fastcache_init_error_3);

	if(thread_run(&fc->t, _fastcache_write, fc) == FALSE)
		error_goto(fastcache_init_error_4);

	return fc;

fastcache_init_error_4:
	queue_destroy(fc->q);
fastcache_init_error_3:
	hash_free(fc->hash);
fastcache_init_error_2:
	lib_free(fc);
fastcache_init_error_1:
	return NULL;
}

//name为NULL时为内存
int fastcache_add(fastcache *fc, char *name, int level, __int64 size, __int64 speed)
{
	fastcache_node *fn;
	int block;

	fn = lib_calloc(sizeof(fastcache_node));
	if(!fn)
		error_goto(fastcache_add_error_1);

	if(!(fc->size * fc->count))
		error_goto(fastcache_add_error_2);

	fn->block = size / (fc->size * fc->count);
	if(fn->block == 0)
		error_goto(fastcache_add_error_2);

	size = fn->block * (fc->size * fc->count);

	if(name)
	{
		if(file_create(name, size) == FALSE)
			error_goto(fastcache_add_error_2);

		fn->f = file_open(name);
		if(!fn->f)
			error_goto(fastcache_add_error_2);

		fn->name = lib_calloc(strlen(name) + 1);
		if(!fn->name)
			error_goto(fastcache_add_error_3);

		sprintf(fn->name, "%s", name);

		fn->index_name = lib_calloc(strlen(name) + sizeof(FASTCACHE_INDEX) + 1);
		if(!fn->index_name)
			error_goto(fastcache_add_error_4);
		sprintf(fn->index_name, "%s%s", name, FASTCACHE_INDEX);

		if(file_create(fn->index_name, fn->block * sizeof(__int64)) == FALSE)
			error_goto(fastcache_add_error_5);

		fn->index_f = file_open(fn->index_name);
		if(!fn->index_f)
			error_goto(fastcache_add_error_5);
	}
	else
	{
		fn->s = lib_calloc(size);
		if(!fn->s)
			error_goto(fastcache_add_error_2);

		fn->name = NULL;
	}

	fn->q = queue_init(0);
	if(!fn->q)
		error_goto(fastcache_add_error_6);

	fn->size = size;
	fn->level = level;
	fn->index = fc->index;

	limit_speed_attr attr =
	{
		.limit = speed,
		.size = sizeof(((package *)0)->data),
		.interval = 100 MS,
		.duration = 1 S
	};
	fn->ls = limit_speed_init(&attr);
	if(!fn->ls)
		error_goto(fastcache_add_error_7);

	hash_t *hash;
	hash = hash_get(fc->hash, fn->level);
	if(hash == NULL)
	{
		hash = hash_new();
		if(!hash)
			error_goto(fastcache_add_error_7);
		hash_set(fc->hash, fn->level, hash);
	}

	__sync_lock_test_and_set(&fn->count, 0);
	if(name)
	{
		if(file_map(fn->index_f) == FALSE)
			error_goto(fastcache_add_error_8);
		fn->index_s = fn->index_f->buf;

		hash_set(hash, fn->name, fn);
		int i;
		for(i = 0; i < fn->block; i++)
			if(fn->index_s[i])
				__sync_fetch_and_add(&fn->count, 1);
	}
	else
	{
		fn->index_s = lib_calloc(fn->block * sizeof(__int64));
		if(!fn->index_s)
			error_goto(fastcache_add_error_8);

		hash_set(hash, FASTCACHE_MEMORY_NAME, fn);
	}

	fn->hash = hash_new();
	if(!fn->hash)
		error_goto(fastcache_add_error_9);

	if(thread_run(&fn->t, __fastcache_write, fn) == FALSE)
		error_goto(fastcache_add_error_10);

	__sync_fetch_and_add(&fc->num, fn->count);
	__sync_fetch_and_add(&fc->max, fn->block);
	return TRUE;

fastcache_add_error_10:
	hash_free(fn->hash);
fastcache_add_error_9:
	if(name)
		hash_del(hash, fn->name);
	else
		hash_del(hash, FASTCACHE_MEMORY_NAME);
fastcache_add_error_8:
	limit_speed_destroy(fn->ls);
fastcache_add_error_7:
	queue_destroy(fn->q);
fastcache_add_error_6:
	if(name)
		file_close(fn->index_f);
fastcache_add_error_5:
	if(name)
		lib_free(fn->index_name);
fastcache_add_error_4:
	if(name)
		lib_free(fn->name);
fastcache_add_error_3:
	if(name)
		file_close(fn->f);
	else
		lib_free(fn->s);
fastcache_add_error_2:
	lib_free(fn);
fastcache_add_error_1:
	return FALSE;
}

int fastcache_del(fastcache *fc, char *name, int level)
{
	fastcache_node *fn;
	hash_t *hash;
	int check;

	if(name)
		check = TRUE;
	else
	{
		check = FALSE;
		name = FASTCACHE_MEMORY_NAME;
	}

	if(level < 0)
	{
		fn = NULL;
		hash_each_int64(fc->hash, {
			hash = val;
			fn = hash_get(hash, name);
			if(fn)
			{
				level = key;
				break;
			}
		});
	}
	else
	{
		hash = hash_get(fc->hash, level);
		if(!hash)
			return FALSE;
		fn = hash_get(hash, name);
	}
	if(!fn)
		return FALSE;

	__sync_fetch_and_sub(&fc->num, fn->count);
	__sync_fetch_and_sub(&fc->max, fn->block);
	hash_free(fn->hash);

	hash_del(hash, name);
	if(hash_size(hash) == 0)
	{
		hash_del(fc->hash, level);
		hash_free(hash);
	}

	thread_kill(fn->t);
	if(check == TRUE)
	{
		lib_free(fn->index_name);
		file_unmap(fn->index_f);
		file_close(fn->index_f);

		lib_free(fn->name);
		file_close(fn->f);
	}
	else
	{
		lib_free(fn->index_s);
		lib_free(fn->s);
	}

	limit_speed_destroy(fn->ls);
	queue_destroy(fn->q);
	lib_free(fn);
	return TRUE;
}

//thread_run(__fastcache_write, fn);
static int __fastcache_write(fastcache_node *fn)
{
	package *p;
	__int64 id;
	int i;

	i = 0;
	while(1)
	{
		queue_get(fn->q, p, -1);
		while(fn->index_s[i])
		{
			i++;
			i %= fn->block;
		}
		id = fn->index(p->data);
		if(fn->name)
		{
			file_seek(fn->f, i * sizeof(p->data), FILE_SEEK_SET);
			file_write(fn->f, p->data, sizeof(p->data));
		}
		else
		{
			memcpy(fn->s + i * sizeof(p->data) / sizeof(__int64), p->data, sizeof(p->data));
		}
		__sync_lock_test_and_set(fn->index_s + i, id);
		hash_set(fn->hash, id, i + 1); //避免i==0
		package_free(p);
	}
	thread_exit();
}

static int _fastcache_write(fastcache *fc)
{
	fastcache_node *fn;
	package *p;
	hash_t *hash;

	queue_get(fc->q, p, -1);
	while(1)
	{
		int size = hash_size(fc->hash);
		int index;
		__int64 level;
		fastcache_node *fn_list[size];
		int i;

		index = -1;
		level = -1;
		i = 0;
		hash_each_int64(fc->hash, {
			if(i >= size)
				continue;

			fn_list[i] = NULL;

			hash = val;
			hash_each(hash, {
				fn = val;

				if(limit_speed_check(fn->ls) == FALSE)
					continue;

				if(fn->count >= fn->block)
					continue;

				if(!(fn_list[i] && queue_size(fn_list[i]->q) <= queue_size(fn->q)))
					fn_list[i] = fn;
			});

			if(fn_list[i])
			{
				if(!(index >=0 && key <= level))
				{
					index = i;
					level = key;
				}
			}

			i++;
		});
		if(index >= 0)
		{
			fn = fn_list[index];
			queue_put(fn->q, p, -1);
			limit_speed_add(fn->ls);
			__sync_fetch_and_add(&fn->count, 1);
			//printf("%d %d %d\n", index, level, fn->count);
			queue_get(fc->q, p, -1);
		}
		else
			tools_yield();
	}
}

inline int fastcache_write(fastcache *fc, package *p)
{
	if(fc->num < fc->max)
	{
		package_set(p);
		__sync_fetch_and_add(&fc->num, 1);
		queue_put(fc->q, p, -1);
		return TRUE;
	}
	return FALSE;
}

inline package *fastcache_read(fastcache *fc, __int64 index)
{
	fastcache_node *fn;
	package *p;
	hash_t *hash;
	int i;

	hash_each_int64(fc->hash, {
		hash = val;

		hash_each(hash, {
			fn = val;

			i = (__int64)hash_get(fn->hash, index);
			if(!i)
				continue;
			i--;

			p = package_malloc();
			if(fn->name)
			{
				file_seek(fn->f, i * sizeof(p->data), FILE_SEEK_SET);
				file_read(fn->f, p->data, sizeof(p->data));
			}
			else
			{
				memcpy(p->data, fn->s + i * sizeof(p->data) / sizeof(__int64), sizeof(p->data));
			}

			package_get(p);
			return p;
		});
	});
	return NULL;
}

inline int fastcache_clean(fastcache *fc, __int64 index)
{
	fastcache_node *fn;
	package *p;
	hash_t *hash;
	int i;

	hash_each_int64(fc->hash, {
		hash = val;

		hash_each(hash, {
			fn = val;

			i = (__int64)hash_get(fn->hash, index);
			if(!i)
				continue;
			i--;

			hash_del(fn->hash, index);
			__sync_lock_test_and_set(fn->index_s + i, 0);

			__sync_fetch_and_sub(&fn->count, 1);
			__sync_fetch_and_sub(&fc->num, 1);
			return TRUE;
		});
	});
	return FALSE;
}

int fastcache_destroy(fastcache *fc)
{
	thread_kill(fc->t);
	queue_destroy(fc->q);
	hash_free(fc->hash);
	lib_free(fc);
}


#ifdef TEST

#include "test.h"

void fastcache_test()
{
	fastcache *fc;
	char *name1 = "test/fastcache/test1";
	char *index_name1 = "test/fastcache/test1.__index__";
	file_delete(name1);
	file_delete(index_name1);

	fc = fastcache_init();

	test_assert(fastcache_add(fc, NULL, 1, 1 MB, 1 MB), TRUE, 'd', TEST_EQUAL);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 31, 'd', TEST_EQUAL);
	fastcache_del(fc, NULL, -1);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);

	test_assert(fastcache_add(fc, NULL, 1, 1024 TB, 1 MB), FALSE, 'd', TEST_EQUAL);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);
	fastcache_del(fc, NULL, -1);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);

	test_assert(fastcache_add(fc, name1, 1, 1 MB, 1 MB), TRUE, 'd', TEST_EQUAL);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 31, 'd', TEST_EQUAL);
	fastcache_del(fc, name1, -1);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);

	test_assert(fastcache_add(fc, name1, 1, 1024 TB, 1 MB), FALSE, 'd', TEST_EQUAL);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);
	fastcache_del(fc, name1, -1);
	test_assert(fc->num, 0, 'd', TEST_EQUAL);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);

	fastcache_destroy(fc);

	file_delete(name1);
	file_delete(index_name1);


	char *name2 = "test/fastcache/test2";
	char *index_name2 = "test/fastcache/test2.__index__";
	file_delete(name2);
	file_delete(index_name2);

	fc = fastcache_init();

	test_assert(fastcache_add(fc, NULL, 1, 1 MB, 10 MB), TRUE, 'd', TEST_EQUAL);
	test_assert(fc->max, 31, 'd', TEST_EQUAL);
	test_assert(fastcache_add(fc, name2, 1, 1 MB, 20 MB), TRUE, 'd', TEST_EQUAL);
	test_assert(fc->max, 62, 'd', TEST_EQUAL);
	test_assert(fastcache_add(fc, NULL, 2, 1 MB, 30 MB), TRUE, 'd', TEST_EQUAL);
	test_assert(fc->max, 93, 'd', TEST_EQUAL);

	package *p;
	int i, j;
	for(i = 1; i < 100; i++)
	{
		p = package_malloc();
		p->code.id = i;
		p->task.id = i;
		p->info.id = i;
		for(j = 0; j < sizeof(p->data) / sizeof(__int64); j++)
			p->data[j] = i;
		if(fastcache_write(fc, p) == FALSE)
			package_free(p);
	}

	tools_sleep(500 MS);

	test_assert(fc->num, 93, 'd', TEST_EQUAL);

	for(i = 1; i < 100; i += 20)
	{
		__int64 index = ((__int64)i << 32) + i;
		p = fastcache_read(fc, index);
		if(!p)
			continue;
		test_assert(p->code.id, i, 'd', TEST_EQUAL);
		test_assert(p->task.id, i, 'd', TEST_EQUAL);
		test_assert(p->info.id, i, 'd', TEST_EQUAL);
		package_free(p);

		test_assert(fastcache_clean(fc, index), TRUE, 'd', TEST_EQUAL);
		test_assert(fastcache_read(fc, index), NULL, 'p', TEST_EQUAL);
	}

	test_assert(fc->num, 88, 'd', TEST_EQUAL);

	for(i = 2; i < 21; i++)
	{
		for(j = i; j < 100; j += 20)
			fastcache_clean(fc, ((__int64)j << 32) + j);
		if(i < 14)
			test_assert(fc->num, 93 - 5 * i, 'd', TEST_EQUAL);
		else
			test_assert(fc->num, 80 - 4 * i, 'd', TEST_EQUAL);
	}

	test_assert(fc->num, 0, 'd', TEST_EQUAL);

	fastcache_del(fc, NULL, 1);
	test_assert(fc->max, 62, 'd', TEST_EQUAL);
	fastcache_del(fc, name2, 1);
	test_assert(fc->max, 31, 'd', TEST_EQUAL);
	fastcache_del(fc, NULL, 2);
	test_assert(fc->max, 0, 'd', TEST_EQUAL);

	fastcache_destroy(fc);

	file_delete(name2);
	file_delete(index_name2);
}

#endif
