#ifndef RPC_H
#define RPC_H


/*
先用socket，如有必要，可用rdma来优化性能
*/
typedef struct rpc
{
} rpc;

int rpc_register(char *name, void *func);

rpc *rpc_open();

int rpc_run(rpc *r, void *func, void *args);

int rpc_close(rpc *r);


#endif
