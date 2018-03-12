#ifndef RPC_H
#define RPC_H


/*
����socket�����б�Ҫ������rdma���Ż�����
*/
typedef struct rpc
{
} rpc;

int rpc_register(char *name, void *func);

rpc *rpc_open();

int rpc_run(rpc *r, void *func, void *args);

int rpc_close(rpc *r);


#endif
