#ifndef BTREE_H
#define BTREE_H

#include "head.h"
#include "lib.h"
#include "klib/kbtree.h"
#include "klib/klock.h"


KBTREE_INIT(int64, int64, kb_generic_cmp)
KBTREE_INIT(str, str, kb_str_cmp)

typedef struct
{
	kbtree_t(int64) *i;
	kbtree_t(str) *s;
	klock_t lock;
} btree_t;

#define btree_new() \
	({ \
		btree_t *self; \
		self = lib_calloc(sizeof(btree_t)); \
		if(self) \
		{ \
			self->i = kb_init(int64, KB_DEFAULT_SIZE); \
			self->s = kb_init(str, KB_DEFAULT_SIZE); \
		} \
		self; \
	})

#define btree_free(self) \
	({ \
		kb_destroy(int64, self->i); \
		kb_destroy(str, self->s); \
		lib_free(self); \
	})

#define btree_size(self) \
	({ \
		kb_size(self->i) + kb_size(self->s); \
	})

#define btree_set(self, val) \
	({ \
		klock((self)->lock); \
		if(is_int64(val)) \
			kb_put(int64, self->i, (int64)val); \
		else if(is_str(val)) \
			kb_put(str, self->s, (str)val); \
		kunlock((self)->lock); \
	})

#define btree_get(self, val) \
	({ \
		void *__ret; \
		if(is_int64(val)) \
			__ret = kb_get(int64, self->i, (int64)val); \
		else if(is_str(val)) \
			__ret = kb_get(str, self->s, (str)val); \
		__ret; \
	})

#define btree_del(self, val) \
	({ \
		if(is_int64(val)) \
			kb_del(int64, self->i, (int64)val); \
		else if(is_str(val)) \
			kb_del(str, self->s, (str)val); \
	})

#endif
