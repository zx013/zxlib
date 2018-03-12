#include "signal.h"

int signal_init(signal *cond)
{
#ifdef _WIN32
	*cond = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(*cond == NULL)
		error_goto(signal_init_error_1);
	return TRUE;

signal_init_error_1:
	return FALSE;
#else
	pthread_condattr_t condattr;
	if(pthread_condattr_init(&condattr) != 0)
		error_goto(signal_init_error_1);

	if(pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC) != 0)
		error_goto(signal_init_error_2);

	if(pthread_cond_init(cond, &condattr) != 0)
		error_goto(signal_init_error_2);

	if(pthread_condattr_destroy(&condattr) != 0)
		error_goto(signal_init_error_3);
	return TRUE;

signal_init_error_3:
	pthread_cond_destroy(cond);
signal_init_error_2:
	pthread_condattr_destroy(&condattr);
signal_init_error_1:
	return FALSE;
#endif
}

int signal_event(signal *cond)
{
#ifdef _WIN32
	if(SetEvent(*cond) == 0) //使用PulseEvent会出现阻塞
		test_goto(signal_event_error_1);
	return TRUE;
#else
	if(pthread_cond_broadcast(cond) != 0);
		test_goto(signal_event_error_1);
	return TRUE;
#endif

signal_event_error_1:
	return FALSE;
}

int signal_wait(signal *cond, mutex *lock, int tm)
{
#ifdef _WIN32
	if(tm <= 0)
		WaitForSingleObject(*cond, INFINITE);
	else
		if(WaitForSingleObject(*cond, tm * 1000) == WAIT_TIMEOUT)
			test_goto(signal_wait_error_1);
	ResetEvent(*cond);
#else
	if(tm <= 0)
		pthread_cond_wait(cond, lock);
	else
	{
		struct timespec timeout;
		clock_gettime(CLOCK_MONOTONIC, &timeout);

		timeout.tv_sec += tm;
		if(pthread_cond_timedwait(cond, lock, &timeout))
			test_goto(signal_wait_error_1);
	}
#endif
	return TRUE;

signal_wait_error_1:
	return FALSE;
}

int signal_destroy(signal *cond)
{
#ifdef _WIN32
	if(CloseHandle(*cond) == 0)
		error_goto(signal_destroy_error_1);
	return TRUE;
#else
	if(pthread_cond_destroy(cond) != 0)
		error_goto(signal_destroy_error_1);
	return TRUE;
#endif

signal_destroy_error_1:
	return FALSE;
}


#ifdef TEST

#include "test.h"

void signal_test()
{
	signal cond;

	test_assert(signal_init(&cond), TRUE, 'd', TEST_EQUAL);
	test_assert(signal_destroy(&cond), TRUE, 'd', TEST_EQUAL);
}

#endif
