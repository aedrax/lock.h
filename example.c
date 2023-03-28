#include <stdio.h>
#include <stdlib.h>
#include "lock.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int resource = 0;

void *producer(void *arg)
{
    for (int i = 0; i < 5; i++)
    {
        LOCK_SIGNAL(&mutex, &cond, {
            WAIT(&mutex, &cond, resource != 0);
            resource = i + 1;
            printf("Producer: produced %d\n", resource);
        });
    }
    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < 5; i++)
    {
        LOCK_SIGNAL(&mutex, &cond, {
            WAIT(&mutex, &cond, resource == 0);
            printf("Consumer: consumed %d\n", resource);
            resource = 0;
        });
    }
    return NULL;
}

int main()
{
    pthread_t prod_tid, cons_tid;
    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cons_tid, NULL, consumer, NULL);
    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);
    return 0;
}
