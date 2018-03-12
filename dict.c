#include "dict.h"


dict *dict_new_0()
{
	return dict_new_1(DICT_NONE);
}

dict *dict_new_1(int type)
{
	dict *d;
	d = lib_malloc(sizeof(dict), dict_free);
	if(unlikely(!d))
		error_goto(dict_new_error_1);

	switch(type)
	{
	case DICT_NONE: case DICT_DATA:
		d->type = type;
		d->data = NULL;
		break;
	case DICT_LIST:
		d->type = DICT_LIST;
		d->data = dict_list_new();
		if(unlikely(!d->data))
			error_goto(dict_new_error_2);
		break;
	case DICT_HASH:
		d->type = DICT_HASH;
		d->data = dict_hash_new();
		if(unlikely(!d->data))
			error_goto(dict_new_error_2);
		break;
	default:
		d->type = DICT_NONE;
		d->data = NULL;
		break;
	}

	return d;

dict_new_error_2:
	lib_free(d);
dict_new_error_1:
	return NULL;
}

int dict_free(dict *d)
{
	lib_autofree(d->data);
	lib_free(d);
}

dict *dict_pack_1_imple(int datatype, void *datavalue)
{
	dict *d = NULL;
	switch(major_type(datatype))
	{
	case DICT_DICT:
		d = (dict *)datavalue;
		break;
	default:
		d = dict_new();
		d->type = datatype;
		d->data = datavalue;
		break;
	}
	return d;
}

dict *dict_pack_2_imple(dict *d, int datatype, void *datavalue)
{
	lib_autofree(d->data);
	switch(major_type(datatype))
	{
	case DICT_DICT:
		*d = *(dict *)datavalue;
		lib_free(datavalue);
		break;
	default:
		d->type = datatype;
		d->data = datavalue;
		break;
	}
	return d;
}

int dict_equal_imple(dict *d, int datatype, void *datavalue)
{
	if(unlikely(!d))
		return FALSE;

	if(major_type(datatype) == DICT_DICT)
	{
		datatype = ((dict *)datavalue)->type;
		datavalue = ((dict *)datavalue)->data;
	}

	if(major_type(datatype) != major_type(d->type))
		return FALSE;

	switch(major_type(d->type))
	{
	case DICT_INT64:
		if(d->data != datavalue)
			return FALSE;
		break;
	case DICT_STR:
		if(strcmp((char *)d->data, (char *)datavalue))
			return FALSE;
		break;
	default:
		if(d->data != datavalue)
			return FALSE;
		break;
	}
	return TRUE;
}

int dict_each(dict *d, void *func)
{
	switch(major_type(d->type))
	{
	case DICT_LIST:
		return dict_list_each(d->data, func);
	case DICT_HASH:
		return dict_hash_each(d->data, func);
	case DICT_DICT:
		return dict_each(d->data, func);
	default:
		break;
	}
	return TRUE;
}


dict_list *dict_list_new()
{
	dict_list *dl;

	dl = lib_malloc(sizeof(dict_list), dict_list_free);
	if(unlikely(!dl))
		error_goto(dict_list_new_error_1);

	dl->head = NULL;
	dl->tail = NULL;
	dl->size = 0;
	dl->lock = 0;
	dl->fast.node = NULL;
	dl->fast.num = -1;
	dl->fast.lock = 0;

	return dl;

dict_list_new_error_1:
	return NULL;
}

int dict_list_free(dict_list *dl)
{
	while(dl->head)
		lib_autofree(dict_list_pop(dl));
	lib_free(dl);
}

static int __dict_list_push_3_imple(dict_list *dl, int datatype, void *datavalue, int num)
{
	dict_list_node *dln, *__dln;

	__dln = lib_malloc(sizeof(dict_list_node));
	__dln->data = dict_pack_1_imple(datatype, datavalue);

	if(num > dl->size - 1)
	{
		__dln->next = NULL;
		__dln->prev = NULL;

		if(!dl->head)
			dl->head = __dln;
		else
		{
			dl->tail->next = __dln;
			__dln->prev = dl->tail;
		}
		dl->tail = __dln;
	}
	else
	{
		if(num < -dl->size)
			dln = dl->head;
		else
			dln = dict_list_get_node(dl, num);

		__dln->next = dln;
		__dln->prev = dln->prev;

		if(dln->prev)
			dln->prev->next = __dln;
		else
			dl->head = __dln;

		dln->prev = __dln;

		with_lock(dl->fast.lock, {
			dl->fast.node ? __sync_fetch_and_add(&dl->fast.num, 1) : 0;
		});
	}

	dl->size++;
	return TRUE;
}

int dict_list_push_3_imple(dict_list *dl, int datatype, void *datavalue, int num)
{
	return with_wlock(dl->lock, __dict_list_push_3_imple(dl, datatype, datavalue, num));
}

static void *__dict_list_pop_3_imple(dict_list *dl, int num, void *error)
{
	dict_list_node *dln;
	void *data;

	if(unlikely(!dl->head))
		return error;

	if(num < -dl->size || num == 0)
	{
		dln = dl->head;
		dl->head = dln->next;

		if(!dl->head)
			dl->tail = NULL;
		else
			dl->head->prev = NULL;

		dl->size--;
	}
	else
	{
		if(num > dl->size - 1)
			dln = dl->tail;
		else
			dln = dict_list_get_node(dl, num);

		if(dln->next)
			dln->next->prev = dln->prev;
		else
			dl->tail = dln->prev;

		if(dln->prev)
			dln->prev->next = dln->next;
		else
			dl->head = dln->next;

		dl->size--;

		with_lock(dl->fast.lock, __sync_lock_test_and_set(&dl->fast.node, dln->next));
		//dl->fast_node = NULL;
		//dl->fast_num = -1;
	}

	data = dict_data(dln->data);
	lib_flag_sub(dln->data);
	lib_free(dln);
	return data;
}

void *dict_list_pop_3_imple(dict_list *dl, int num, void *error)
{
	return with_wlock(dl->lock, __dict_list_pop_3_imple(dl, num, error));
}

static dict_list_node *dict_list_get_node(dict_list *dl, int num)
{
	dict_list_node *dln_s, *dln_e, *dln;
	int num_s, num_e;
	int size;
	int i;

	size = dict_list_size(dl);

	//超出范围
	if(num > size - 1)
		return NULL;
	if(num < -size)
		return NULL;
	if(num < 0)
		num += size;

	dln_s = dl->head;
	num_s = 0;
	dln_e = dl->tail;
	num_e = size - 1;

	lock(dl->fast.lock);
	if(likely(dl->fast.node))
	{
		//减小查找范围
		if(num > dl->fast.num)
		{
			dln_s = dl->fast.node->next;
			num_s = dl->fast.num + 1;
		}
		else if(num < dl->fast.num)
		{
			dln_e = dl->fast.node->prev;
			num_e = dl->fast.num - 1;
		}
		else
		{
			dln = dl->fast.node;
			unlock(dl->fast.lock);
			return dln;
		}
	}
	unlock(dl->fast.lock);

	//从较近的一侧查找
	if(num > ((num_s + num_e) >> 1))
		for(dln = dln_e, i = num_e; likely(dln) && i > num; dln = dln->prev, i--) ;
	else
		for(dln = dln_s, i = num_s; likely(dln) && i < num; dln = dln->next, i++) ;

	//插入删除时也会改变这两个值
	with_lock(dl->fast.lock, {
		__sync_lock_test_and_set(&dl->fast.node, dln);
		__sync_lock_test_and_set(&dl->fast.num, num);
	});

	return dln;
}

static dict *__dict_list_get_dict(dict_list *dl, int num)
{
	dict_list_node *dln;
	dln = dict_list_get_node(dl, num);
	if(unlikely(!dln))
		return NULL;
	return dln->data;
}

dict *dict_list_get_dict(dict_list *dl, int num)
{
	return with_rlock(dl->lock, __dict_list_get_dict(dl, num));
}

static int __dict_list_set_imple(dict_list *dl, int num, int datatype, void *datavalue)
{
	dict_list_node *dln;
	dln = dict_list_get_node(dl, num);
	if(unlikely(!dln))
		return FALSE;
	dict_pack_2_imple(dln->data, datatype, datavalue);
	return TRUE;
}

