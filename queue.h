#ifndef QUEUE_H
#define QUEUE_H

#include "head.h"
#include "lib.h"
#include "lock.h"
#include "list.h"
#include "error.h"


typedef struct queue
{
	int max;
	list_t *list;
	int lock;
} queue;


queue *queue_init(int max);

int queue_put(queue *q, void *data, __int64 tm);

int _queue_get(queue *q, void **data, __int64 tm);

#define queue_get(q, s, t) _queue_get(q, (void *)&s, t)

#define queue_each(q, block) list_each(q->list, block)

#define queue_head(q) list_head(q->list)

int queue_empty(queue *q);

inline int queue_size(queue *q);

void queue_destroy(queue *q);


#endif
