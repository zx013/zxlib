#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdlib.h>
#include <string.h>
#include "head.h"
#include "lib.h"

//标记长度
#define HEAD_SIGN								1
//编码信息长度
#define HEAD_CODE								1
//任务信息长度
#define HEAD_TASK								1
//包信息长度
#define HEAD_INFO								1

//4096 * 8，每个包的长度
#define PACKAGE_LENGTH							4096

#define PACKAGE_SIZE							(PACKAGE_LENGTH * sizeof(__int64))

#define PACKAGE_TOTAL_LENGTH					(PACKAGE_LENGTH + HEAD_INFO + HEAD_TASK + HEAD_CODE + HEAD_SIGN)

#define PACKAGE_OFFSET_SIGN					(PACKAGE_LENGTH + HEAD_INFO + HEAD_TASK + HEAD_CODE + HEAD_SIGN - 1)
#define PACKAGE_OFFSET_CODE					(PACKAGE_LENGTH + HEAD_INFO + HEAD_TASK + HEAD_CODE - 1)
#define PACKAGE_OFFSET_TASK					(PACKAGE_LENGTH + HEAD_INFO + HEAD_TASK - 1)
#define PACKAGE_OFFSET_INFO					(PACKAGE_LENGTH + HEAD_INFO - 1)


#define PACKAGE_MASK(bit)						(((__int64)1 << bit) - 1)


#define PACKAGE_SIGN							0x135702469bdf8ace


#define PACKAGE_BIT_CODE_ID					32
#define PACKAGE_BIT_CODE_SIGN					4
#define PACKAGE_BIT_CODE_NUM					16
#define PACKAGE_BIT_CODE_I0					4
#define PACKAGE_BIT_CODE_I1					4
#define PACKAGE_BIT_CODE_I2					4

#define PACKAGE_OFFSET_CODE_ID					0
#define PACKAGE_OFFSET_CODE_SIGN				(PACKAGE_OFFSET_CODE_ID + PACKAGE_BIT_CODE_ID)
#define PACKAGE_OFFSET_CODE_NUM				(PACKAGE_OFFSET_CODE_SIGN + PACKAGE_BIT_CODE_SIGN)
#define PACKAGE_OFFSET_CODE_I0					(PACKAGE_OFFSET_CODE_NUM + PACKAGE_BIT_CODE_NUM)
#define PACKAGE_OFFSET_CODE_I1					(PACKAGE_OFFSET_CODE_I0 + PACKAGE_BIT_CODE_I0)
#define PACKAGE_OFFSET_CODE_I2					(PACKAGE_OFFSET_CODE_I1 + PACKAGE_BIT_CODE_I1)

#define PACKAGE_GET_CODE(d, x)					((d >> PACKAGE_OFFSET_CODE_##x) & PACKAGE_MASK(PACKAGE_BIT_CODE_##x))
#define PACKAGE_SET_CODE(x)					(PACKAGE_OFFSET_CODE_##x)

#define PACKAGE_CODE_SIGN_DATA					0
#define PACKAGE_CODE_SIGN_CODE					1

typedef struct package_code
{
	__int64 id; //单元编号，32
	__int64 sign; //包标识，4
	__int64 num; //包顺序（unit的发送顺序）
	__int64 i[3]; //包位置，4 * 3
} package_code;


#define PACKAGE_BIT_TASK_ID					32
#define PACKAGE_BIT_TASK_EMPTY					1
#define PACKAGE_BIT_TASK_CACHE					1
#define PACKAGE_BIT_TASK_RETRANS				1
#define PACKAGE_BIT_TASK_TYPE					4
#define PACKAGE_BIT_TASK_COMPRESS				8
#define PACKAGE_BIT_TASK_ENCRYPT				8
#define PACKAGE_BIT_TASK_STATE					4

