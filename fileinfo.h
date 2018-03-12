#ifndef FILEINFO_H
#define FILEINFO_H

#include "lib.h"
#include "user.h"
#include "analysis.h"
#include "cache.h"
#include "dir.h"

typedef struct fileinfo
{
	char name_send[USERNAME_SIZE]; //Դ�û�
	char name_recv[USERNAME_SIZE]; //Ŀ���û�
	int number; //�ļ�����
	char path[PATH_SIZE];
	list_t *list;
} fileinfo;

int fileinfo_client_send(queue *q, fileinfo *f);

int fileinfo_recv(queue *q);

int fileinfo_client_recv(queue *q, fileinfo *f);


#endif
