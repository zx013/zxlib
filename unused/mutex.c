#include "mutex.h"

int mutex_init(mutex *lock)
{
#ifdef _WIN32
    InitializeCriticalSection(lock);
    return TRUE;
#else
    if(pthread_mutex_init(lock, NULL) != 0)
        error_goto(mutex_init_error_1);
    return TRUE;

mutex_init_error_1:
    return FALSE;
#endif
}

int mutex_destroy(mutex *lock)
{
#ifdef _WIN32
    DeleteCriticalSection(lock);
    return TRUE;
#else
    if(pthread_mutex_destroy(lock) != 0)
        error_goto(mutex_destroy_error_1);
    return TRUE;

mutex_destroy_error_1:
    return FALSE;
#endif
}

int mutex_lock(mutex *lock)
{
#ifdef _WIN32
    EnterCriticalSection(lock);
    return TRUE;
#else
    if(pthread_mutex_lock(lock) != 0)
        error_goto(mutex_lock_error_1);
    return TRUE;

mutex_lock_error_1:
    return FALSE;
#endif
}

int mutex_trylock(mutex *lock)
{
#ifdef _WIN32
    if(TryEnterCriticalSection(lock) == 0)
        test_goto(mutex_trylock_error_1);
    return TRUE;
#else
    if(pthread_mutex_trylock(lock) != 0)
        test_goto(mutex_trylock_error_1);
    return TRUE;
#endif

mutex_trylock_error_1:
    return FALSE;
}

int mutex_unlock(mutex *lock)
{
#ifdef _WIN32
    LeaveCriticalSection(lock);
    return TRUE;
#else
    if(pthread_mutex_unlock(lock) != 0)
        error_goto(mutex_unlock_error_1);
    return TRUE;

mutex_unlock_error_1:
    return FALSE;
#endif
}
