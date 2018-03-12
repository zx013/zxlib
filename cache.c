#include "cache.h"


static dict *d = NULL;

cache_client *cache_client_open(char *filename, __int64 filesize, int blocksize)
{
	cache_client *cc;
	int len = strlen(filename);

	cc = lib_calloc(sizeof(cache_client));

	cc->filename = lib_calloc(len + 1);
	sprintf(cc->filename, "%s", filename);
	cc->cachefile = lib_calloc(len + sizeof(CACHE_CLIENT_CACHE) + 1);
	sprintf(cc->cachefile, "%s%s", filename, CACHE_CLIENT_CACHE);
	cc->statefile = lib_calloc(len + sizeof(CACHE_CLIENT_STATE) + 1);
	sprintf(cc->statefile, "%s%s", filename, CACHE_CLIENT_STATE);

	if(!access(cc->cachefile, 0) && !access(cc->statefile, 0)) //有缓存文件
	{
		cc->cachefp = file_open(cc->cachefile);

		cc->statefp = file_open(cc->statefile);
		file_read(cc->statefp, &cc->filesize, sizeof(cc->filesize));
		file_read(cc->statefp, &cc->blocksize, sizeof(cc->blocksize));
		file_read(cc->statefp, &cc->blocknum, sizeof(cc->blocknum));

		cc->block = lib_calloc(cc->blocknum * sizeof(cache_block));
		file_read(cc->statefp, cc->block, cc->blocknum * sizeof(cache_block));

		cc->num = 0;
		int i;
		for(i = 0; i < cc->blocknum; i++)
			cc->num += cc->block[i].sign;
	}
	else //没有缓存文件
	{
		file_create(cc->cachefile, filesize);
		cc->cachefp = file_open(cc->cachefile);

		file_create(cc->statefile, 0);
		cc->statefp = file_open(cc->statefile);

		cc->filesize = filesize;
		cc->blocksize = blocksize;
		cc->blocknum = (cc->filesize - 1) / cc->blocksize + 1;
		cc->num = 0;

		file_write(cc->statefp, &cc->filesize, sizeof(cc->filesize));
		file_write(cc->statefp, &cc->blocksize, sizeof(cc->blocksize));
		file_write(cc->statefp, &cc->blocknum, sizeof(cc->blocknum));

		cc->block = lib_calloc(cc->blocknum * sizeof(cache_block));
		file_write(cc->statefp, cc->block, cc->blocknum * sizeof(cache_block));
	}
	file_seek(cc->statefp, 0, FILE_SEEK_SET);
	return cc;
}

int cache_client_close(cache_client *cc)
{
	file_close(cc->cachefp);
	rename(cc->cachefile, cc->filename);
	lib_free(cc->cachefile);

	file_close(cc->statefp);
	remove(cc->statefile);
	lib_free(cc->statefile);

	lib_free(cc->block);
	dict_del(d, cc->filename);

	lib_free(cc->filename);

	lib_free(cc);
}

/*
只有缓存时，重新生成状态文件
只有状态时，清空状态文件
*/
cache_client *cache_client_init(char *filename, __int64 filesize, int blocksize)
{
	cache_client *cc;
	if(d == NULL)
		d = dict_new();

	cc = dict_get(d, filename);
	if(!cc)
	{
		cc = cache_client_open(filename, filesize, blocksize);
		dict_set(d, cc->filename, cc);
	}
	return cc;
}

int cache_client_update(cache_client *cc, __int64 id)
{
	cache_block block;
	file_seek(cc->statefp, CACHE_CLIENT_OFFSET_BLOCK + id, FILE_SEEK_SET);
	file_read(cc->statefp, &block, sizeof(cache_block));

	if(block.sign == 0)
	{
		block.sign = 1;

		cc->block[id] = block;
		file_seek(cc->statefp, CACHE_CLIENT_OFFSET_BLOCK + id, FILE_SEEK_SET);
		file_write(cc->statefp, &block,  sizeof(cache_block));

		cc->num++;
	}

	if(cc->num == cc->blocknum)
		cache_client_close(cc);
}

int cache_client_write(cache_client *cc, void *data, __int64 id)
{
	int offset = id * cc->blocksize;
	int len = cc->blocksize;

	file_seek(cc->cachefp, offset, FILE_SEEK_SET);
	if(len > cc->filesize - offset)
		len = cc->filesize - offset;
	file_write(cc->cachefp, data, len);
	cache_client_update(cc, id);
}


#ifdef TEST

#include "test.h"

void cache_test()
{
}

#endif
