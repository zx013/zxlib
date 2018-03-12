#ifndef REMOTE_H
#define REMOTE_H

#include "lib.h"
#include "dict.h"
#include "analysis.h"

typedef struct remote
{
	char *name;
	void *func;
	void *data[PARAMETER_NUMBER];
	__int64 len[PARAMETER_NUMBER];
	int size;
} remote;

int remote_send(queue *q, remote *r);

int remote_recv(queue *q);

int remote_register(char *name, void *func);

void *_remote_args(int num, char *name);

#define remote_args(num) _remote_args(num, (char *)__FUNCTION__)


#endif
