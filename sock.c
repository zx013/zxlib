#include "sock.h"


#ifdef _WIN32
	static int wsa_num = 0; //保证WSAStartup和WSACleanup只调用一次
#endif

SOCKET sock_create(int type)
{
	SOCKET soc;

#ifdef _WIN32
	WSADATA wsa;
	if(wsa_num == 0)
	{
		if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			error_goto(sock_create_error_1);
	}
	__sync_fetch_and_add(&wsa_num, 1);
#endif

	if(type == SOCK_TYPE_TCP)
		soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(soc == INVALID_SOCKET)
		error_goto(sock_create_error_2);

	int opt = 1;
	if(setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR)
		error_goto(sock_create_error_3);

	int size = 16 MB;
	if(setsockopt(soc, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size)) == SOCKET_ERROR)
		error_goto(sock_create_error_3);

	if(setsockopt(soc, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size)) == SOCKET_ERROR)
		error_goto(sock_create_error_3);

	return soc;

sock_create_error_3:
#ifdef _WIN32
	closesocket(soc);
#else
	close(soc);
#endif
sock_create_error_2:
#ifdef _WIN32
	__sync_fetch_and_sub(&wsa_num, 1);
	if(wsa_num == 0)
		WSACleanup();
#endif
sock_create_error_1:
	return INVALID_SOCKET;
}

int sock_destroy(SOCKET soc)
{
	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 10;

	setsockopt(soc, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(struct linger));

#ifdef _WIN32
	shutdown(soc, SD_BOTH);
	closesocket(soc);
	__sync_fetch_and_sub(&wsa_num, 1);
	if(wsa_num == 0)
		WSACleanup();
#else
	shutdown(soc, SHUT_RDWR);
	close(soc);
#endif
}

sock *sock_open(sock_attr *attr)
{
	sock *s;

	s = (sock *)lib_calloc(sizeof(sock));
	if(!s)
		error_goto(sock_open_error_1);

	s->soc = sock_create(attr->type);
	if(s->soc == INVALID_SOCKET)
		error_goto(sock_open_error_2);

	s->addr.sin_family = AF_INET;
	s->addr.sin_port = htons(attr->port);

	s->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(s->soc, (struct sockaddr *)&s->addr, sizeof(struct sockaddr)))
		error_goto(sock_open_error_3);

	if(attr->type == SOCK_TYPE_TCP)
	{
		if(listen(s->soc, 1024) == SOCKET_ERROR)
			error_goto(sock_open_error_3);
	}

	s->len = sizeof(s->addr);

	return s;

sock_open_error_3:
	sock_destroy(s->soc);
sock_open_error_2:
	lib_free(s);
sock_open_error_1:
	return NULL;
}

int sock_close(sock *s)
{
	sock_destroy(s->soc);
	lib_free(s);
}

/*
服务器检查用户登陆状态

服务器发送256位随机字符串

客户端将密码，KEY分别与该随机字符串组合计算md5
客户端将用户名密码，KEY发送到服务器

服务器查找用户名对应的密码和KEY，使用该随机字符串组合计算md5
服务器比较密码和KEY，并移除该随机字符串

服务器发送登陆成功状态
*/
int sock_client_login(sock_client *sc)
{
	sock_client_send(sc, "user01", 1024);
}

int sock_server_login(sock_client *ss)
{
	char buf[1024];
	sock_server_recv(ss, buf, 1024);
}


sock_client *sock_client_open(sock_attr *attr)
{
	sock_client *sc;

	sc = calloc(1, sizeof(sock_client));
	if(!sc)
		error_goto(sock_client_open_error_1);

	sc->curl = curl_easy_init();
	if(!sc->curl)
		error_goto(sock_client_open_error_2);

	curl_socket_t opensocket(void *clientp, curlsocktype purpose, struct curl_sockaddr *address)
	{
		return sock_create(attr->type);
	}

	curl_easy_setopt(sc->curl, CURLOPT_OPENSOCKETFUNCTION, opensocket);
	curl_easy_setopt(sc->curl, CURLOPT_URL, attr->ip);
	curl_easy_setopt(sc->curl, CURLOPT_PORT, attr->port);
	curl_easy_setopt(sc->curl, CURLOPT_CONNECT_ONLY, 1L);

	if(curl_easy_perform(sc->curl) != CURLE_OK)
		error_goto(sock_client_open_error_3);

	return sc;

sock_client_open_error_3:
	curl_easy_cleanup(sc->curl);
sock_client_open_error_2:
	free(sc);
sock_client_open_error_1:
	return NULL;
}

int sock_client_close(sock_client *sc)
{
	curl_easy_cleanup(sc->curl);
	free(sc);
}

inline int sock_client_send(sock_client *sc, char *data, int len)
{
	size_t iolen;
	if(curl_easy_send(sc->curl, data, len, &iolen) == CURLE_OK)
		return TRUE;

	return FALSE;
}

inline int sock_client_recv(sock_client *sc, char *data, int len)
{
	size_t iolen;
	if(curl_easy_recv(sc->curl, data, len, &iolen) == CURLE_OK)
		return TRUE;

	return FALSE;
}


//libevent函数之间传递数据
typedef struct sock_argv
{
	struct event *ev;
	void *func;
} sock_argv;

static struct event_base *eb = NULL;
static int lock = 0;

