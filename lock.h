#ifndef LOCK_H
#define LOCK_H

#include <pthread.h>

#define LOCK(MUTEX, CODE)      \
    pthread_mutex_lock(MUTEX); \
    CODE;                      \
    pthread_mutex_unlock(MUTEX);

#define WAIT(MUTEX, COND, CODE) \
    while (CODE) { \
    pthread_cond_wait(COND, MUTEX); \
    }

#define LOCK_SIGNAL(MUTEX, COND, CODE) \
    LOCK(MUTEX, CODE);  \
    pthread_cond_signal(COND);

#define LOCK_BROADCAST(MUTEX, COND, CODE) \
    LOCK(MUTEX, CODE);  \
    pthread_cond_broadcast(COND);

#endif
