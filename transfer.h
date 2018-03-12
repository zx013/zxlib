#ifndef TRANSFER_H
#define TRANSFER_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "head.h"
#include "lib.h"
#include "error.h"

#ifndef _WIN32
#define INVALID_SOCKET						-1
#define SOCKET_ERROR						-1
#endif

#define TRANSFER_PORT_DEFAULT				15400

#ifdef _WIN32
	typedef SOCKET	SOCKET;
#else
	typedef int	SOCKET;
#endif

typedef struct transfer_attr
{
	int side;
	char *ip;
	int port;
} transfer_attr;

typedef struct transfer
{
	SOCKET soc;
	struct sockaddr_in addr;
	int len;
} transfer;


transfer *transfer_open(transfer_attr *attr);

int transfer_close(transfer *t);

inline int transfer_send(transfer *t, void *buf, int len);

inline int transfer_recv(transfer *t, void *buf, int len);


#endif