int dict_list_set_imple(dict_list *dl, int num, int datatype, void *datavalue)
{
	return with_wlock(dl->lock, __dict_list_set_imple(dl, num, datatype, datavalue));
}

static dict *__dict_list_get_3_imple(dict_list *dl, int num, void *error)
{
	dict_list_node *dln;
	dln = dict_list_get_node(dl, num);
	if(unlikely(!dln))
		return error;
	return dict_data(dln->data);
}

dict *dict_list_get_3_imple(dict_list *dl, int num, void *error)
{
	return with_rlock(dl->lock, __dict_list_get_3_imple(dl, num, error));
}

static int __dict_list_del(dict_list *dl, int num)
{
	dict_list_node *dln;
	dln = dict_list_get_node(dl, num);
	if(unlikely(!dln))
		return FALSE;

	if(dln->next)
		dln->next->prev = dln->prev;
	else
		dl->tail = dln->prev;

	if(dln->prev)
		dln->prev->next = dln->next;
	else
		dl->head = dln->next;

	dl->size--;

	with_lock(dl->fast.lock, __sync_lock_test_and_set(&dl->fast.node, dln->next));

	lib_flag_sub(dln->data);
	lib_free(dln);
}

int dict_list_del(dict_list *dl, int num)
{
	return with_wlock(dl->lock, __dict_list_del(dl, num));
}

static int __dict_list_each_2(dict_list *dl, void *func)
{
	dict_list_node *dln;
	dict_list_node *dln_next;
	int num = 0;

	for(dln = dl->head; dln; dln = dln_next)
	{
		dln_next = dln->next;
		if((*(int(*)(dict *, int))func)(dln->data, num) == FALSE)
			return FALSE;
		num++;
	}
	return TRUE;
}

int dict_list_each_2(dict_list *dl, void *func)
{
	return with_wlock(dl->lock, disable_rwlock(dl->lock, __dict_list_each_2(dl, func)));
}

int dict_list_each_4(dict_list *dl, int start, int end, void *func)
{
	return dict_list_each_5(dl, start, end, 1, func);
}

static int __dict_list_each_5(dict_list *dl, int start, int end, int step, void *func)
{
	dict_list_node *dln;
	int size = dict_list_size(dl);
	int num;
	int i, j;

	if(step == 0)
		return TRUE;

	if(start > size - 1)
		start = size - 1;
	else if(start < -size)
		start = -size;
	if(start < 0)
		start += size;

	if(end > size)
		end = size;
	else if(end < -size - 1)
		end = -size - 1;
	if(end < 0)
		end += size;

	//通过上面的限制将参数限制在范围内，不需要进行dln是否是NULL的判断
	//如果在遍历时允许增删操作，则需要加入判断
	num = 0;
	if(end > start && step > 0)
	{
		for(dln = dl->head, i = 0; i < start; i++)
			dln = dln->next;
		for(i = start; i < end; i += step)
		{
			if((*(int(*)(dict *, int))func)(dln->data, num) == FALSE)
				return FALSE;
			for(j = 0; j < step; j++)
				dln = dln->next;
			num++;
		}
	}
	else if(end < start && step < 0)
	{
		for(dln = dl->tail, i = size - 1; i > start; i--)
			dln = dln->prev;
		for(i = start; i > end; i += step)
		{
			if((*(int(*)(dict *, int))func)(dln->data, num) == FALSE)
				return FALSE;
			for(j = 0; j > step; j--)
				dln = dln->prev;
			num++;
		}
	}
	return TRUE;
}

int dict_list_each_5(dict_list *dl, int start, int end, int step, void *func)
{
	return with_wlock(dl->lock, disable_rwlock(dl->lock, __dict_list_each_5(dl, start, end, step, func)));
}

dict_list *dict_list_slice_3(dict_list *dl, int start, int end)
{
	return dict_list_slice_4(dl, start, end, 1);
}

dict_list *dict_list_slice_4(dict_list *dl, int start, int end, int step)
{
	dict_list *dl_slice;
	dl_slice = dict_list_new();

	//dl_slice不需要额外加锁
	dict_list_each_5(dl, start, end, step, lambda((dict *d), {
		__dict_list_push_3_imple(dl_slice, DICT_DICT, d, dl_slice->size);
		lib_flag_add(d);
	}));

	return dl_slice;
}

static int __dict_list_attach(dict_list *dl1, dict_list *dl2)
{
	return __dict_list_each_2(dl2, lambda((dict *d), __dict_list_push_3_imple(dl1, DICT_DICT, d, dl1->size)));
}

int dict_list_attach(dict_list *dl1, dict_list *dl2)
{
	return with_wlock(dl1->lock, with_wlock(dl2->lock, __dict_list_attach(dl1, dl2)));
}



dict_hash *dict_hash_new_1_imple(int max)
{
	dict_hash *dh;
	int i;

	dh = lib_malloc(sizeof(dict_hash), dict_hash_free);
	if(unlikely(!dh))
		error_goto(dict_hash_new_error_1);

	dh->max = max;
	dh->size = 0;
	dh->lock = 0;

	dh->list = lib_calloc(dh->max * sizeof(dict_hash_node *));
	if(unlikely(!dh->list))
		error_goto(dict_hash_new_error_2);

	return dh;

dict_hash_new_error_2:
	lib_free(dh);
dict_hash_new_error_1:
	return NULL;
}

int dict_hash_free(dict_hash *dh)
{
	int i;

	//释放key和data
	dict_hash_each(dh, lambda((dict *key, dict *data), {
		dict_free(key);
		dict_free(data);
	}));

	//释放list
	for(i = 0; i < dh->max; i++)
		if(dh->list[i])
			dict_list_free(dh->list[i]);
	lib_free(dh->list);
	lib_free(dh);
}

unsigned __int64 dict_hash_map_2_imple(int datatype, void *datavalue)
{
	unsigned __int64 hash;
	char *key;
	int i;

	if(major_type(datatype) == DICT_DICT)
	{
		datatype = ((dict *)datavalue)->type;
		datavalue = ((dict *)datavalue)->data;
	}

	//可再加上一个初始化时生成的随机种子，保证随机性
	hash = (major_type(datatype) << IS_OFFSET);
	key = (char *)datavalue;

	if(major_type(datatype) == DICT_STR)
	{
		while(*key)
			hash = (hash << 5) + hash + *key++;
	}
	else
		hash = (hash << 5) + hash + (unsigned __int64)datavalue;

	return hash;
}

static int dict_hash_rehash(dict_hash *dh)
{
	dict_list *dl, *__dl;
	dict_hash_node *dhn, *__dhn;
	int keyhash, __keyhash;
	void *ptr;
	int i;

	ptr = lib_realloc(dh->list, (dh->max << 1) * sizeof(dict_hash_node *));
	if(!ptr)
		return FALSE;

	dh->list = ptr;
	for(i = dh->max; i < dh->max << 1; i++)
		dh->list[i] = 0;
	dh->max <<= 1;

	for(keyhash = 0; keyhash < dh->max; keyhash++)
	{
		dl = dh->list[keyhash];
		if(!dl)
			continue;

		__dict_list_each_2(dl, lambda((dict *d, int num), {
			dhn = d->data;
			__keyhash = dict_hash_map_2_imple(dhn->key->type, dhn->key->data) % dh->max;
			if(keyhash != __keyhash)
			{
				__dl = dh->list[__keyhash];
				if(!__dl)
				{
					__dl = dict_list_new();
					dh->list[__keyhash] = __dl;
				}
				__dhn = __dict_list_pop_3_imple(dl, num, NULL);
				__dict_list_push_3_imple(__dl, dict_type(__dhn), __dhn, __dl->size);
			}
		}));
	}
	return TRUE;
}

static dict_hash_node *__dict_hash_get_node(dict_hash *dh, int keytype, void *keyvalue)
{
	dict_list *dl;
	dict_hash_node *dhn;
	int keyhash;

	if(unlikely(!dh))
		return NULL;

	keyhash = dict_hash_map_2_imple(keytype, keyvalue) % dh->max;
	dl = dh->list[keyhash];
	if(!dl)
		return NULL;

	if(__dict_list_each_2(dl, lambda((dict *d), {
		dhn = d->data;
		if(dict_equal_imple(dhn->key, keytype, keyvalue) == TRUE)
			return FALSE;
	})) == TRUE)
		return NULL;
	else
		return dhn;
}

