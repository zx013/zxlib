#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "analysis.h"

int systemcall_send(queue *q, char *cmd, char *args[], int len);

int systemcall_recv(queue *q);


#endif
