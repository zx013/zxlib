#ifndef HEAD_H
#define HEAD_H

#include <stdint.h>


//不定参最大个数，1为16个，2为32个，3为64个，每一级提升12-13%的编译速度
#define __VA_MAX__							1

//判断是否是大端
static const unsigned int __ENDIAN = 0x12345678;
#define ENDIAN (*(unsigned char *)&__ENDIAN == 0x12)


#ifndef _WIN32
#define __int64							long long
#endif

typedef __int64							int64;
typedef char *							str;


//错误，用于成功时返回数量等参数的场景
#define NONE								-1
//错误
#define FALSE								0
//正确
#define TRUE								1

//发送端
#define SIDE_SEND							0
//接收端
#define SIDE_RECV							1


//信息大小
#define INFO_SIZE							1024

//指令大小
#define COMMAND_SIZE						4096

//参数大小
#define PARAMETER_SIZE						1024

//参数数量
#define PARAMETER_NUMBER					16

//用户名长度
#define USERNAME_SIZE						128

//路径长度
#define PATH_SIZE							4096

//1B
#define B									* ((__int64)1)

//1 KB
#define KB									* ((__int64)1024 B)

//1 MB
#define MB									* ((__int64)1024 KB)

// 1GB
#define GB									* ((__int64)1024 MB)

// 1TB
#define TB									* ((__int64)1024 GB)

//1us
#define US									* ((__int64)1)

//1ms
#define MS									* ((__int64)1000 US)

//1s
#define S									* ((__int64)1000 MS)

//1min
#define MIN								* ((__int64)60 S)

//1hour
#define HOUR								* ((__int64)60 MIN)

//1day
#define DAY								* ((__int64)24 HOUR)

//类型转换，大小默认为B，时间默认为US
#define TO(x)								/ (1 x)

#define TOSTRING(s)						#s

#define _MACROCAT(x, y)					x##y
#define MACROCAT(x, y)						_MACROCAT(x, y)

//__VA_NARG__(__VA_ARGS__)获取不定参数个数，多加一个0与##是为了解决参数为0时会获取成1的问题
#define __VA_NARG__(...) __VA_NARG_(0, ##__VA_ARGS__, __VA_RSEQ_N())
#define __VA_NARG_(...) __VA_ARG_N(__VA_ARGS__)

#if __VA_MAX__ == 1

#define __VA_ARG_N( \
	 _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15, N,...) N

#define __VA_RSEQ_N() \
	14,13,12,11,10, \
	 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __VA_RSEQ_1() \
	0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,1,1

#define __VA_RSEQ_2() \
	0,0,0,0,0,0, \
	0,0,0,0,0,0,0,1,1,1

#elif __VA_MAX__ == 2

#define __VA_ARG_N( \
	 _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,N,...) N

#define __VA_RSEQ_N() \
	30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __VA_RSEQ_1() \
	0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,1,1

#define __VA_RSEQ_2() \
	0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,1,1,1

#elif __VA_MAX__ == 3

#define __VA_ARG_N( \
	 _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,N,...) N

#define __VA_RSEQ_N() \
	62,61,60, \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __VA_RSEQ_1() \
	0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,1,1

#define __VA_RSEQ_2() \
	0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,1,1,1

#endif

//不定参数函数，格式为func_n，n为参数数量
#define __VA_FUNC__(func, ...) MACROCAT(func, MACROCAT(_, __VA_NARG__(__VA_ARGS__)))(__VA_ARGS__)

//获取第一个元素
#define __VA_GETFIR__(n, ...) n
//获取第二个元素
#define __VA_GETSEC__(x, n, ...) n

//获取最后一个元素
#define __VA_GETLAST__0(x, ...) __VA_DEFER__(__VA_GETLAST__I)()(__VA_ARGS__)
#define __VA_GETLAST__1(...) __VA_ARGS__
#define __VA_GETLAST__I() __VA_GETLAST

#define __VA_GETLAST(...) MACROCAT(__VA_GETLAST__, __VA_NARG_1__(__VA_ARGS__))(__VA_ARGS__)
#define __VA_GETLAST__(...) __VA_EVAL__(__VA_GETLAST(__VA_ARGS__))

//获取除最后一个元素外的剩余元素
#define __VA_EXCLUDE__0(x, ...) x, __VA_DEFER__(__VA_EXCLUDE__I)()(__VA_ARGS__)
#define __VA_EXCLUDE__1(n, ...) n
#define __VA_EXCLUDE__I() __VA_EXCLUDE

#define __VA_EXCLUDE(...) MACROCAT(__VA_EXCLUDE__, __VA_NARG_2__(__VA_ARGS__))(__VA_ARGS__)
#define __VA_EXCLUDE__(...) __VA_EVAL__(__VA_EXCLUDE(__VA_ARGS__))

#define __VA_CHECKFIR__(...) __VA_GETFIR__(__VA_ARGS__, 0)
#define __VA_CHECKSEC__(...) __VA_GETSEC__(__VA_ARGS__, 0)

//判断不定参数是否为1或0个
#define __VA_NARG_1__(...) __VA_NARG_(__VA_ARGS__, __VA_RSEQ_1())
//判断不定参数是否为2，1或0个
#define __VA_NARG_2__(...) __VA_NARG_(__VA_ARGS__, __VA_RSEQ_2())


#define EMPTY()

#define EVAL(...)  EVAL1(EVAL1(EVAL1(EVAL1(__VA_ARGS__))))
#define EVAL1(...)  EVAL2(EVAL2(EVAL2(EVAL2(__VA_ARGS__))))

#if __VA_MAX__ == 1
#define EVAL2(...)  __VA_ARGS__
#elif __VA_MAX__ == 2
#define EVAL2(...)  EVAL3(EVAL3(__VA_ARGS__))
#elif __VA_MAX__ == 3
#define EVAL2(...)  EVAL3(EVAL3(EVAL3(EVAL3(__VA_ARGS__))))
#endif
#define EVAL3(...)  __VA_ARGS__

//__VA_DEFER__, __VA_EVAL__用于宏的递归展开
#define __VA_DEFER__(id) id EMPTY()
#define __VA_EVAL__(...) EVAL(__VA_ARGS__)


#endif