static dict_hash_node *dict_hash_get_node(dict_hash *dh, int keytype, void *keyvalue)
{
	return with_rlock(dh->lock, __dict_hash_get_node(dh, keytype, keyvalue));
}

dict *dict_hash_get_dict(dict_hash *dh, int keytype, void *keyvalue)
{
	dict_hash_node *dhn;
	dhn = dict_hash_get_node(dh, keytype, keyvalue);
	if(unlikely(!dhn))
		return NULL;
	return dhn->data;
}

static void *__dict_hash_set_node(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue)
{
	dict_list *dl;
	dict_hash_node *dhn;
	int keyhash;

	if(unlikely(!dh))
		return NULL;

	keyhash = dict_hash_map_2_imple(keytype, keyvalue) % dh->max;
	dl = dh->list[keyhash];
	if(!dl) //no key exist
	{
		dl = dict_list_new();
		dh->list[keyhash] = dl;
	}

	if(__dict_list_each_2(dl, lambda((dict *d), {
		dhn = d->data;
		if(dict_equal_imple(dhn->key, keytype, keyvalue) == TRUE)
			return FALSE;
	})) == TRUE) //没有对应的key
	{
		dhn = lib_calloc(sizeof(dict_hash_node));
		dhn->key = dict_pack_1_imple(keytype, keyvalue);
		dhn->data = dict_pack_1_imple(datatype, datavalue);
		__dict_list_push_3_imple(dl, dict_type(dhn), dhn, dl->size);
		dh->size++;

		//不是每次超出都rehash，防止内存不够时一直rehash影响性能
		if(((dh->max >> 1) < dh->size) & !(dh->size % (dh->max >> 3)))
			dict_hash_rehash(dh);
	}
	else
	{
		dict_pack_2_imple(dhn->key, keytype, keyvalue); //防止重复的dict的key值
		dict_pack_2_imple(dhn->data, datatype, datavalue);
	}

	return dhn;
}

static void *dict_hash_set_node(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue)
{
	return with_wlock(dh->lock, __dict_hash_set_node(dh, keytype, keyvalue, datatype, datavalue));
}

dict *dict_hash_set_dict(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue)
{
	dict_hash_node *dhn;
	dhn = dict_hash_set_node(dh, keytype, keyvalue, datatype, datavalue);
	if(unlikely(!dhn))
		return NULL;
	return dhn->data;
}

void *dict_hash_set_imple(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue)
{
	dict_hash_node *dhn;
	dhn = dict_hash_set_node(dh, keytype, keyvalue, datatype, datavalue);
	if(unlikely(!dhn))
		return NULL;
	return dict_data(dhn->data);
}

void *dict_hash_get_3_imple(dict_hash *dh, int keytype, void *keyvalue, void *error)
{
	dict_hash_node *dhn;
	dhn = dict_hash_get_node(dh, keytype, keyvalue);
	if(unlikely(!dhn))
		return error;
	return dict_data(dhn->data);
}

static int __dict_hash_del_imple(dict_hash *dh, int keytype, void *keyvalue)
{
	dict_list *dl;
	dict_hash_node *dhn;
	int keyhash;

	if(unlikely(!dh))
		return FALSE;

	keyhash = dict_hash_map_2_imple(keytype, keyvalue) % dh->max;
	dl = dh->list[keyhash];
	if(!dl)
		return FALSE;

	if(__dict_list_each_2(dl, lambda((dict *d, int num), {
		dhn = d->data;
		if(dict_equal_imple(dhn->key, keytype, keyvalue) == TRUE)
		{
			__dict_list_del(dl, num);
			dict_free(dhn->data);
			dict_free(dhn->key);
			lib_free(dhn);

			dh->size--;
			return FALSE;
		}
	})) == TRUE)
		return FALSE;

	return TRUE;
}

int dict_hash_del_imple(dict_hash *dh, int keytype, void *keyvalue)
{
	return with_wlock(dh->lock, __dict_hash_del_imple(dh, keytype, keyvalue));
}

static int __dict_hash_each(dict_hash *dh, void *func)
{
	dict_list *dl;
	dict_hash_node *dhn;
	int num;
	int i;

	num = 0;
	for(i = 0; i < dh->max; i++)
	{
		dl = dh->list[i];
		if(!dl)
			continue;

		__dict_list_each_2(dl, lambda((dict *d, int n), {
			dhn = d->data;
			if((*(int(*)(dict *, dict *, int))func)(dhn->key, dhn->data, num) == FALSE)
				return FALSE;
			num++;
		}));
	}
	return TRUE;
}

int dict_hash_each(dict_hash *dh, void *func)
{
	return with_wlock(dh->lock, disable_rwlock(dh->lock, __dict_hash_each(dh, func)));
}



//去除开头的空格和结尾的多余，当json部分出错时尽量将该错误忽略
static char *dict_json_skip(char *str, int type)
{
	if(unlikely(!str))
		return NULL;

	switch(type)
	{
	case JSON_SKIP_SPACE:
		while(*str && (unsigned char)*str <= 32)
			str++;
		break;
	case JSON_SKIP_DATA:
		while(*str)
			str++;
		break;
	case JSON_SKIP_LIST:
		while(*str && *str != ',' && *str != ']')
			str++;
		break;
	case JSON_SKIP_HASH:
		while(*str && *str != ',' && *str != ':' && *str != '}')
			str++;
		break;
	}
	return str;
}

static unsigned dict_json_hex4(const char *str)
{
	unsigned h = 0;
	char c;
	int i;

	for(i = 0; i < 4; i++)
	{
		c = *str++;
		switch(c)
		{
		case '0' ... '9':
			h += c - '0';
			break;
		case 'A' ... 'F':
			h += 10 + c - 'A';
			break;
		case 'a' ... 'z':
			h += 10 + c - 'a';
			break;
		default:
			return 0;
		}
		h <<= 4;
	}
}

__int64 dict_json_size(dict *d)
{
	int size = 0;
	char buffer[128];
	double g;
	ansi *s;

	switch(major_type(d->type))
	{
	case DICT_NONE:
		size += 4;
		break;
	case DICT_INT64:
		size += sprintf(buffer, "%lld", d->data);
		break;
	case DICT_DOUBLE:
		g = castdouble(d->data);
		if (fabs(floor(g) - g) <= DBL_EPSILON && fabs(g) < 1.0e60)
			size += sprintf(buffer, "%g.0", g);
		else
			size += sprintf(buffer, "%g", g);
		break;
	case DICT_STR:
		s = d->data;
		while(*s) //根据utf8计算转换成unicode后的json长度
		{
			if(*s < 0x80)
			{
				switch(*s)
				{
				case '"': case '\\': case '\b': case '\f': case '\n': case '\r': case '\t':
					size++;
				}
			}
			else
			{
				size += 5;
				if((*s & 0xE0) == 0xC0)
					s++;
				else if((*s & 0xF0) == 0xE0)
					s += 2;
			}
			size++;
			s++;
		}
		size += 2;
		break;
	case DICT_OTHER:
		size += 4;
		break;
	case DICT_LIST:
		if(dict_list_size(d->data))
		{
			__dict_list_each_2(d->data, lambda((dict *d), {
				size += dict_json_size(d) + 2;
			}));
			size -= 2;
		}
		size += 2;
		break;
	case DICT_HASH:
		if(dict_hash_size(d->data))
		{
			__dict_hash_each(d->data, lambda((dict *key, dict *data), {
				switch(major_type(key->type))
				{
				case DICT_INT64: case DICT_DOUBLE:
					size += dict_json_size(key) + dict_json_size(data) + 6;
					break;
				case DICT_STR:
					size += dict_json_size(key) + dict_json_size(data) + 4;
					break;
				}
			}));
			size -= 2;
		}
		size += 2;
		break;
	default:
		size += 4;
		break;
	}
	return size;
}

char *dict_json_dump(dict *d)
{
	char *addr;

	addr = lib_calloc(dict_json_size(d) + 1);
	__dict_json_dump(d, addr);
	return addr;
}

