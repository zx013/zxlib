#ifndef THREAD_H
#define THREAD_H

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#endif
#include "head.h"
#include "tools.h"

#ifdef _WIN32
#define thread HANDLE
#else
#define thread pthread_t
#endif

unsigned long thread_create(thread *t, void *func, void *args);

int thread_run(thread *t, void *func, void *args);

int thread_exit();

unsigned long thread_id();

int thread_alive(thread t);

int thread_suspend(thread t);

int thread_resume(thread t);

int thread_kill(thread t);

int thread_detach(thread t);

int thread_join(thread t);

void thread_priority(int priority);


#endif
