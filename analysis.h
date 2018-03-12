#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "dict.h"
#include "unit.h"
#include "queue.h"
#include "pool.h"

//整数
#define ANALYSIS_INT							(1 << 0)
//字符串
#define ANALYSIS_STR							(1 << 1)
//开始时调用，释放前一个包
#define ANALYSIS_FREE							(1 << 2)
//结束时调用，将包压入其它队列
#define ANALYSIS_RESET							(1 << 3)

//数据的最大位数
#define ANALYSIS_MAX							60

//初始化，data设置为analysis_attr
#define ANALYSIS_INIT							(1 << 0)
//取消
#define ANALYSIS_CANCEL							(1 << 1)
//错误
#define ANALYSIS_ERROR							(1 << 2)
//结束，最后一个数据时带入该参数
#define ANALYSIS_END							(1 << 3)
//直接结束，不带入数据
#define ANALYSIS_FINISH							(1 << 4)
//创建一个新的package获取数据
#define ANALYSIS_NEW							(1 << 5)
//将当前package中的内容设置为前缀
#define ANALYSIS_PREFIX							(1 << 6)
//压缩
#define ANALYSIS_COMPRESS						(1 << 7)
//加密
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
	int offset_len;	//已填充长度
	int remain_len; //剩余空间长度
	char prefix[PACKAGE_SIZE]; //前缀，不能太大
	int prefix_len; //前缀长度
} analysis;


int analysis_pack(queue *q, void *data, __int64 len, int sign);

int analysis_unpack(queue *q, void **data, __int64 *len, int sign);

inline package *analysis_package();

inline int analysis_remain();

int analysis_allocate(queue *q);

queue *analysis_queue(int type);


#endif