#define PACKAGE_OFFSET_TASK_ID					0
#define PACKAGE_OFFSET_TASK_EMPTY				(PACKAGE_OFFSET_TASK_ID + PACKAGE_BIT_TASK_ID)
#define PACKAGE_OFFSET_TASK_CACHE				(PACKAGE_OFFSET_TASK_EMPTY + PACKAGE_BIT_TASK_EMPTY)
#define PACKAGE_OFFSET_TASK_RETRANS			(PACKAGE_OFFSET_TASK_CACHE + PACKAGE_BIT_TASK_CACHE)
#define PACKAGE_OFFSET_TASK_TYPE				(PACKAGE_OFFSET_TASK_RETRANS + PACKAGE_BIT_TASK_RETRANS)
#define PACKAGE_OFFSET_TASK_COMPRESS			(PACKAGE_OFFSET_TASK_TYPE + PACKAGE_BIT_TASK_TYPE)
#define PACKAGE_OFFSET_TASK_ENCRYPT			(PACKAGE_OFFSET_TASK_COMPRESS + PACKAGE_BIT_TASK_COMPRESS)
#define PACKAGE_OFFSET_TASK_STATE				(PACKAGE_OFFSET_TASK_ENCRYPT + PACKAGE_BIT_TASK_ENCRYPT)

#define PACKAGE_GET_TASK(d, x)					((d >> PACKAGE_OFFSET_TASK_##x) & PACKAGE_MASK(PACKAGE_BIT_TASK_##x))
#define PACKAGE_SET_TASK(x)					(PACKAGE_OFFSET_TASK_##x)

#define PACKAGE_TASK_TYPE_CONTROL				0
#define PACKAGE_TASK_TYPE_REMOTE				1
#define PACKAGE_TASK_TYPE_SYSTEMCALL			2
#define PACKAGE_TASK_TYPE_HEARTBEAT			3
#define PACKAGE_TASK_TYPE_FILEINFO				4
#define PACKAGE_TASK_TYPE_FILE					5

#define PACKAGE_TASK_STATE_NORMAL				0
#define PACKAGE_TASK_STATE_FINISH				1
#define PACKAGE_TASK_STATE_CANCEL				2
#define PACKAGE_TASK_STATE_ERROR				3

typedef struct package_task
{
	__int64 id; //任务编号，32
	__int64 empty; //是否为空包，1
	__int64 cache; //缓存标识，1
	__int64 retrans; //重传标识，1
	__int64 type; //任务类型，4
	__int64 compress; //压缩类型，8
	__int64 encrypt; //加密类型，8
	__int64 state; //任务状态，4
} package_task;


#define PACKAGE_BIT_INFO_ID					32
#define PACKAGE_BIT_INFO_NUM					32

#define PACKAGE_OFFSET_INFO_ID					0
#define PACKAGE_OFFSET_INFO_NUM				(PACKAGE_OFFSET_INFO_ID + PACKAGE_BIT_INFO_ID)

#define PACKAGE_GET_INFO(d, x)					((d >> PACKAGE_OFFSET_INFO_##x) & PACKAGE_MASK(PACKAGE_BIT_INFO_##x))
#define PACKAGE_SET_INFO(x)					(PACKAGE_OFFSET_INFO_##x)

typedef struct package_info
{
	__int64 id; //包编号，32
	__int64 num; //包数量，32
} package_info;

typedef struct package
{
	int exist;
	package_code code;
	package_task task;
	package_info info;
	__int64 data[PACKAGE_TOTAL_LENGTH]; //long long 64位，可包含8个char类型
	int len; //package中的数据长度
} package;


inline package *package_malloc();

inline int package_free(package *p);

inline int package_reset(package *p);

inline int package_copy(package *p1, package *p2);

//连续的异或的速度远快于依次异或的速度
#define package_xor(p0, ...) MACROCAT(package_xor, __VA_NARG__(__VA_ARGS__))(p0, __VA_ARGS__)

inline int package_xor2(package *p0, package *p1, package *p2);
inline int package_xor3(package *p0, package *p1, package *p2, package *p3);
inline int package_xor4(package *p0, package *p1, package *p2, package *p3, package *p4);
inline int package_xor5(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5);
inline int package_xor6(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6);
inline int package_xor7(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6, package *p7);
inline int package_xor8(package *p0, package *p1, package *p2, package *p3, package *p4, package *p5, package *p6, package *p7, package *p8);

inline int package_get_code(package *p);

inline int package_set_code(package *p);

inline int package_get_task(package *p);

inline int package_set_task(package *p);

inline int package_get_info(package *p);

inline int package_set_info(package *p);

inline int package_get(package *p);

inline int package_set(package *p);


//用于异或操作
typedef struct package_stack
{
	package **pl;
	int num;
} package_stack;


inline package_stack *package_stack_init();

inline int package_stack_destroy(package_stack *ps);

inline int package_stack_push(package_stack *ps, package *p);

inline int package_stack_xor(package_stack *ps, package *p);


#endif
