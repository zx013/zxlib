#include "thread.h"


//返回最后一个开启线程的pid，不设置detach会导致内存泄漏
unsigned long thread_create(thread *t, void *func, void *args)
{
	unsigned long pid;

#ifdef _WIN32
	*t = (HANDLE)_beginthreadex(0, 0, func, args, 0, 0);
	pid = GetThreadId(*t);
	if(*t == NULL)
		return 0;
#else
	if(pthread_create(t, NULL, func, args) != 0)
		return 0;
	pid = *t;
#endif
	return pid;
}

int thread_run(thread *t, void *func, void *args)
{
	if(thread_create(t, func, args) == 0)
		return FALSE;
	thread_detach(*t);
	return TRUE;
}

int thread_exit()
{
#ifdef _WIN32
	_endthread();
#else
	pthread_exit(0);
#endif
}

int thread_detach(thread t)
{
#ifdef _WIN32
#else
	pthread_detach(t);
#endif
}

int thread_join(thread t)
{
#ifdef _WIN32
	WaitForSingleObject(t, INFINITE);
#else
	pthread_join(t, NULL);
#endif
}

unsigned long thread_id()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

int thread_alive(thread t)
{
	if(!t)
		return FALSE;
#ifdef _WIN32
	DWORD sign;
	GetExitCodeThread(t, &sign);
	if(sign != STILL_ACTIVE)
		return FALSE;
#else
	int sign;
	sign = pthread_kill(t, 0);
	if(sign == ESRCH || sign == EINVAL)
		return FALSE;
#endif
	return TRUE;
}

#ifndef _WIN32
#include <signal.h> 

#define SIGRESUME SIGUSR2  
#define SIGSUSPEND SIGUSR1
static __thread int g_bSuspend;

void suspend_handler(int signum)
{
    g_bSuspend = 1;

    sigset_t nset;
    pthread_sigmask(0, NULL, &nset);
    /* make sure that the resume is not blocked*/
    sigdelset(&nset, SIGRESUME);
    while(g_bSuspend) sigsuspend(&nset);
}

void resume_handler(int signum)
{
    g_bSuspend = 0;
}

int thread_signal()
{
	static int init = FALSE;
	if(init == FALSE)
	{
		struct sigaction suspendsa = {0};
		struct sigaction resumesa = {0};
		suspendsa.sa_handler = suspend_handler;
		sigaddset(&suspendsa.sa_mask, SIGRESUME);
		sigaction(SIGSUSPEND, &suspendsa, NULL);
	
		resumesa.sa_handler = resume_handler;
		sigaddset(&resumesa.sa_mask, SIGSUSPEND);
		sigaction(SIGRESUME, &resumesa, NULL);

		init = TRUE;
	}
}
#endif

int thread_suspend(thread t)
{
#ifdef _WIN32
	SuspendThread(t);
#else
	thread_signal();
	return pthread_kill(t, SIGSUSPEND);
#endif
}

int thread_resume(thread t)
{
#ifdef _WIN32
	ResumeThread(t);
#else
	thread_signal();
	return pthread_kill(t, SIGRESUME);
#endif
}

//强制关闭线程
int thread_kill(thread t)
{
#ifdef _WIN32
	TerminateThread(t, 0);
#else
	pthread_cancel(t);
#endif
	while(thread_alive(t))
		tools_yield(); //等待线程被关闭
	return TRUE;
}

//提高线程的优先级
void thread_priority(int priority)
{
#ifdef _WIN32
	//SetThreadPriorityBoost
#else
	pthread_t pid;
	struct sched_param param;

	pid = thread_id();

	param.sched_priority = priority;
	pthread_setschedparam(pid, SCHED_FIFO, &param);
#endif
}
