#include "lost.h"


#include <stdio.h>

//丢失整个unit的处理
int lost_unit(int id)
{
	printf("lost unit: id = %d\n", id);
	fflush(stdout);
}

//丢失unit中的一个package的处理，package可能为空包
int lost_package(int id, int num)
{
	printf("lost package: id = %d, num = %d\n", id, num);
	fflush(stdout);
}
