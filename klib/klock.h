#ifndef _AC_KLOCK_H
#define _AC_KLOCK_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sched.h>
#endif

typedef char klock_t;

#ifdef _WIN32
#define kyield() SwitchToThread()
#else
#define kyield() sched_yield()
#endif

#define klock(lock) \
	({ \
		while(!__sync_bool_compare_and_swap(&lock, 0, 1)) \
			kyield(); \
	})

#define ktrylock(lock) \
	({ \
		__sync_bool_compare_and_swap(&lock, 0, 1); \
	})

#define kunlock(lock) \
	({ \
		__sync_lock_release(&lock); \
	})


#endif