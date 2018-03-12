#ifndef HASH_H
#define HASH_H

#include "head.h"
#include "lib.h"
#include "klib/khash.h"
#include "klib/klock.h"


KHASH_MAP_INIT_INT64(int64, void *)
KHASH_MAP_INIT_STR(str, void *)

typedef struct
{
	khash_t(int64) *i;
	khash_t(str) *s;
	klock_t lock;
} hash_t;

#define hash_new() \
	({ \
		hash_t *self; \
		self = lib_calloc(sizeof(hash_t)); \
		if(self) \
		{ \
			self->i = kh_init(int64); \
			self->s = kh_init(str); \
		} \
		self; \
	})

#define hash_clear(self) \
	({ \
		kh_clear(int64, (self)->i); \
		kh_clear(str, (self)->s); \
	})

#define hash_free(self) \
	({ \
		kh_destroy(int64, (self)->i); \
		kh_destroy(str, (self)->s); \
		lib_free(self); \
	})

#define hash_size(self) \
	({ \
		kh_size(self->i) + kh_size(self->s); \
	})

#define hash_each_int64(self, block) \
	({ \
		__int64 key; \
		void *val; \
		kh_foreach(self->i, key, val, block); \
	})

#define hash_each_str(self, block) \
	({ \
		const char *key; \
		void *val; \
		kh_foreach(self->s, key, val, block); \
	})

#define hash_each(self, block) \
	({ \
		void *val; \
		{ \
			const char *key; \
			kh_foreach(self->s, key, val, block); \
		} \
		{ \
			__int64 key; \
			kh_foreach(self->i, key, val, block); \
		} \
	})

#define _hash_set(type_t, self, key, val) \
	({ \
		int __ret; \
		khiter_t k = kh_put(type_t, self, (type_t)key, &__ret); \
		kh_value(self, k) = (void *)val; \
	})

#define hash_set(self, key, val) \
	({ \
		klock((self)->lock); \
		if(is_int64(key)) \
			_hash_set(int64, (self)->i, key, val); \
		else if(is_str(key)) \
			_hash_set(str, (self)->s, key, val); \
		kunlock((self)->lock); \
	})

#define _hash_get(type_t, self, key) \
	({\
		khiter_t k = kh_get(type_t, self, (type_t)key); \
		k == kh_end(self) ? NULL : kh_value(self, k); \
	})

#define hash_get(self, key) \
	({ \
		void *__ret = NULL; \
		if(is_int64(key)) \
			__ret = _hash_get(int64, (self)->i, key); \
		else if(is_str(key)) \
			__ret = _hash_get(str, (self)->s, key); \
		__ret; \
	})

#define _hash_del(type_t, self, key) \
	({ \
		khiter_t k = kh_get(type_t, self, (type_t)key); \
		kh_del(type_t, self, k); \
	})

#define hash_del(self, key) \
	({ \
		klock((self)->lock); \
		if(is_int64(key)) \
			_hash_del(int64, (self)->i, key); \
		else if(is_str(key)) \
			_hash_del(str, (self)->s, key); \
		kunlock((self)->lock); \
	})

#define hash_push(self, key) \
	({ \
		hash_set(self, key, 1); \
	})

#define hash_pop(self, __key) \
	({ \
		int __ret = FALSE; \
		if(is_int64(__key)) \
		{ \
			__key = 0; \
			if(kh_size(self->i) > 0) \
			{ \
				hash_each_int64(self, { \
					__key = (typeof(__key))key; \
					break; \
				}); \
				hash_del(self, __key); \
				__ret = TRUE; \
			} \
		} \
		else if(is_str(__key)) \
		{ \
			__key = NULL; \
			if(kh_size(self->s) > 0) \
			{ \
				hash_each_str(self, { \
					__key = (typeof(__key))key; \
					break; \
				}); \
				hash_del(self, __key); \
				__ret = TRUE; \
			} \
		} \
		__ret; \
	})

#endif
