#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "dict.h"
#include "unit.h"
#include "queue.h"
#include "pool.h"

//����
#define ANALYSIS_INT							(1 << 0)
//�ַ���
#define ANALYSIS_STR							(1 << 1)
//��ʼʱ���ã��ͷ�ǰһ����
#define ANALYSIS_FREE							(1 << 2)
//����ʱ���ã�����ѹ����������
#define ANALYSIS_RESET							(1 << 3)

//���ݵ����λ��
#define ANALYSIS_MAX							60

//��ʼ����data����Ϊanalysis_attr
#define ANALYSIS_INIT							(1 << 0)
//ȡ��
#define ANALYSIS_CANCEL							(1 << 1)
//����
#define ANALYSIS_ERROR							(1 << 2)
//���������һ������ʱ����ò���
#define ANALYSIS_END							(1 << 3)
//ֱ�ӽ���������������
#define ANALYSIS_FINISH							(1 << 4)
//����һ���µ�package��ȡ����
#define ANALYSIS_NEW							(1 << 5)
//����ǰpackage�е���������Ϊǰ׺
#define ANALYSIS_PREFIX							(1 << 6)
//ѹ��
#define ANALYSIS_COMPRESS						(1 << 7)
//����
#define ANALYSIS_ENCRYPT						(1 << 8)

typedef struct analysis_attr
{
	package_task task; //cache, retrans, type, compress, encrypt
	package_info info; //num
} analysis_attr;

typedef struct analysis
{
	analysis_attr attr;
	package *p;
	int id;
	int offset_len;	//����䳤��
	int remain_len; //ʣ��ռ䳤��
	char prefix[PACKAGE_SIZE]; //ǰ׺������̫��
	int prefix_len; //ǰ׺����
} analysis;


int analysis_pack(queue *q, void *data, __int64 len, int sign);

int analysis_unpack(queue *q, void **data, __int64 *len, int sign);

inline package *analysis_package();

inline int analysis_remain();

int analysis_allocate(queue *q);

queue *analysis_queue(int type);


#endif