static char *__dict_json_dump(dict *d, char *addr)
{
	double g;
	unicode *s, *t;
	switch(major_type(d->type))
	{
	case DICT_NONE:
		addr += sprintf(addr, "null");
		break;
	case DICT_INT64:
		addr += sprintf(addr, "%lld", d->data);
		break;
	case DICT_DOUBLE:
		g = castdouble(d->data);
		if (fabs(floor(g) - g) <= DBL_EPSILON && fabs(g) < 1.0e60)
			addr += sprintf(addr, "%g.0", g);
		else
			addr += sprintf(addr, "%g", g);
		break;
	case DICT_STR:
		s = t = decode_utf8(d->data);
		addr += sprintf(addr, "\"");
		while(*s)
		{
			switch(*s)
			{
			case '"': addr += sprintf(addr, "\\\""); break;
			case '\\': addr += sprintf(addr, "\\\\"); break;
			case '\b': addr += sprintf(addr, "\\b"); break;
			case '\f': addr += sprintf(addr, "\\f"); break;
			case '\n': addr += sprintf(addr, "\\n"); break;
			case '\r': addr += sprintf(addr, "\\r"); break;
			case '\t': addr += sprintf(addr, "\\t"); break;
			default:
				if(*s >= 32 && *s <= 126)
					addr += sprintf(addr, "%c", *s);
				else if(*s >= 128)
					addr += sprintf(addr, "\\u%04x", *s);
			}
			s++;
		}
		addr += sprintf(addr, "\"");
		lib_free(t);
		break;
	case DICT_OTHER:
		addr += sprintf(addr, "null");
		break;
	case DICT_LIST:
		addr += sprintf(addr, "[");
		if(dict_list_size(d->data))
		{
			__dict_list_each_2(d->data, lambda((dict *d), {
				addr = __dict_json_dump(d, addr);
				addr += sprintf(addr, ", ");
			}));
			addr -= sprintf(addr, "\b\b");
		}
		addr += sprintf(addr, "]");
		break;
	case DICT_HASH:
		addr += sprintf(addr, "{");
		if(dict_hash_size(d->data))
		{
			__dict_hash_each(d->data, lambda((dict *key, dict *data), {
				switch(major_type(key->type))
				{
				case DICT_INT64: case DICT_DOUBLE:
					addr += sprintf(addr, "\"");
					addr = __dict_json_dump(key, addr);
					addr += sprintf(addr, "\"");

					addr += sprintf(addr, ": ");
					addr = __dict_json_dump(data, addr);
					addr += sprintf(addr, ", ");
					break;
				case DICT_STR:
					addr = __dict_json_dump(key, addr);

					addr += sprintf(addr, ": ");
					addr = __dict_json_dump(data, addr);
					addr += sprintf(addr, ", ");
					break;
				}
			}));
			addr -= sprintf(addr, "\b\b");
		}
		addr += sprintf(addr, "}");
		break;
	default:
		addr += sprintf(addr, "null");
		break;
	}
	return addr;
}

dict *dict_json_load(char *s)
{
	dict *d;
	d = __dict_json_load(&s);
	return d ? d : dict_new();
}

static dict *__dict_json_load(char **addr)
{
	dict *d = NULL;
	dict *key = NULL, *data = NULL;

	char *str = NULL;
	int len = 0;
	int u1, u2;

	int integer = 0;
	int decimal = 0;
	int scale = 1;
	int sign = 1;

	int factorial = 0;
	int fsign = 1;

	//去除开头的空格
	*addr = dict_json_skip(*addr, JSON_SKIP_SPACE);

	switch(**addr)
	{
	case '{':
		d = dict_new(DICT_HASH);
		while(1)
		{
			if(unlikely(key))
				dict_free(key);
			if(unlikely(data))
				dict_free(data);

			*addr = dict_json_skip(*addr + 1, JSON_SKIP_SPACE);
			if(**addr == ',' || **addr == ':')
				continue;
			if(**addr == '}' || !**addr)
				break;

			key = __dict_json_load(addr);
			*addr = dict_json_skip(*addr, JSON_SKIP_HASH);
			if(!key)
				continue;
			if(**addr == ',' || **addr != ':')
				continue;
			if(**addr == '}' || !**addr)
				break;

			*addr = dict_json_skip(*addr + 1, JSON_SKIP_SPACE);
			data = __dict_json_load(addr);
			*addr = dict_json_skip(*addr, JSON_SKIP_HASH);
			if(!data)
				continue;
			if(likely(d))
			{
				//dict_hash_set(d->data, key, data);
				__dict_hash_set_node(d->data, DICT_DICT, key, DICT_DICT, data);
				key = NULL;
				data = NULL;
			}
			if(**addr == '}' || !**addr)
				break;
		}
		if(unlikely(key))
			dict_free(key);
		if(unlikely(data))
			dict_free(data);
		if(**addr == '}')
			(*addr)++;
		break;
	case '[':
		d = dict_new(DICT_LIST);
		do
		{
			if(unlikely(data))
				dict_free(data);

			*addr = dict_json_skip(*addr + 1, JSON_SKIP_SPACE);
			data = __dict_json_load(addr);
			*addr = dict_json_skip(*addr, JSON_SKIP_LIST);
			if(!data)
				break;
			if(likely(d))
			{
				//dict_list_push(d->data, data);
				__dict_list_push_3_imple(d->data, DICT_DICT, data, dict_list_size(d->data));
				data = NULL;
			}
		} while(**addr == ',');
		if(unlikely(data))
			dict_free(data);
		if(**addr == ']')
			(*addr)++;
		break;
	case '"':
		d = dict_new(DICT_STR);
		(*addr)++;
		str = *addr;
		len = 0;
		while(*str && *str != '"')
		{
			str += *str == '\\' ? 2 : 1;
			len++;
		}

		d->data = lib_malloc(len + 1);
		str = d->data;
		while(**addr && **addr != '"')
		{
			if(**addr == '\\')
			{
				(*addr)++;
				switch(**addr)
				{
				case 'b': *str++ = '\b'; break;
				case 'f': *str++ = '\f'; break;
				case 'n': *str++ = '\n'; break;
				case 'r': *str++ = '\r'; break;
				case 't': *str++ = '\t'; break;
				case 'u':
					u1 = dict_json_hex4(*addr + 1);
					(*addr) += 4;
					if((u1 >= 0xDC00 && u1 <= 0xDFFF) || u1 == 0)
						break;
					if(u1 >= 0xD800 && u1 <= 0xDBFF)
					{
						if((*addr)[1] != '\\' || (*addr)[2] != 'u')
							break;
						u2 = dict_json_hex4(*addr + 3);
						(*addr) += 6;
						if (u2 < 0xDC00 || u2 > 0xDFFF)
							break;
						u1 = 0x10000 + (((u1 & 0x3FF) << 10) | (u2 & 0x3FF));
					}
					len = 4;
					len = u1 < 0x80 ? 1 : u1 < 0x800 ? 2 : u1 < 0x10000 ? 3 : 4;
					str += len;
					switch(len)
					{
					case 4: *--str =((u1 | 0x80) & 0xBF); u1 >>= 6;
					case 3: *--str =((u1 | 0x80) & 0xBF); u1 >>= 6;
					case 2: *--str =((u1 | 0x80) & 0xBF); u1 >>= 6;
					case 1: *--str =(u1 | dict_json_mark[len]);
					}
					str += len;
					break;
				default: *str++ = **addr; break;
				}
				(*addr)++;
			}
			else
				*str++ = *(*addr)++;
		}
		*str = 0;
		break;
	case 't':
		d = dict_new();
		if(!strncmp(*addr, "true", 4))
		{
			(*addr) += 4;
			d->type = DICT_INT64;
			d->data = (void *)1;
		}
		break;
	case 'f':
		d = dict_new();
		if(!strncmp(*addr, "false", 5))
		{
			(*addr) += 5;
			d->type = DICT_INT64;
			d->data = (void *)0;
		}
		break;
	case 'n':
		d = dict_new();
		if(!strncmp(*addr, "null", 4))
		{
			(*addr) += 4;
		}
		break;
	case '-':
		sign = -1;
		(*addr)++;
	case '0' ... '9':
		d = dict_new();
		d->type = DICT_INT64;
		while(**addr >= '0' && **addr <= '9')
		{
			integer = integer * 10 + **addr - '0';
			(*addr)++;
		}
		integer *= sign;

		if(**addr == '.')
		{
			d->type = DICT_DOUBLE;
			(*addr)++;
			while(**addr >= '0' && **addr <= '9')
			{
				decimal = decimal * 10 + **addr - '0';
				scale = scale * 10;
				(*addr)++;
			}
		}

		if(**addr == 'e' || **addr == 'E')
		{
			d->type = DICT_DOUBLE;
			(*addr)++;
			switch(**addr)
			{
			case '-':
				fsign = -1;
			case '+':
				(*addr)++;
				break;
			}
			while(**addr >= '0' && **addr <= '9')
			{
				factorial = factorial * 10 + **addr - '0';
				(*addr)++;
			}
			factorial *= fsign;
		}

		if(d->type == DICT_INT64)
			d->data = (void *)integer;
		else
			d->data = cast(((double)integer + (double)decimal / (double)scale) * pow(10, factorial));
		break;
	}

	return d;
}


