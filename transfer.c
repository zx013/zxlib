#include <stdlib.h>
#include <string.h>
#include "transfer.h"

//#include <time.h>
transfer *transfer_open(transfer_attr *attr)
{
	//srand(time(NULL));
	transfer *t;
	int size = 16 * 1024 * 1024;

	t = (transfer *)lib_calloc(sizeof(transfer));
	if(!t)
		error_goto(transfer_open_error_1);

#ifdef _WIN32
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		error_goto(transfer_open_error_2);
#endif

	t->soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(t->soc == INVALID_SOCKET)
		error_goto(transfer_open_error_3);

	t->addr.sin_family = AF_INET;
	t->addr.sin_port = htons(attr->port);

	int opt = 1;
	if(setsockopt(t->soc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR)
		error_goto(transfer_open_error_4);

	if(attr->side == SIDE_RECV)
	{
		if(setsockopt(t->soc, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(int)) == SOCKET_ERROR)
			error_goto(transfer_open_error_4);
		t->addr.sin_addr.s_addr = htonl(INADDR_ANY);
   		if(bind(t->soc, (struct sockaddr *)&t->addr, sizeof(struct sockaddr)))
   			error_goto(transfer_open_error_4);
   	}
   	else
   	{
   		if(setsockopt(t->soc, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(int)) == SOCKET_ERROR)
			error_goto(transfer_open_error_4);
   		t->addr.sin_addr.s_addr = inet_addr(attr->ip);
   	}
   	t->len = sizeof(t->addr);

	return t;

transfer_open_error_4:
#ifdef _WIN32
	closesocket(t->soc);
#else
	close(t->soc);
#endif
transfer_open_error_3:
#ifdef _WIN32
	WSACleanup();
#endif
transfer_open_error_2:
	lib_free(t);
transfer_open_error_1:
	return NULL;
}

int transfer_close(transfer *t)
{
	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 10;

	setsockopt(t->soc, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(struct linger));

#ifdef _WIN32
	shutdown(t->soc, SD_BOTH);
	closesocket(t->soc);
	WSACleanup();
#else
	shutdown(t->soc, SHUT_RDWR);
	close(t->soc);
#endif

	lib_free(t);
}

inline int transfer_send(transfer *t, void *buf, int len)
{
	//if(rand() % 100 < 30)
	//	return 0;
	return sendto(t->soc, buf, len, 0, (struct sockaddr *)&t->addr, t->len);
}

inline int transfer_recv(transfer *t, void *buf, int len)
{
	return recvfrom(t->soc, buf, len, 0, (struct sockaddr *)&t->addr, &t->len);
}


#ifdef TEST

#include "pool.h"
#include "test.h"

void transfer_test_1()
{
	transfer_attr attr =
	{
		.side = SIDE_SEND,
		.ip = "127.0.0.1",
		.port = TRANSFER_PORT_DEFAULT
	};

	transfer *t;
	t = transfer_open(&attr);
	test_assert(t, NULL, 'p', TEST_UNEQUAL);
	char buf[16] = "1234567890";
	int result;
	result = transfer_send(t, buf, 16);
	test_assert(result, 16, 'd', TEST_EQUAL);

	transfer_close(t);
}

void transfer_test_2()
{
	transfer_attr attr =
	{
		.side = SIDE_RECV,
		.port = TRANSFER_PORT_DEFAULT
	};

	transfer *t;
	t = transfer_open(&attr);
	test_assert(t, NULL, 'p', TEST_UNEQUAL);
	char buf[16];
	int result;
	result = transfer_recv(t, buf, 16);
	test_assert(result, 16, 'd', TEST_EQUAL);
	test_assert(buf, "1234567890", 's', TEST_EQUAL);

	transfer_close(t);
}

void transfer_test()
{
	pool *p;
	p = pool_init(2);
	pool_start(p, transfer_test_2, NULL);
	tools_sleep(100 MS);
	pool_start(p, transfer_test_1, NULL);
	pool_join(p, NULL);
	pool_destroy(p);
}

#endif
