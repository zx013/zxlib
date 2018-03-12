#ifndef TRANSQUEUE_H
#define TRANSQUEUE_H

#include "lib.h"
#include "transfer.h"
#include "unit.h"
#include "queue.h"
#include "pool.h"
#include "lock.h"
#include "error.h"

#define TRANSQUEUE_SIDE_SEND					TRANSFER_SIDE_SEND
#define TRANSQUEUE_SIDE_RECV					TRANSFER_SIDE_RECV

#define TRANSQUEUE_PORT_DEFAULT				TRANSFER_PORT_DEFAULT

typedef struct transqueue_attr
{
	int side;
	char *ip;
	int port;
} transqueue_attr;

typedef struct transqueue
{
	int side;
	pool *p;
	transfer *t;
	queue *fq; //快速接收队列
	queue *q; //普通队列
	int lock;
	unit u;
	int number;
	struct timeval tv;
} transqueue;


transqueue *transqueue_open(transqueue_attr *attr);

int transqueue_close(transqueue *tq);

static int transqueue_monitor(transqueue *tq);

static int transqueue_send(transqueue *tq);

static int transqueue_fast(transqueue *tq);

static int transqueue_recv(transqueue *tq);

int transqueue_push(transqueue *tq, package *p);

package *transqueue_pop(transqueue *tq);


#endif