#ifdef TEST

#include "test.h"
#include "pool.h"
#include "queue.h"

//push and pop
void dict_list_1_test()
{
	dict_list *dl;

	dl = dict_list_new();

	test_assert(dict_list_size(dl), 0);
	dict_list_push(dl, 1);
	test_assert(dict_list_size(dl), 1);
	test_assert(dict_list_pop(dl), 1);

	test_assert(dict_list_size(dl), 0);
	dict_list_push(dl, '2');
	test_assert(dict_list_size(dl), 1);
	test_assert(dict_list_pop(dl), '2', 'c');

	test_assert(dict_list_size(dl), 0);
	dict_list_push(dl, "3");
	test_assert(dict_list_size(dl), 1);
	test_assert(dict_list_pop(dl), "3", 's');

	test_assert(dict_list_size(dl), 0);
	dict_list_push(dl, 1);
	test_assert(dict_list_size(dl), 1);
	dict_list_push(dl, '2');
	test_assert(dict_list_size(dl), 2);
	dict_list_push(dl, "3");

	test_assert(dict_list_size(dl), 3);
	test_assert(dict_list_pop(dl), 1);
	test_assert(dict_list_size(dl), 2);
	test_assert(dict_list_pop(dl), '2', 'c');
	test_assert(dict_list_size(dl), 1);
	test_assert(dict_list_pop(dl), "3", 's');
	test_assert(dict_list_size(dl), 0);

	dict_list_push(dl, 1);
	dict_list_push(dl, 2);
	dict_list_push(dl, 3);
	dict_list_push(dl, 4);
	dict_list_push(dl, 5);
	dict_list_push(dl, 6);

	test_assert(dict_list_pop(dl, 1), 2);
	test_assert(dict_list_pop(dl, 1), 3);
	test_assert(dict_list_pop(dl, 2), 5);
	test_assert(dict_list_pop(dl, 2), 6);
	test_assert(dict_list_pop(dl, 0), 1);
	test_assert(dict_list_pop(dl, 0), 4);

	dict_list_free(dl);
}

//list attach
void dict_list_2_test()
{
	dict_list *dl1, *dl2;

	dl1 = dict_list_new();
	dl2 = dict_list_new();

	dict_list_push(dl1, 1);
	dict_list_push(dl1, 2);
	dict_list_push(dl2, 3);
	dict_list_push(dl2, 4);

	test_assert(dict_list_size(dl1), 2);
	test_assert(dict_list_size(dl2), 2);
	dict_list_attach(dl1, dl2);
	test_assert(dict_list_size(dl1), 4);

	test_assert(dict_list_pop(dl1), 1);
	test_assert(dict_list_pop(dl1), 2);
	test_assert(dict_list_pop(dl1), 3);
	test_assert(dict_list_pop(dl1), 4);

	test_assert(dict_list_size(dl1), 0);
	test_assert(dict_list_size(dl2), 2);
	dict_list_attach(dl2, dl1);
	test_assert(dict_list_size(dl2), 2);

	dict_list_free(dl2);
	dict_list_free(dl1);
}

//auto free
void dict_list_3_test()
{
	dict_list *dl1, *dl2;
	char *s;
	pool *p;
	queue *q;

	dl1 = dict_list_new();
	dl2 = dict_list_new();

	s = lib_malloc(10);
	dict_list_push(dl1, s);

	p = pool_init(10);
	dict_list_push(dl1, p);

	q = queue_init(10);
	dict_list_push(dl1, q);

	//dict_new会导致hash释放不了
	dict *d;
	d = dict_pack(1);
	dict_list_push(dl1, d);

	d = dict_pack('1');
	dict_list_push(dl1, d);

	d = dict_pack("1");
	dict_list_push(dl1, d);

	dict_list_free(dl2);
	dict_list_free(dl1);
}

//slice
void dict_list_4_test()
{
	dict_list *dl1, *dl2;

	dl1 = dict_list_new();

	dict_list_push(dl1, 0);
	dict_list_push(dl1, 1);
	dict_list_push(dl1, 2);
	dict_list_push(dl1, 3);
	dict_list_push(dl1, 4);

	int ck[4][13][4] =
	{
		{
			{2, 0, 1},
			{2, 0, 1},
			{1, 1},
			{0},
			{0},
			{0},
			{2, 0, 1},
			{1, 1},
			{0},
			{0},
			{0},
			{0},
			{0}
		},
		{
			{0},
			{0},
			{0},
			{0},
			{1, 2},
			{2, 2, 3},
			{0},
			{0},
			{0},
			{1, 2},
			{2, 2, 3},
			{3, 2, 3, 4},
			{3, 2, 3, 4}
		},
		{
			{0},
			{0},
			{0},
			{0},
			{1, 3},
			{2, 4, 3},
			{0},
			{0},
			{0},
			{1, 3},
			{2, 4, 3},
			{2, 4, 3},
			{2, 4, 3}
		},
		{
			{3, 2, 1, 0},
			{2, 2, 1},
			{1, 2},
			{0},
			{0},
			{0},
			{2, 2, 1},
			{1, 2},
			{0},
			{0},
			{0},
			{0},
			{0}
		}
	};
	int i, j;

	for(i = -6; i < 7; i++)
	{
		dl2 = dict_list_slice(dl1, i, 2);
		test_assert(dict_list_size(dl2), ck[0][i + 6][0]);
		for(j = 0; j < ck[0][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[0][i + 6][j + 1]);
		dict_list_free(dl2);

		dl2 = dict_list_slice(dl1, i, 2, 1);
		test_assert(dict_list_size(dl2), ck[0][i + 6][0]);
		for(j = 0; j < ck[0][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[0][i + 6][j + 1]);
		dict_list_free(dl2);

		dl2 = dict_list_slice(dl1, 2, i);
		test_assert(dict_list_size(dl2), ck[1][i + 6][0]);
		for(j = 0; j < ck[1][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[1][i + 6][j + 1]);
		dict_list_free(dl2);

		dl2 = dict_list_slice(dl1, 2, i, 1);
		test_assert(dict_list_size(dl2), ck[1][i + 6][0]);
		for(j = 0; j < ck[1][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[1][i + 6][j + 1]);
		dict_list_free(dl2);

		dl2 = dict_list_slice(dl1, i, 2, -1);
		test_assert(dict_list_size(dl2), ck[2][i + 6][0]);
		for(j = 0; j < ck[2][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[2][i + 6][j + 1]);
		dict_list_free(dl2);

		dl2 = dict_list_slice(dl1, 2, i, -1);
		test_assert(dict_list_size(dl2), ck[3][i + 6][0]);
		for(j = 0; j < ck[3][i + 6][0]; j++)
			test_assert(dict_list_get(dl2, j), ck[3][i + 6][j + 1]);
		dict_list_free(dl2);
	}

	dict_list_free(dl1);
}

//slice auto free
void dict_list_5_test()
{
	dict_list *dl1, *dl2;
	char *s;
	pool *p;
	queue *q;

	dl1 = dict_list_new();

	s = lib_malloc(10);
	dict_list_push(dl1, s);

	p = pool_init(10);
	dict_list_push(dl1, p);

	q = queue_init(10);
	dict_list_push(dl1, q);

	dl2 = dict_list_slice(dl1, 0, 5);

	dict_list_free(dl1);
	dict_list_free(dl2);
}

//set, get, del
void dict_list_6_test()
{
	dict_list *dl;

	dl = dict_list_new();

	dict_list_push(dl, 1);
	dict_list_push(dl, 2);
	dict_list_push(dl, 3);
	dict_list_push(dl, 4);

	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 2);
	test_assert(dict_list_get(dl, 2), 3);
	test_assert(dict_list_get(dl, 3), 4);

	dict_list_set(dl, 1, 5);
	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 5);
	test_assert(dict_list_get(dl, 2), 3);
	test_assert(dict_list_get(dl, 3), 4);

	dict_list_set(dl, 2, 6);
	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 5);
	test_assert(dict_list_get(dl, 2), 6);
	test_assert(dict_list_get(dl, 3), 4);

	dict_list_del(dl, 2);
	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 5);
	test_assert(dict_list_get(dl, 2), 4);
	test_assert(dict_list_get(dl, 3, -1), -1);

	dict_list_del(dl, 2);
	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 5);
	test_assert(dict_list_get(dl, 2, -1), -1);

	dict_list_del(dl, 0);
	test_assert(dict_list_get(dl, 0), 5);
	test_assert(dict_list_get(dl, 1, -1), -1);

	dict_list_del(dl, 0);
	test_assert(dict_list_get(dl, 0, -1), -1);


	dict_list_push(dl, 1, 0);
	test_assert(dict_list_get(dl, 0), 1);

	dict_list_push(dl, 2, 10);
	test_assert(dict_list_get(dl, 0), 1);
	test_assert(dict_list_get(dl, 1), 2);

	dict_list_push(dl, 3, -10);
	test_assert(dict_list_get(dl, 0), 3);
	test_assert(dict_list_get(dl, 1), 1);
	test_assert(dict_list_get(dl, 2), 2);

	dict_list_push(dl, 4, 1);
	test_assert(dict_list_get(dl, 0), 3);
	test_assert(dict_list_get(dl, 1), 4);
	test_assert(dict_list_get(dl, 2), 1);
	test_assert(dict_list_get(dl, 3), 2);

	dict_list_free(dl);
}

