#include "user.h"


static dict *d = NULL;

user *user_get(char *name)
{
	user *u;

	if(d == NULL)
		d = dict_new();

	u = dict_get(d, name);
	if(!u)
	{
		u = lib_calloc(sizeof(user));
		strcpy(u->name, name);
		u->q_send = queue_init(0);
		u->q_recv = queue_init(0);
		dict_set(d, u->name, u);
	}
	return u;
}
