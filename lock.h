#ifndef LOCK_H
#define LOCK_H

#ifdef _WIN32
#include <windows.h>
#include <pthread.h>
#else
#include <sched.h>
#endif
#include "head.h"

#ifdef _WIN32
#define lock_cancel()
#else
#define lock_cancel() pthread_testcancel()
#endif

#ifdef _WIN32
#define lock_yield() SwitchToThread()
#else
#define lock_yield() sched_yield()
#endif


#define lock(...) __VA_FUNC__(lock, __VA_ARGS__)

#define lock_1(__lock) \
	({ \
		while(!trylock(__lock)) \
			lock_yield(); \
	})

#define lock_2(__lock, __cond) \
	({ \
		int __ret = TRUE; \
		while(1) \
		{ \
			lock_cancel(); \
			if(trylock(__lock)) \
			{ \
				if(__cond) \
					break; \
				unlock(__lock); \
			} \
			lock_yield(); \
		} \
		__ret; \
	})

/*
��һ������Ϊ��������ȡ��ַ���ڶ�������Ϊ�ж�������û����TRUE������������Ϊ��ʱʱ�䣬�����ó�ʱ��-1
���ر��߳�ʱ�����رյ��߳��ڼ���״̬�У����ᱻ�رգ���ʹ�õ���Դ�Ѿ����ͷţ������ɶδ���
t=0ʱ���൱��trylock
*/
#define lock_3(__lock, __cond, __time) \
	({ \
		int __ret = TRUE; \
		struct timeval __tv1, __tv2; \
		gettimeofday(&__tv1, NULL); \
		while(1) \
		{ \
			lock_cancel(); \
			if(trylock(__lock)) \
			{ \
				if(__cond) \
					break; \
				unlock(__lock); \
			} \
			lock_yield(); \
			if(__time >= 0) \
			{ \
				gettimeofday(&__tv2, NULL); \
				if((__tv2.tv_sec - __tv1.tv_sec) S + (__tv2.tv_usec - __tv1.tv_usec) > __time) \
				{ \
					__ret = FALSE; \
					break; \
				} \
			} \
		} \
		__ret; \
	})

#define trylock(__lock) \
	({ \
		__sync_bool_compare_and_swap(&(__lock), 0, 1); \
	})

#define unlock(__lock) \
	({ \
		__sync_lock_release(&(__lock)); \
	})

#define with_lock(__lock, __body) \
	({ \
		typeof((__body)) __result; \
		lock(__lock); \
		__result = (__body); \
		unlock(__lock); \
		__result; \
	})


//��д��
typedef struct rwlock
{
	unsigned char disable;
	unsigned char rl;
	unsigned char wl;
	unsigned char rwl;
	unsigned short rc;
	unsigned short wc;
} rwlock;

#define rlock(__lock) \
	({ \
		if(((rwlock *)(&__lock))->disable == 0) \
		{ \
			lock(((rwlock *)(&__lock))->rl); \
			if(__sync_fetch_and_add(&((rwlock *)(&__lock))->rc, 1) == 0) \
				lock(((rwlock *)(&__lock))->rwl); \
			unlock(((rwlock *)(&__lock))->rl); \
		} \
	})

#define runlock(__lock) \
	({ \
		if(((rwlock *)(&__lock))->disable == 0) \
		{ \
			lock(((rwlock *)(&__lock))->rl); \
			if(__sync_fetch_and_sub(&((rwlock *)(&__lock))->rc, 1) == 1) \
				unlock(((rwlock *)(&__lock))->rwl); \
			unlock(((rwlock *)(&__lock))->rl); \
		} \
	})

#define wlock(__lock) \
	({ \
		if(((rwlock *)(&__lock))->disable == 0) \
		{ \
			lock(((rwlock *)(&__lock))->wl); \
			if(__sync_fetch_and_add(&((rwlock *)(&__lock))->wc, 1) == 0) \
				lock(((rwlock *)(&__lock))->rl); \
			unlock(((rwlock *)(&__lock))->wl); \
			lock(((rwlock *)(&__lock))->rwl); \
		} \
	})

#define wunlock(__lock) \
	({ \
		if(((rwlock *)(&__lock))->disable == 0) \
		{ \
			unlock(((rwlock *)(&__lock))->rwl); \
			lock(((rwlock *)(&__lock))->wl); \
			if(__sync_fetch_and_sub(&((rwlock *)(&__lock))->wc, 1) == 1) \
				unlock(((rwlock *)(&__lock))->rl); \
			unlock(((rwlock *)(&__lock))->wl); \
		} \
	})

#define with_rlock(__lock, __body) \
	({ \
		typeof((__body)) __result; \
		rlock(__lock); \
		__result = (__body); \
		runlock(__lock); \
		__result; \
	})

#define with_wlock(__lock, __body) \
	({ \
		typeof((__body)) __result; \
		wlock(__lock); \
		__result = (__body); \
		wunlock(__lock); \
		__result; \
	})

//���������ڽṹ�ı�����ʹ��
#define disable_rwlock(__lock, __body) \
	({ \
		typeof((__body)) __result; \
		((rwlock *)(&__lock))->disable = 1; \
		__result = (__body); \
		((rwlock *)(&__lock))->disable = 0; \
		__result; \
	})


#endif
