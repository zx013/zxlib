#ifndef MUTEX_H
#define MUTEX_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "head.h"
#include "error.h"


#ifdef _WIN32
#define mutex CRITICAL_SECTION
#else
#define mutex pthread_mutex_t
#endif


int mutex_init(mutex *lock);

int mutex_destroy(mutex *lock);

int mutex_lock(mutex *lock);

int mutex_trylock(mutex *lock);

int mutex_unlock(mutex *lock);


#endif