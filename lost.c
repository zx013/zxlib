#include "lost.h"


#include <stdio.h>

//��ʧ����unit�Ĵ���
int lost_unit(int id)
{
	printf("lost unit: id = %d\n", id);
	fflush(stdout);
}

//��ʧunit�е�һ��package�Ĵ���package����Ϊ�հ�
int lost_package(int id, int num)
{
	printf("lost package: id = %d, num = %d\n", id, num);
	fflush(stdout);
}
