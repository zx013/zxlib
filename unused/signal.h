#ifndef SIGNAL_H
#define SIGNAL_H

#ifdef _WIN32
#else
#include <sys/time.h>
#endif
#include "mutex.h"
#include "error.h"

#ifdef _WIN32
#define signal HANDLE
#else
#define signal pthread_cond_t
#endif


int signal_init(signal *cond);

int signal_event(signal *cond);

int signal_wait(signal *cond, mutex *lock, int tm);

int signal_destroy(signal *cond);


#endif
