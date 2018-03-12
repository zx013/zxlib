#ifndef SOCK_H
#define SOCK_H

/*
实现高可用性和负载均衡

设置传输服务器，可多台
设置传输服务器对应的备份服务器，每台传输服务器可设置一台备份服务器或不设置备份服务器
设置了备份服务器的传输服务器视为安全服务器，如果数据设置了安全属性，则优先通过安全服务器传输（可设置为只能通过安全服务器传输）

发送处理
流量分发
故障切换


接收处理
*/

/*
//发送数据包
sock_send

//接收数据包
sock_recv
*/

/*
一发多收的实现
发送端将数据包以负载均衡的方式发送到接收端
接收端收到数据后，将数据转发到其它的接收端

接收端上线和下线的处理
上线时将接收端加入负载均衡队列，下线时将接收端从负载均衡队列移出


发送端和接收端建立发送连接和接收连接，发送连接只负责发送数据包，接收连接用来反馈接收状态


发送端逻辑
发送端轮流将数据包发送到各个接收端，并将数据包缓存
发送端从接收端获取返回信息，标记已接收数据包，若该数据包已被所有在线客户端接收，则清除该缓存
当有接收端下线，如果有客户端未接收该包，将该数据包重新发送到一个未接收的客户端

接收端逻辑
接收端登陆，获取已登陆的接收端列表，建立和已登陆接收端的连接（如果有接收端登出，可能会连接失败）
接收端收到数据包后，将数据包转发至其它接收端

接收端定时发送心跳到发送端，反馈已接收的数据包


*/

#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <curl.h> //必须放在head.h前
#include <event.h>

#include "head.h"
#include "lib.h"
#include "lock.h"
#include "queue.h"
#include "error.h"

#ifndef _WIN32
#define INVALID_SOCKET								-1
#define SOCKET_ERROR								-1
#endif

#ifndef _WIN32
	typedef int SOCKET;
#endif

#ifdef _WIN32
#define SEND_SIG									0
#else
#define SEND_SIG									MSG_NOSIGNAL
#endif

#define SOCK_TYPE_TCP								1
#define SOCK_TYPE_UDP								2

typedef struct sock_attr
{
	int type;
	char *ip;
	int port;
	void *func;
} sock_attr;

typedef struct sock
{
	SOCKET soc;
	struct sockaddr_in addr;
	int len;
} sock;

typedef struct sock_client
{
	CURL *curl;
	size_t iolen;
} sock_client;

typedef struct sock_server
{
	sock *s;
	struct event *ev;
	void *func;
} sock_server;

sock *sock_open(sock_attr *attr);

int sock_close(sock *s);




/*
typedef struct sock_server_attr
{
} sock_server_attr;

typedef struct sock_server
{
} sock_server;

typedef struct sock_server_pool
{
} sock_server_pool;

sock_server *sock_server_open(sock_server_attr *attr);

//初始化服务器池
sock_server_pool *sock_server_pool_init();

//销毁服务器池
int sock_server_pool_destroy(sock_server_pool *ssp);

//将服务器添加到服务器池中
int sock_server_pool_insert(sock_server_pool *ssp, sock_server *ss);

//合并两个服务器池
int sock_server_pool_merge(sock_server_pool *ssp1, sock_server_pool *ssp2);

//同步服务器池
int sock_server_pool_sync(sock_server_pool *ssp);

//将服务器设置为传输服务器
int sock_server_pool_channel(sock_server_pool *ssp, sock_server *ss);

//将服务器设置为备份服务器
int sock_server_pool_backup(sock_server_pool *ssp, sock_server *ss1, sock_server *ss2);


typedef struct sock_client_pool
{
} sock_client_pool;

//初始化客户端池
sock_client_pool *sock_client_pool_init();

//销毁客户端池
int sock_client_pool_destroy(sock_client_pool *scp);


sock *sock_client_open(sock_attr *attr);

//客户端登陆到服务器
int sock_client_login(sock *s);

//客户端从服务器退出
int sock_client_logout(sock *s);

*/


#endif