//get after del(test fast_num)
void dict_list_7_test()
{
	dict_list *dl;
	int i, j, k;

	dl = dict_list_new();

	for(i = 0; i < 8; i++)
		dict_list_push(dl, i, i);

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			for(k = 0; k < 7; k++)
			{
				test_assert(dict_list_get(dl, i), i);
				dict_list_del(dl, j);
				if(k < j)
					test_assert(dict_list_get(dl, k), k);
				else
					test_assert(dict_list_get(dl, k), k + 1);
				dict_list_push(dl, j, j);
			}
		}
	}

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			for(k = 0; k < 7; k++)
			{
				test_assert(dict_list_get(dl, i), i);
				dict_list_push(dl, j, j);
				if(k <= j)
					test_assert(dict_list_get(dl, k), k);
				else
					test_assert(dict_list_get(dl, k + 1), k);
				dict_list_del(dl, j);
			}
		}
	}

	dict_list_free(dl);
}

//sync push
void dict_list_8_test_1(dict_list *dl)
{
	int i;
	for(i = 0; i < 1000; i++)
		dict_list_push(dl, i);
}

void dict_list_8_test()
{
	dict_list *dl;
	pool *p;
	int array[1000] = {0};
	int num = 10;
	int i;

	dl = dict_list_new();
	p = pool_init(num);

	for(i = 0; i < num; i++)
		pool_start(p, dict_list_8_test_1, dl);
	pool_join(p, NULL);

	for(i = 0; i < num * 1000; i++)
		array[(__int64)dict_list_get(dl, i)]++;
	for(i = 0; i < 1000; i++)
		test_assert(array[i], num);

	pool_destroy(p);
	dict_list_free(dl);
}

//sync get
void dict_list_9_test_1(dict_list *dl)
{
	int i;
	for(i = 0; i < 1000; i++)
		test_assert(dict_list_get(dl, i), i);
}

void dict_list_9_test()
{
	dict_list *dl;
	pool *p;
	int num = 10;
	int i;

	dl = dict_list_new();
	p = pool_init(num);

	for(i = 0; i < 1000; i++)
		dict_list_push(dl, i);

	for(i = 0; i < num; i++)
		pool_start(p, dict_list_9_test_1, dl);
	pool_join(p, NULL);

	pool_destroy(p);
	dict_list_free(dl);
}

//get, set, del in each
void dict_list_10_test()
{
	dict_list *dl;

	dl = dict_list_new();

	dict_list_push(dl, 1);

	dict_list_each(dl, lambda((dict *d, int num), {
		dict_list_get(dl, num);
	}));

	dict_list_push(dl, 2);

	dict_list_each(dl, lambda((dict *d, int num), {
		dict_list_set(dl, num, num);
	}));

	dict_list_push(dl, 3);

	dict_list_each(dl, lambda((dict *d, int num), {
		dict_list_del(dl, num);
	}));

	dict_list_free(dl);
}


//set, get, del
void dict_hash_1_test()
{
	dict_hash *dh;
	int i;

	dh = dict_hash_new();

	for(i = 1; i < 100; i++)
		test_assert(dict_hash_get(dh, i), NULL, 'p');

	for(i = 1; i < 100; i++)
	{
		dict_hash_set(dh, i, i);
		test_assert(dict_hash_size(dh), i);
	}

	for(i = 1; i < 100; i++)
		test_assert(dict_hash_get(dh, i), i);

	for(i = 1; i < 100; i++)
	{
		dict_hash_set(dh, i, i << 1);
		test_assert(dict_hash_size(dh), 99);
	}

	for(i = 1; i < 100; i++)
		test_assert(dict_hash_get(dh, i), i << 1);

	for(i = 1; i < 100; i++)
	{
		dict_hash_del(dh, i);
		test_assert(dict_hash_size(dh), 99 - i);
	}

	for(i = 1; i < 100; i++)
		test_assert(dict_hash_get(dh, i), NULL, 'p');

	dict_hash_free(dh);
}

//hash free
void dict_hash_2_test()
{
	dict_hash *dh;
	int i;

	dh = dict_hash_new();

	for(i = 0; i < 100; i++)
		dict_hash_set(dh, i, i);
	for(i = 100; i < 200; i++)
		dict_hash_set(dh, i, lib_malloc(i));
	for(i = 200; i < 300; i++)
		dict_hash_set(dh, i, dict_pack(i));
	for(i = 300; i < 400; i++)
		dict_hash_set(dh, i, pool_init(i));
	for(i = 400; i < 500; i++)
		dict_hash_set(dh, i, queue_init(i));

	dict_hash_free(dh);
}

//hash each
void dict_hash_3_test()
{
	dict_hash *dh;
	__int64 k[100];
	int i;

	dh = dict_hash_new();

	for(i = 0; i < 100; i++)
		dict_hash_set(dh, i, i << 1);

	dict_hash_each(dh, lambda((dict *key, dict *data), k[(__int64)dict_data(key)] = (__int64)dict_data(data)));

	for(i = 0; i < 100; i++)
		test_assert(k[i], i << 1);

	dict_hash_free(dh);
}

//hash rehash
void dict_hash_4_test()
{
	dict_hash *dh;
	__int64 k[100];
	int i;

	dh = dict_hash_new();

	for(i = 0; i < 100; i++)
		dict_hash_set(dh, i, i << 1);

	dict_hash_rehash(dh);

	dict_hash_each(dh, lambda((dict *key, dict *data), k[(__int64)dict_data(key)] = (__int64)dict_data(data)));

	for(i = 0; i < 100; i++)
		test_assert(k[i], i << 1);

	for(i = 1; i < 100; i++)
		test_assert(dict_hash_get(dh, i), i << 1);

	test_assert(dict_hash_size(dh), 100);

	dict_hash_free(dh);
}

//sync set
void dict_hash_5_test_1(dict_hash *dh)
{
	int i;
	for(i = 0; i < 1000; i++)
		dict_hash_set(dh, i, i);
}

void dict_hash_5_test()
{
	dict_hash *dh;
	pool *p;
	int num = 10;
	int i;

	dh = dict_hash_new();
	p = pool_init(num);

	for(i = 0; i < num; i++)
		pool_start(p, dict_hash_5_test_1, dh);
	pool_join(p, NULL);

	for(i = 0; i < 1000; i++)
		test_assert(dict_hash_get(dh, i), i);

	pool_destroy(p);
	dict_hash_free(dh);
}

//sync get
void dict_hash_6_test_1(dict_hash *dh)
{
	int i;
	for(i = 0; i < 1000; i++)
		test_assert(dict_hash_get(dh, i), i);
}