//一个服务端可能有多个连接，所以需要加回调来区分
void sock_on_read(evutil_socket_t soc, short event, void *argv)
{
	sock_server *ss = argv;
	struct event *ev = ss->ev;
	int (*func)(sock_server *) = ss->func;

	if(func)
	{
		if(func(ss) == FALSE)
			debug_goto(sock_on_read_error_1);
	}
	else
	{
		char buf[1024];
		if(sock_server_recv(ss, buf, 1024) == FALSE)
			debug_goto(sock_on_read_error_1);
	}
	return;

sock_on_read_error_1:
	sock_server_close(ss);
	return;
}

void sock_on_accept(evutil_socket_t soc, short event, void *argv)
{
	sock_server *ss;

	ss = calloc(1, sizeof(struct sock_server));
	if(!ss)
		error_goto(sock_on_accept_error_1);

	ss->ev = calloc(1, sizeof(struct event));
	if(!ss->ev)
		error_goto(sock_on_accept_error_2);

	ss->func = ((sock_server *)argv)->func;

	ss->s = (sock *)lib_calloc(sizeof(sock));
	if(!ss->s)
		error_goto(sock_on_accept_error_3);

	ss->s->soc = accept(soc, NULL, NULL);
	if(ss->s->soc == INVALID_SOCKET)
		error_goto(sock_on_accept_error_4);

	event_assign(ss->ev, eb, ss->s->soc, EV_READ | EV_PERSIST | EV_ET, sock_on_read, ss);
	event_add(ss->ev, NULL);
	return;

sock_on_accept_error_4:
	lib_free(ss->s);
sock_on_accept_error_3:
	free(ss->ev);
sock_on_accept_error_2:
	free(ss);
sock_on_accept_error_1:
	return;
}

/*
mingw64环境下编译
如果出现/usr/bin/sh: line 1: C:/Program: No such file or directory这个错误
将Makefile中的LIBTOOL = $(SHELL) $(top_builddir)/libtool
改为LIBTOOL = $(top_builddir)/libtool
*/

int _sock_server_init()
{
	while(1)
	{
		event_base_dispatch(eb);
		lock_yield();
	}
	event_base_free(eb);
}

int sock_server_init()
{
	thread t;
	if(!eb)
	{
		eb = event_base_new();
		thread_run(&t, _sock_server_init, NULL);
	}
}

sock_server *sock_server_open(sock_attr *attr)
{
	sock_server_init();

	sock_server *ss;

	ss = calloc(1, sizeof(sock_server));
	if(!ss)
		error_goto(sock_server_open_error_1);

	ss->ev = calloc(1, sizeof(struct event));
	if(!ss->ev)
		error_goto(sock_server_open_error_2);

	ss->s = sock_open(attr);
	if(!ss->s)
		error_goto(sock_server_open_error_3);

	ss->func = attr->func;

	switch(attr->type)
	{
	case SOCK_TYPE_TCP:
		event_assign(ss->ev, eb, ss->s->soc, EV_READ | EV_PERSIST | EV_ET, sock_on_accept, ss);
		break;
	case SOCK_TYPE_UDP:
		event_assign(ss->ev, eb, ss->s->soc, EV_READ | EV_PERSIST | EV_ET, sock_on_read, ss);
		break;
	}
	event_add(ss->ev, NULL);

	return ss;

sock_server_open_error_4:
	sock_close(ss->s);
sock_server_open_error_3:
	free(ss->ev);
sock_server_open_error_2:
	free(ss);
sock_server_open_error_1:
	return NULL;
}

int sock_server_close(sock_server *ss)
{
	event_del(ss->ev);
	free(ss->ev);
	sock_close(ss->s);
	free(ss);
}

inline int sock_server_send(sock_server *ss, char *data, int len)
{
	if(sendto(ss->s->soc, data, len, SEND_SIG, 0, 0) > 0)
		return TRUE;

	return FALSE;
}

inline int sock_server_recv(sock_server *ss, char *data, int len)
{
	if(recvfrom(ss->s->soc, data, len, 0, 0, 0) > 0)
		return TRUE;

	return FALSE;
}


#ifdef TEST

#include "test.h"
#include "pool.h"

void sock_test_client()
{
	int i = 0;

	for(i = 0; i < 3; i++)
	{
		sock_attr attr =
		{
			.type = SOCK_TYPE_TCP,
			.ip = "127.0.0.1",
			.port = 12345
		};
		sock_client *sc = sock_client_open(&attr);

		if(sock_client_send(sc, "abcde", 5) != TRUE)
			return;

		tools_sleep(200 MS);

		if(sock_client_send(sc, "fghij", 5) != TRUE)
			return;

		sock_client_close(sc);
	}
}

int sock_func(sock_server *ss)
{
	char buf[32] = {0};
	if(sock_server_recv(ss, buf, 5) == FALSE)
		return FALSE;
	printf("%s\n", buf);

	return TRUE;
}

void sock_test_server()
{
	sock_attr attr =
	{
		.type = SOCK_TYPE_TCP,
		.port = 12345,
		.func = sock_func
	};

	sock_server *ss = sock_server_open(&attr);
	tools_sleep(2 S);
	sock_server_close(ss);
}

void sock_test_curl()
{
	pool *p;
	p = pool_init(2);
	pool_start(p, sock_test_server, NULL);
	tools_sleep(300 MS);
	pool_start(p, sock_test_client, NULL);
	pool_join(p, NULL);
	pool_destroy(p);
}

void sock_test()
{
	sock_test_curl();
}

#endif
