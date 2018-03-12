#ifndef LIST_H
#define LIST_H

#include "klib/klist.h"

#define __void_free(x)
KLIST_INIT(ptr, void *, __void_free)

typedef klist_t(ptr) list_t;

#define list_new() kl_init(ptr)

#define list_free(self) kl_destroy(ptr, self)

#define list_push(self, val) *kl_pushp(ptr, self) = val

#define list_pop(self) \
	({ \
		void *val; \
		kl_shift(ptr, self, &val) == 0 ? val : NULL; \
	})

#define list_put(self, val) \
	({ \
		list_push(self, (void *)val); \
		TRUE; \
	})

/*
if not use __val, when val is not point, after call kl_shift, self became null
*/
#define list_get(self, val) \
	({ \
		typeof(val) __val; \
		kl_shift(ptr, self, (void **)&__val) == 0 ? ({val = __val; TRUE;}) : FALSE; \
	})

#define list_size(self) self->size

#define list_each(self, block) \
	({ \
		kliter_t(ptr) *k; \
		void *val; \
		for(k = kl_begin(self); k != kl_end(self); k = kl_next(k)) \
		{ \
			val = kl_val(k); \
			block; \
		} \
	})

#define list_head(self) kl_val(kl_begin(self))


#endif