void dict_hash_6_test()
{
	dict_hash *dh;
	pool *p;
	int num = 10;
	int i;

	dh = dict_hash_new();
	p = pool_init(num);

	for(i = 0; i < 1000; i++)
		dict_hash_set(dh, i, i);

	for(i = 0; i < num; i++)
		pool_start(p, dict_hash_6_test_1, dh);
	pool_join(p, NULL);

	pool_destroy(p);
	dict_hash_free(dh);
}

//get, set, del in each
void dict_hash_7_test()
{
	dict_hash *dh;

	dh = dict_hash_new();

	dict_hash_set(dh, 1, 1);

	dict_hash_each(dh, lambda((dict *key, dict *data), {
		dict_hash_get(dh, key);
	}));

	dict_hash_set(dh, 2, 2);

	dict_hash_each(dh, lambda((dict *key, dict *data), {
		dict_hash_set(dh, key, key);
	}));

	dict_hash_set(dh, 3, 3);

	dict_hash_each(dh, lambda((dict *key, dict *data), {
		dict_hash_del(dh, key);
	}));

	dict_hash_free(dh);
}


//different key type
void dict_1_test()
{
	dict *d;
	d = dict_new();

	test_assert(dict_get(d, 1), NULL, 'p');
	test_assert(dict_get(d, 1, '2'), NULL, 'p');
	test_assert(dict_get(d, 1, '2', "3"), NULL, 'p');
	test_assert(dict_get(d, 1, '2', "3", 4.0), NULL, 'p');

	test_assert(dict_get(d, 1, '2', "3", 4.0, 5), NULL, 'p');
	dict_set(d, 1, '2', "3", 4.0, 5, 6);
	test_assert(dict_get(d, 1, '2', "3", 4.0, 5), 6);
	dict_del(d, 1, '2', "3", 4.0, 5);
	test_assert(dict_get(d, 1, '2', "3", 4.0, 5), NULL, 'p');

	test_assert(dict_get(d, 1, '2', "3", 4.0, "5"), NULL, 'p');
	dict_set(d, 1, '2', "3", 4.0, "5", "6");
	test_assert(dict_get(d, 1, '2', "3", 4.0, "5"), "6", 's');
	dict_del(d, 1, '2', "3", 4.0, "5");
	test_assert(dict_get(d, 1, '2', "3", 4.0, "5"), NULL, 'p');

	test_assert(dict_get(d, 1, '2', "3", 4.0, '5'), NULL, 'p');
	dict_set(d, 1, '2', "3", 4.0, '5', '6');
	test_assert(dict_get(d, 1, '2', "3", 4.0, '5'), '6', 'c');
	dict_del(d, 1, '2', "3", 4.0, '5');
	test_assert(dict_get(d, 1, '2', "3", 4.0, '5'), NULL, 'p');

	test_assert(dict_get(d, 1, '2', "3", 4.0, 5.0), NULL, 'p');
	dict_set(d, 1, '2', "3", 4.0, 5.0, 6.0);
	test_assert(dict_get(d, 1, '2', "3", 4.0, 5.0), 6.0, 'f');
	dict_del(d, 1, '2', "3", 4.0, 5.0);
	test_assert(dict_get(d, 1, '2', "3", 4.0, 5.0), NULL, 'p');

	test_assert(dict_get(d, 1, '2', "3", 4.0), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, 1, '2', "3"), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, 1, '2'), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, 1), NULL, 'p', TEST_UNEQUAL);

	dict_del(d, 1, '2', "3", 4.0);
	test_assert(dict_get(d, 1, '2', "3", 4.0), NULL, 'p');
	dict_del(d, 1, '2', "3");
	test_assert(dict_get(d, 1, '2', "3"), NULL, 'p');
	dict_del(d, 1, '2');
	test_assert(dict_get(d, 1, '2'), NULL, 'p');
	dict_del(d, 1);
	test_assert(dict_get(d, 1), NULL, 'p');

	dict_free(d);
}

//dict_del test
void dict_2_test()
{
	dict *d;
	d = dict_new();

	dict_set(d, 1, 2, 3, 4);
	test_assert(dict_get(d, 1, 2, 3), 4);

	dict_set(d, 1, 2, 4, 5);
	test_assert(dict_get(d, 1, 2, 4), 5);

	dict_set(d, 1, 2, 5, 6);
	test_assert(dict_get(d, 1, 2, 5), 6);

	dict_del(d, 1, 2, 3);
	test_assert(dict_get(d, 1, 2, 3), NULL, 'p');
	test_assert(dict_get(d, 1, 2, 4), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, 1, 2, 5), NULL, 'p', TEST_UNEQUAL);

	dict_del(d, 1, 2, 4);
	test_assert(dict_get(d, 1, 2, 4), NULL, 'p');
	test_assert(dict_get(d, 1, 2, 5), NULL, 'p', TEST_UNEQUAL);

	dict_del(d, 1, 2, 5);
	test_assert(dict_get(d, 1, 2, 5), NULL, 'p');

	dict_del(d, 1, 2);
	test_assert(dict_get(d, 1, 2), NULL, 'p');

	dict_del(d, 1);
	test_assert(dict_get(d, 1), NULL, 'p');

	dict_free(d);
}

//dict set repeated key
void dict_3_test()
{
	dict *d;
	d = dict_new();

	dict_set(d, 1, 2, 3, 4, 5);
	test_assert(dict_get(d, 1, 2, 3, 4), 5);

	dict_set(d, 1, 2, 3, 4, 5, 6);
	test_assert(dict_get(d, 1, 2, 3, 4, 5), 6);

	dict_set(d, 1, 2, 3, 4, 5, 7);
	test_assert(dict_get(d, 1, 2, 3, 4, 5), 7);

	dict_set(d, 1, 2, 3, 4, 5);
	test_assert(dict_get(d, 1, 2, 3, 4, 5), NULL, 'p');
	test_assert(dict_get(d, 1, 2, 3, 4), 5);

	dict_set(d, 1, 2, 3, 4);
	test_assert(dict_get(d, 1, 2, 3, 4), NULL, 'p');
	test_assert(dict_get(d, 1, 2, 3), 4);

	dict_set(d, 1, 2, 3);
	test_assert(dict_get(d, 1, 2, 3), NULL, 'p');
	test_assert(dict_get(d, 1, 2), 3);

	dict_set(d, 1, 2);
	test_assert(dict_get(d, 1, 2), NULL, 'p');
	test_assert(dict_get(d, 1), 2);

	dict_free(d);
}

//test add dict
void dict_4_test()
{
	dict *d;
	d = dict_new();

	dict *_d = dict_new();
	dict_set(_d, 2, 3);
	dict_set(_d, 3, 4);

	dict_set(d, 1, _d);

	test_assert(dict_get(d, 1, 2), 3);

	test_assert(dict_get(d, 1, 3), 4);

	dict_del(d, 1);
	test_assert(dict_get(d, 1), NULL, 'p');

	dict_free(d);
}

//auto free
void dict_5_test()
{
	dict *d;
	d = dict_new();

	dict_set(d, 1, lib_malloc(10));
	dict_set(d, 1, 2);
	dict_set(d, 1, lib_malloc(10));
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, lib_malloc(10));
	dict_set(d, 1, 2);

	dict_set(d, 1, pool_init(10));
	dict_set(d, 1, 2);
	dict_set(d, 1, pool_init(10));
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, pool_init(10));
	dict_set(d, 1, 2);

	dict_set(d, 1, queue_init(10));
	dict_set(d, 1, 2);
	dict_set(d, 1, queue_init(10));
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, queue_init(10));
	dict_set(d, 1, 2);

	dict_set(d, 1, dict_list_new());
	dict_set(d, 1, 2);
	dict_set(d, 1, dict_list_new());
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, dict_list_new());
	dict_set(d, 1, 2);

	dict_set(d, 1, dict_hash_new());
	dict_set(d, 1, 2);
	dict_set(d, 1, dict_hash_new());
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, dict_hash_new());
	dict_set(d, 1, 2);

	dict_set(d, 1, dict_new(DICT_LIST));
	dict_set(d, 1, 2);
	dict_set(d, 1, dict_new(DICT_LIST));
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, dict_new(DICT_LIST));
	dict_set(d, 1, 2);

	dict_set(d, 1, dict_new(DICT_HASH));
	dict_set(d, 1, 2);
	dict_set(d, 1, dict_new(DICT_HASH));
	dict_set(d, 1, 2, 3);
	dict_set(d, 1, 2, dict_new(DICT_HASH));
	dict_set(d, 1, 2);

	dict_free(d);
}

