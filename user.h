#ifndef USER_H
#define USER_H

#include "head.h"
#include "lib.h"
#include "dict.h"
#include "queue.h"

typedef struct user
{
	char name[USERNAME_SIZE];
	queue *q_send;
	queue *q_recv;
} user;

user *user_get(char *name);


#endif
