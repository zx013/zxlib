#ifndef REDUNDANCY_H
#define REDUNDANCY_H
/*
生成码距为k的码表，码长为1 + 2k（k=0时相当于直接异或）
1.生成有1个1的编码，共1 + 2k
2.生成有1 + k个1的编码，共(1 + 2k)!/(1 + k)!(k)!，k=2时为10
3.生成有1 + 2k个1的编码，共1

k=2时，如00001, 01110, 11111等

将2, 3按顺序按1 - m编号，k=2时如下
01 00111
02 01011
03 01101
04 01110
05 10011
06 10101
07 10110
08 11001
09 11010
10 11100
11 11111

将1按顺序按1 - n编号，第i个编码中的1所在第i位，k=2时如下
01 10000
02 01000
03 00100
04 00010
05 00001

按编码的顺序将m个数据包放置到n个分组中，第i个分组中包含所有第i位为1的数据包，k=2时如下
n1 00001111111
n2 01110001111
n3 10110110011
n4 11011010101
n5 11101101001

只考虑丢包的情况下的丢包恢复，相当于已知错误位置的纠错
将每个分组的数据包全部异或，生成对应的编码包，k=2时，11个数据包，生成5个编码包，纠错能力为3


需要验证和研究的问题
1.码距为k的情况下，纠错能力为3，如何提高纠错能力
2.如何生成码距为k的编码，除去1个1和全1的情况，即在有k位0和k+1位1组成的1 + 2k位长度的编码中，如何生成码距为k的编码


*/
#include "lib.h"
#include "package.h"
#include "list.h"
#include "queue.h"
#include "pool.h"
#include "error.h"


/*
完全编码的传输效率为43%，可进行部分编码以提高传输效率，传输效率的变化可以是动态的
*/

//数据长度
#define REDUNDANCY_LENGTH_DATA						11
//编码长度
#define REDUNDANCY_LENGTH_CODE						5

#define REDUNDANCY_LENGTH							(REDUNDANCY_LENGTH_DATA + REDUNDANCY_LENGTH_CODE)

#define REDUNDANCY_STEP							((__int64)1 << 60)


/*
c_, data类型
d_, code类型
当队列传输单元为package时，队列极限速度（无阻塞）为213GB/s，不会影响到正常的传输
*/
typedef struct redundancy
{
	__int64 id; //编号
	int dn, cn; //数据包和编码包计数
	package *dp[REDUNDANCY_LENGTH_DATA]; //数据包
	package *cp[REDUNDANCY_LENGTH_CODE]; //编码包

	char mark[REDUNDANCY_LENGTH]; //解码标记
	list_t *list; //解码包的序列

	int n; //当前编码单元的维度
	int i; //当前编码单元的编号

	package_stack *ps; //用于快速异或的单元
} redundancy;

typedef struct redundancy2
{
	__int64 id;
	int i1;
	int i2;
	redundancy *r1[REDUNDANCY_LENGTH];
	redundancy *r2[REDUNDANCY_LENGTH];
	list_t *list;

	queue *q1;
	queue *q2;
} redundancy2;

typedef struct redundancy_multi
{
	int num;
	pool *p;
	queue *q;
	redundancy2 **r;
} redundancy_multi;


redundancy *redundancy_init();

int redundancy_destroy(redundancy *r);

redundancy2 *redundancy2_init();

int redundancy2_destroy(redundancy2 *r);

inline int redundancy_encode(redundancy *r, package *p);

inline int redundancy2_encode(redundancy2 *r, package *p);

package *redundancy_decode(redundancy *r, queue *q);

package *redundancy2_decode(redundancy2 *r, queue *q);

redundancy_multi *redundancy_multi_init(int num);

int redundancy_multi_destroy(redundancy_multi *rm);


#endif
