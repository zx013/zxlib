#ifndef DICT_H
#define DICT_H

#include <float.h>
#include <math.h>
#include "head.h"
#include "lib.h"
#include "lock.h"
#include "code.h"
#include "error.h"


/*
读速度每秒3000万次，写速度每秒300万次（应用快速内存分配模块后性能会有所提高）
*/
#define DICT_NONE									0
#define DICT_INT64									1
#define DICT_DOUBLE									2
#define DICT_STR									3
#define DICT_OTHER									4
#define DICT_HASH									5
#define DICT_LIST									6
#define DICT_DICT									7
#define DICT_DATA DICT_INT64: case DICT_DOUBLE: case DICT_STR: case DICT_OTHER


typedef struct dict
{
	char type; //类型，前四位为次类型(char, int, __int64)，后四位为主类型(int64, double, str)
	void *data;
} __attribute__((packed)) dict;


#define dict_value(__data) \
	({ \
		is_dict(__data) ? ((dict *)(cast(__data)))->data : cast(__data); \
	})

#define is_dict_hash(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), dict_hash *) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), dict_hash []) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), const dict_hash *) ? 3 : \
		__builtin_types_compatible_p(typeof(__data), const dict_hash []) ? 4 : \
		0) << IS_OFFSET; \
	})

#define is_dict_list(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), dict_list *) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), dict_list []) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), const dict_list *) ? 3 : \
		__builtin_types_compatible_p(typeof(__data), const dict_list []) ? 4 : \
		0) << IS_OFFSET; \
	})

#define is_dict(__data) \
	({ \
		(__builtin_types_compatible_p(typeof(__data), dict *) ? 1 : \
		__builtin_types_compatible_p(typeof(__data), dict []) ? 2 : \
		__builtin_types_compatible_p(typeof(__data), const dict *) ? 3 : \
		__builtin_types_compatible_p(typeof(__data), const dict []) ? 4 : \
		0) << IS_OFFSET; \
	})

//if else也会优化，不过似乎是间址方式赋值，注意部分DICT_DICT类型需要特殊处理
#define dict_type(__data) \
	({ \
		is_int64(__data) ? DICT_INT64 + is_int64(__data) : \
		is_double(__data) ? DICT_DOUBLE + is_double(__data) : \
		is_str(__data) ? DICT_STR + is_str(__data) : \
		is_dict_hash(__data) ? DICT_HASH + is_dict_hash(__data) : \
		is_dict_list(__data) ? DICT_LIST + is_dict_list(__data) : \
		is_dict(__data) ? DICT_DICT + is_dict(__data) : \
		DICT_OTHER; \
	})

//获取dict中的数据
#define dict_data(...) __VA_FUNC__(dict_data, __VA_ARGS__)
#define dict_data_1(__data) dict_data_2(__data, NULL)
#define dict_data_2(__data, __error) \
	({ \
		__data ? ((dict *)(__data))->data : __error; \
	})

//创建新的dict
#define dict_new(...) __VA_FUNC__(dict_new, __VA_ARGS__)
dict *dict_new_0();
dict *dict_new_1(int type);

//释放dict
int dict_free(dict *d);

//比较数据是否和dict中的数据相等
#define dict_equal(__d, __data) dict_equal_imple(__d, dict_type(__data), cast(__data))
int dict_equal_imple(dict *d, int datatype, void *datavalue);

//将数据封装成dict格式，若提供dict则添加到提供的dict中
#define dict_pack(...) __VA_FUNC__(dict_pack, __VA_ARGS__)
dict *dict_pack_1_imple(int datattype, void *datavalue);
#define dict_pack_1(__data) dict_pack_1_imple(dict_type(__data), cast(__data))
dict *dict_pack_2_imple(dict *d, int datattype, void *datavalue);
#define dict_pack_2(__d, __data) dict_pack_2_imple(__d, dict_type(__data), cast(__data))



#define dict_set_0(__d, x, ...) __VA_DEFER__(dict_set_I)()(__d, __VA_NARG__(__VA_ARGS__), __VA_ARGS__)
#define dict_set_1(__d, ...) __d
#define dict_set_I() _dict_set

