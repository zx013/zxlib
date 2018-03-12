#ifndef REDUNDANCY_H
#define REDUNDANCY_H
/*
�������Ϊk������볤Ϊ1 + 2k��k=0ʱ�൱��ֱ�����
1.������1��1�ı��룬��1 + 2k
2.������1 + k��1�ı��룬��(1 + 2k)!/(1 + k)!(k)!��k=2ʱΪ10
3.������1 + 2k��1�ı��룬��1

k=2ʱ����00001, 01110, 11111��

��2, 3��˳��1 - m��ţ�k=2ʱ����
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

��1��˳��1 - n��ţ���i�������е�1���ڵ�iλ��k=2ʱ����
01 10000
02 01000
03 00100
04 00010
05 00001

�������˳��m�����ݰ����õ�n�������У���i�������а������е�iλΪ1�����ݰ���k=2ʱ����
n1 00001111111
n2 01110001111
n3 10110110011
n4 11011010101
n5 11101101001

ֻ���Ƕ���������µĶ����ָ����൱����֪����λ�õľ���
��ÿ����������ݰ�ȫ��������ɶ�Ӧ�ı������k=2ʱ��11�����ݰ�������5�����������������Ϊ3


��Ҫ��֤���о�������
1.���Ϊk������£���������Ϊ3�������߾�������
2.����������Ϊk�ı��룬��ȥ1��1��ȫ1�������������kλ0��k+1λ1��ɵ�1 + 2kλ���ȵı����У�����������Ϊk�ı���


*/
#include "lib.h"
#include "package.h"
#include "list.h"
#include "queue.h"
#include "pool.h"
#include "error.h"


/*
��ȫ����Ĵ���Ч��Ϊ43%���ɽ��в��ֱ�������ߴ���Ч�ʣ�����Ч�ʵı仯�����Ƕ�̬��
*/

//���ݳ���
#define REDUNDANCY_LENGTH_DATA						11
//���볤��
#define REDUNDANCY_LENGTH_CODE						5

#define REDUNDANCY_LENGTH							(REDUNDANCY_LENGTH_DATA + REDUNDANCY_LENGTH_CODE)

#define REDUNDANCY_STEP							((__int64)1 << 60)


/*
c_, data����
d_, code����
�����д��䵥ԪΪpackageʱ�����м����ٶȣ���������Ϊ213GB/s������Ӱ�쵽�����Ĵ���
*/
typedef struct redundancy
{
	__int64 id; //���
	int dn, cn; //���ݰ��ͱ��������
	package *dp[REDUNDANCY_LENGTH_DATA]; //���ݰ�
	package *cp[REDUNDANCY_LENGTH_CODE]; //�����

	char mark[REDUNDANCY_LENGTH]; //������
	list_t *list; //�����������

	int n; //��ǰ���뵥Ԫ��ά��
	int i; //��ǰ���뵥Ԫ�ı��

	package_stack *ps; //���ڿ������ĵ�Ԫ
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