//test dict equal
void dict_6_test()
{
	dict *d;
	d = dict_new();

	dict_pack(d, 1);
	test_assert(dict_equal(d, 1), TRUE);
	test_assert(dict_equal(d, 2), FALSE);
	test_assert(dict_equal(d, '1'), FALSE);
	test_assert(dict_equal(d, '2'), FALSE);
	test_assert(dict_equal(d, "1"), FALSE);
	test_assert(dict_equal(d, "2"), FALSE);

	dict_pack(d, '1');
	test_assert(dict_equal(d, 1), FALSE);
	test_assert(dict_equal(d, 2), FALSE);
	test_assert(dict_equal(d, '1'), TRUE);
	test_assert(dict_equal(d, '2'), FALSE);
	test_assert(dict_equal(d, "1"), FALSE);
	test_assert(dict_equal(d, "2"), FALSE);

	dict_pack(d, "1");
	test_assert(dict_equal(d, 1), FALSE);
	test_assert(dict_equal(d, 2), FALSE);
	test_assert(dict_equal(d, '1'), FALSE);
	test_assert(dict_equal(d, '2'), FALSE);
	test_assert(dict_equal(d, "1"), TRUE);
	test_assert(dict_equal(d, "2"), FALSE);

	dict *d1, *d2;
	d1 = dict_new();
	d2 = dict_new();

	dict_pack(d1, 1);
	dict_pack(d2, 1);
	test_assert(dict_equal(d1, d2), TRUE);
	dict_pack(d2, '1');
	test_assert(dict_equal(d1, d2), FALSE);
	dict_pack(d2, "1");
	test_assert(dict_equal(d1, d2), FALSE);

	dict_pack(d1, '1');
	dict_pack(d2, 1);
	test_assert(dict_equal(d1, d2), FALSE);
	dict_pack(d2, '1');
	test_assert(dict_equal(d1, d2), TRUE);
	dict_pack(d2, "1");
	test_assert(dict_equal(d1, d2), FALSE);

	dict_pack(d1, "1");
	dict_pack(d2, 1);
	test_assert(dict_equal(d1, d2), FALSE);
	dict_pack(d2, '1');
	test_assert(dict_equal(d1, d2), FALSE);
	dict_pack(d2, "1");
	test_assert(dict_equal(d1, d2), TRUE);

	dict_free(d1);
	dict_free(d2);

	dict_free(d);
}

//dict list
void dict_7_test()
{
	dict *d;
	d = dict_new();

	dict_set(d, 1, dict_new(DICT_LIST));
	dict_set(d, 1, 0, 1);
	dict_set(d, 1, 1, 2);
	dict_set(d, 1, 2, 3);

	test_assert(dict_get(d, 1, -3), 1);
	test_assert(dict_get(d, 1, -2), 2);
	test_assert(dict_get(d, 1, -1), 3);
	test_assert(dict_get(d, 1, 0), 1);
	test_assert(dict_get(d, 1, 1), 2);
	test_assert(dict_get(d, 1, 2), 3);

	dict_set(d, 1, 1, 4);

	test_assert(dict_get(d, 1, -3), 1);
	test_assert(dict_get(d, 1, -2), 4);
	test_assert(dict_get(d, 1, -1), 3);
	test_assert(dict_get(d, 1, 0), 1);
	test_assert(dict_get(d, 1, 1), 4);
	test_assert(dict_get(d, 1, 2), 3);

	dict_set(d, 1, -1, 5);

	test_assert(dict_get(d, 1, -3), 1);
	test_assert(dict_get(d, 1, -2), 4);
	test_assert(dict_get(d, 1, -1), 5);
	test_assert(dict_get(d, 1, 0), 1);
	test_assert(dict_get(d, 1, 1), 4);
	test_assert(dict_get(d, 1, 2), 5);

	dict_free(d);
}

//dict json
void dict_8_test()
{
	dict *d;
	char *s;
	d = dict_new();

	dict_set(d, 1, 1);
	dict_set(d, 2, 2.0);
	dict_set(d, 3, '3');
	dict_set(d, 4, "4");
	dict_set(d, 5, NULL);
	dict_set(d, 6, dict_new(DICT_LIST));
	dict_set(d, 7, dict_new(DICT_HASH));

	dict_set(d, 6, 0, 1);
	dict_set(d, 6, 1, 2.0);
	dict_set(d, 6, 2, '3');
	dict_set(d, 6, 3, "4");
	dict_set(d, 6, 4, NULL);
	dict_set(d, 6, 5, dict_new(DICT_LIST));
	dict_set(d, 6, 6, dict_new(DICT_HASH));


	dict_set(d, 7, 1, 1);
	dict_set(d, 7, 2.0, 2);
	dict_set(d, 7, '3', 3);
	dict_set(d, 7, "4", 4);
	dict_set(d, 7, NULL, 5);

	dict_set(d, 7, 1, 1);
	dict_set(d, 7, 2, 2.0);
	dict_set(d, 7, 3, '3');
	dict_set(d, 7, 4, "4");
	dict_set(d, 7, 5, NULL);
	dict_set(d, 7, 6, dict_new(DICT_LIST));
	dict_set(d, 7, 7, dict_new(DICT_HASH));

	dict_set(d, 7, 8, dict_new(DICT_LIST));
	dict_set(d, 7, 8, 0, "\"");
	dict_set(d, 7, 8, 1, "\\");
	dict_set(d, 7, 8, 2, "\b");
	dict_set(d, 7, 8, 3, "\f");
	dict_set(d, 7, 8, 4, "\n");
	dict_set(d, 7, 8, 5, "\r");
	dict_set(d, 7, 8, 6, "\t");

	s = dict_json_dump(d);
	test_assert(dict_json_size(d), strlen(s));
	dict_free(d);


	d = dict_json_load(s);
	lib_free(s);

	test_assert(dict_get(d, "1"), 1);
	test_assert(dict_get(d, "2"), 2.0, 'f');
	test_assert(dict_get(d, "3"), '3', 'c');
	test_assert(dict_get(d, "4"), "4", 's');
	test_assert(dict_get(d, "5"), NULL, 'p');
	test_assert(dict_get(d, "6"), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, "7"), NULL, 'p', TEST_UNEQUAL);

	test_assert(dict_get(d, "6", 0), 1);
	test_assert(dict_get(d, "6", 1), 2.0, 'f');
	test_assert(dict_get(d, "6", 2), '3', 'c');
	test_assert(dict_get(d, "6", 3), "4", 's');
	test_assert(dict_get(d, "6", 4), NULL, 'p');
	test_assert(dict_get(d, "6", 5), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, "6", 6), NULL, 'p', TEST_UNEQUAL);

	test_assert(dict_get(d, "7", "1"), 1);
	test_assert(dict_get(d, "7", "2.0"), 2);
	test_assert(dict_get(d, "7", "51"), 3);

	test_assert(dict_get(d, "7", "1"), 1);
	test_assert(dict_get(d, "7", "2"), 2.0, 'f');
	test_assert(dict_get(d, "7", "3"), '3', 'c');
	test_assert(dict_get(d, "7", "4"), "4", 's');
	test_assert(dict_get(d, "7", "5"), NULL, 'p');
	test_assert(dict_get(d, "7", "6"), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, "7", "7"), NULL, 'p', TEST_UNEQUAL);

	test_assert(dict_get(d, "7", "8"), NULL, 'p', TEST_UNEQUAL);
	test_assert(dict_get(d, "7", "8", 0), "\"", 's');
	test_assert(dict_get(d, "7", "8", 1), "\\", 's');
	test_assert(dict_get(d, "7", "8", 2), "\b", 's');
	test_assert(dict_get(d, "7", "8", 3), "\f", 's');
	test_assert(dict_get(d, "7", "8", 4), "\n", 's');
	test_assert(dict_get(d, "7", "8", 5), "\r", 's');
	test_assert(dict_get(d, "7", "8", 6), "\t", 's');

	dict_free(d);
}

#endif