#define __dict_set(__d, __n, ...) \
	({ \
		dict *__d1##__n = __d; \
		dict *__d2##__n = NULL; \
		dict_hash *__dh = NULL; \
		if(__VA_NARG_1__(__VA_ARGS__)) \
			dict_pack(__d1##__n, __VA_GETFIR__(__VA_ARGS__)); \
		else \
		{ \
			switch(major_type(__d1##__n->type)) \
			{ \
			case DICT_NONE: case DICT_DATA: \
				__dh = dict_hash_new(); \
				__d2##__n = dict_new(); \
				dict_hash_set(__dh, __VA_GETFIR__(__VA_ARGS__), __d2##__n); \
				dict_pack(__d1##__n, __dh); \
				break; \
			case DICT_LIST: \
				__d2##__n = dict_list_get_dict(__d1##__n->data, (int)(__int64)(__VA_GETFIR__(__VA_ARGS__))); \
				if(!__d2##__n) \
				{ \
					__d2##__n = dict_pack(__VA_GETFIR__(__VA_ARGS__)); \
					dict_list_push(__d1##__n->data, __d2##__n); \
				} \
				break; \
			case DICT_HASH: \
				__d2##__n = dict_hash_get_dict(__d1##__n->data, dict_type(__VA_GETFIR__(__VA_ARGS__)), cast(__VA_GETFIR__(__VA_ARGS__))); \
				if(!__d2##__n) \
				{ \
					__d2##__n = dict_new(); \
					dict_hash_set(__d1##__n->data, __VA_GETFIR__(__VA_ARGS__), __d2##__n); \
				} \
				break; \
			} \
		} \
		MACROCAT(dict_set_, __VA_NARG_1__(__VA_ARGS__))(__d2##__n, __VA_ARGS__); \
	})

#define _dict_set(__d, __n, ...) \
	({ \
		__d ? __dict_set(__d, __n, __VA_ARGS__) : NULL; \
	})

#define dict_set(__d, ...) __VA_EVAL__((dict *)_dict_set(((dict *)(__d)), __VA_NARG__(__VA_ARGS__), __VA_ARGS__))


#define dict_get_0(__d, x, ...) __VA_DEFER__(dict_get_I)()(__d, __VA_NARG__(__VA_ARGS__), __VA_ARGS__)
#define dict_get_1(__d, ...) __d
#define dict_get_I() _dict_get

//不用__dict则__d值会被改变
#define __dict_get(__d, __n, ...) \
	({ \
		dict *__d1##__n = __d; \
		dict *__d2##__n = NULL; \
		void *__ret = NULL; \
		if(__VA_NARG_1__(__VA_ARGS__)) \
			__ret = __d1##__n; \
		else \
		{ \
			switch(major_type(__d1##__n->type)) \
			{ \
			case DICT_LIST: \
				__d2##__n = dict_list_get_dict(__d1##__n->data, (int)(__int64)(__VA_GETFIR__(__VA_ARGS__))); \
				break; \
			case DICT_HASH: \
				__d2##__n = dict_hash_get_dict(__d1##__n->data, dict_type(__VA_GETFIR__(__VA_ARGS__)), cast(__VA_GETFIR__(__VA_ARGS__))); \
				break; \
			} \
			__ret = MACROCAT(dict_get_, __VA_NARG_1__(__VA_ARGS__))(__d2##__n, __VA_ARGS__); \
		} \
		__ret; \
	})

#define _dict_get(__d, __n, ...) \
	({ \
		__d ? __dict_get(__d, __n, __VA_ARGS__) : NULL; \
	})


//获取节点的dict
#define dict_get_node(__d, ...) __VA_EVAL__((dict *)_dict_get(((dict *)(__d)), __VA_NARG__(__VA_ARGS__), __VA_ARGS__, 0))

#define dict_get(__d, ...) dict_data(dict_get_node(__d, __VA_ARGS__))


#define dict_del(__d, ...) \
	({ \
		dict *__dp; \
		if(__VA_NARG_1__(__VA_ARGS__)) \
			__dp = __d; \
		else \
			__dp = dict_get_node(__d, __VA_EXCLUDE__(__VA_ARGS__)); \
		dict *__dict = dict_get_node(__dp, __VA_GETLAST__(__VA_ARGS__)); \
		dict_hash_del(__dp->data, __VA_GETLAST__(__VA_ARGS__)); \
	})


int dict_each(dict *d, void *func);



#define DICT_LIST_START						0
#define DICT_LIST_END						INT_MAX

//快速查找，记录上一次查找的节点，多线程下性能没有很好提高，单线程下有较好的性能
//__sync_lock_test_and_set只支持最多64位，因此需要加锁
typedef struct dict_list_fast
{
	struct dict_list_node *node;
	int num;
	char lock;
} dict_list_fast;

typedef struct dict_list
{
	struct dict_list_node *head;
	struct dict_list_node *tail;
	int size;
	__int64 lock;

	dict_list_fast fast;
} dict_list;

typedef struct dict_list_node
{
	struct dict_list_node *next;
	struct dict_list_node *prev;
	dict *data;
} dict_list_node;


//dict_list_new
dict_list *dict_list_new();

//dict_list_free
int dict_list_free(dict_list *dl);

//dict_list_size
#define dict_list_size(__dl) ((dict_list *)__dl)->size

//dict_list_push
#define dict_list_push(...) __VA_FUNC__(dict_list_push, __VA_ARGS__)
int dict_list_push_3_imple(dict_list *dl, int datatype, void *datavalue, int num);
#define dict_list_push_2(__dl, __data) dict_list_push_3(__dl, __data, ((dict_list *)__dl)->size)
#define dict_list_push_3(__dl, __data, __num) dict_list_push_3_imple(__dl, dict_type(__data), cast(__data), __num)

//dict_list_pop
#define dict_list_pop(...) __VA_FUNC__(dict_list_pop, __VA_ARGS__)
void *dict_list_pop_3_imple(dict_list *dl, int num, void *error);
#define dict_list_pop_1(__dl) dict_list_pop_2(__dl, 0)
#define dict_list_pop_2(__dl, __num) dict_list_pop_3(__dl, __num, NULL)
#define dict_list_pop_3(__dl, __num, __error) dict_list_pop_3_imple(__dl, __num, cast(__error))

//dict_list_get_node
static dict_list_node *dict_list_get_node(dict_list *dl, int num);

//dict_list_get_dict
dict *dict_list_get_dict(dict_list *dl, int num);

//dict_list_set
int dict_list_set_imple(dict_list *dl, int num, int datatype, void *datavalue);
#define dict_list_set(__dl, __num, __data) dict_list_set_imple(__dl, __num, dict_type(__data), cast(__data))

//dict_list_get
#define dict_list_get(...) __VA_FUNC__(dict_list_get, __VA_ARGS__)
dict *dict_list_get_3_imple(dict_list *dl, int num, void *error);
#define dict_list_get_2(__dl, __num) dict_list_get_3_imple(__dl, __num, NULL)
#define dict_list_get_3(__dl, __num, __error) dict_list_get_3_imple(__dl, __num, cast(__error))

//dict_list_del
int dict_list_del(dict_list *dl, int num);

//dict_list_each
#define dict_list_each(...) __VA_FUNC__(dict_list_each, __VA_ARGS__)
int dict_list_each_2(dict_list *dl, void *func);
int dict_list_each_4(dict_list *dl, int start, int end, void *func);
int dict_list_each_5(dict_list *dl, int start, int end, int step, void *func);

//dict_list_slice
#define dict_list_slice(...) __VA_FUNC__(dict_list_slice, __VA_ARGS__)
dict_list *dict_list_slice_3(dict_list *dl, int start, int end);
dict_list *dict_list_slice_4(dict_list *dl, int start, int end, int step);

//dict_list_attach
int dict_list_attach(dict_list *dl1, dict_list *dl2);



typedef struct dict_hash
{
	struct dict_list **list;
	int size;
	int max;
	__int64 lock;
} dict_hash;

typedef struct dict_hash_node
{
	dict *key;
	dict *data;
} dict_hash_node;

//dict_hash_new
#define dict_hash_new(...) __VA_FUNC__(dict_hash_new, __VA_ARGS__)
dict_hash *dict_hash_new_1_imple(int max);
#define dict_hash_new_0() dict_hash_new_1_imple(37)
#define dict_hash_new_1(__max) dict_hash_new_1_imple(__max)

//dict_hash_free
int dict_hash_free(dict_hash *dh);

//dict_hash_size
#define dict_hash_size(__dh) ((dict_hash *)__dh)->size

//dict_hash_map
unsigned __int64 dict_hash_map_2_imple(int datatype, void *datavalue);
#define dict_hash_map(__data) dict_hash_map_2_imple(dict_type(__data), cast(__data))

//dict_hash_rehash
static int dict_hash_rehash(dict_hash *dh);

//dict_hash_get_node
static dict_hash_node *dict_hash_get_node(dict_hash *dh, int keytype, void *keyvalue);

//dict_hash_get_dict
dict *dict_hash_get_dict(dict_hash *dh, int keytype, void *keyvalue);

//dict_hash_set_node
static void *dict_hash_set_node(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue);

//dict_hash_set_dict
dict *dict_hash_set_dict(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue);

//dict_hash_set
void *dict_hash_set_imple(dict_hash *dh, int keytype, void *keyvalue, int datatype, void *datavalue);
#define dict_hash_set(__dh, __key, __data) dict_hash_set_imple(__dh, dict_type(__key), cast(__key), dict_type(__data), cast(__data))

//dict_hash_get
#define dict_hash_get(...) __VA_FUNC__(dict_hash_get, __VA_ARGS__)
void *dict_hash_get_3_imple(dict_hash *dh, int keytype, void *keyvalue, void *error);
#define dict_hash_get_2(__dh, __key) dict_hash_get_3(__dh, __key, NULL)
#define dict_hash_get_3(__dh, __key, __error) dict_hash_get_3_imple(__dh, dict_type(__key), cast(__key), cast(__error))

//dict_hash_del
int dict_hash_del_imple(dict_hash *dh, int keytype, void *keyvalue);
#define dict_hash_del(__dh, __key) dict_hash_del_imple(__dh, dict_type(__key), cast(__key))

//dict_hash_each
int dict_hash_each(dict_hash *dh, void *func);



#define JSON_SKIP_SPACE				1
#define JSON_SKIP_DATA				2
#define JSON_SKIP_LIST				3
#define JSON_SKIP_HASH				4

static unsigned dict_json_mark[] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

//dict_json_skip
static char *dict_json_skip(char *str, int type);

//dict_json_hex4
static unsigned dict_json_hex4(const char *str);

//dict_json_size
__int64 dict_json_size(dict *d);

//dict_json_dump
static char *__dict_json_dump(dict *d, char *addr);
char *dict_json_dump(dict *d);

//dict_json_load
static dict *__dict_json_load(char **addr);
dict *dict_json_load(char *s);


#endif
