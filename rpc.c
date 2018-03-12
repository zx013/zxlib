#include "rpc.h"


#ifdef TEST

#include "pool.h"
#include "test.h"


void rpc_test_1()
{
}

void rpc_test_2()
{
}

void rpc_test()
{
	pool *p;
	p = pool_init(2);
	pool_start(p, rpc_test_2, NULL);
	tools_sleep(100 MS);
	pool_start(p, rpc_test_1, NULL);
	pool_join(p, NULL);
	pool_destroy(p);
}

#endif
