#ifndef LIMIT_H
#define LIMIT_H

#include "head.h"
#include "lib.h"
#include "error.h"


/*
�ýӿ�Ϊ�ȴ�С���ݰ��ļ���
limit, sizeΪ���������interval, durationΪ��ѡ������intervalĬ��Ϊ100MS��durationĬ��Ϊ1S
*/
typedef struct limit_speed_attr
{
	__int64 limit; //���ٴ�С
	int size; //һ�μ����Ĵ�С
	__int64 interval; //�������
	__int64 duration; //����ʱ��
} limit_speed_attr;

typedef struct limit_speed
{
	__int64 limit;
	int size; //һ�μ����Ĵ�С
	__int64 interval; //�������
	__int64 duration; //����ʱ��
	int times; //��������

	__int64 count; //�ܼ���

	int index;
	__int64 *data;

	struct timeval time_point; 
} limit_speed;

inline limit_speed *limit_speed_init(limit_speed_attr *attr);

inline int limit_speed_check(limit_speed *ls);

inline void limit_speed_add(limit_speed *ls);

inline int limit_speed_destroy(limit_speed *ls);


#endif
