#ifndef SOCK_H
#define SOCK_H

/*
ʵ�ָ߿����Ժ͸��ؾ���

���ô�����������ɶ�̨
���ô����������Ӧ�ı��ݷ�������ÿ̨���������������һ̨���ݷ����������ñ��ݷ�����
�����˱��ݷ������Ĵ����������Ϊ��ȫ��������������������˰�ȫ���ԣ�������ͨ����ȫ���������䣨������Ϊֻ��ͨ����ȫ���������䣩

���ʹ���
�����ַ�
�����л�


���մ���
*/

/*
//�������ݰ�
sock_send

//�������ݰ�
sock_recv
*/

/*
һ�����յ�ʵ��
���Ͷ˽����ݰ��Ը��ؾ���ķ�ʽ���͵����ն�
���ն��յ����ݺ󣬽�����ת���������Ľ��ն�

���ն����ߺ����ߵĴ���
����ʱ�����ն˼��븺�ؾ�����У�����ʱ�����ն˴Ӹ��ؾ�������Ƴ�


���Ͷ˺ͽ��ն˽����������Ӻͽ������ӣ���������ֻ���������ݰ�����������������������״̬


���Ͷ��߼�
���Ͷ����������ݰ����͵��������նˣ��������ݰ�����
���Ͷ˴ӽ��ն˻�ȡ������Ϣ������ѽ������ݰ����������ݰ��ѱ��������߿ͻ��˽��գ�������û���
���н��ն����ߣ�����пͻ���δ���ոð����������ݰ����·��͵�һ��δ���յĿͻ���

���ն��߼�
���ն˵�½����ȡ�ѵ�½�Ľ��ն��б��������ѵ�½���ն˵����ӣ�����н��ն˵ǳ������ܻ�����ʧ�ܣ�
���ն��յ����ݰ��󣬽����ݰ�ת�����������ն�

���ն˶�ʱ�������������Ͷˣ������ѽ��յ����ݰ�


*/

#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <curl.h> //�������head.hǰ
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

//��ʼ����������
sock_server_pool *sock_server_pool_init();

//���ٷ�������
int sock_server_pool_destroy(sock_server_pool *ssp);

//����������ӵ�����������
int sock_server_pool_insert(sock_server_pool *ssp, sock_server *ss);

//�ϲ�������������
int sock_server_pool_merge(sock_server_pool *ssp1, sock_server_pool *ssp2);

//ͬ����������
int sock_server_pool_sync(sock_server_pool *ssp);

//������������Ϊ���������
int sock_server_pool_channel(sock_server_pool *ssp, sock_server *ss);

//������������Ϊ���ݷ�����
int sock_server_pool_backup(sock_server_pool *ssp, sock_server *ss1, sock_server *ss2);


typedef struct sock_client_pool
{
} sock_client_pool;

//��ʼ���ͻ��˳�
sock_client_pool *sock_client_pool_init();

//���ٿͻ��˳�
int sock_client_pool_destroy(sock_client_pool *scp);


sock *sock_client_open(sock_attr *attr);

//�ͻ��˵�½��������
int sock_client_login(sock *s);

//�ͻ��˴ӷ������˳�
int sock_client_logout(sock *s);

*/


#endif
