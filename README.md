# Lock.h
This is a simple C header wrapper for using mutex locks and condition variables
using the pthreads library.

## Usage
### `LOCK`
The `LOCK` macro is used to acquire a mutex lock and execute some code while the
lock is held. It takes two arguments:

- A pointer to a `pthread_mutex_t` object. 
- The code to execute while the lock is held.

#### Example:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared_resource = 0;

LOCK(&mutex, {
    // Do something with shared_resource while the mutex is held
    shared_resource++;
});
```

### `WAIT`
The `WAIT` macro is used to wait on a condition variable while holding a mutex
lock. It takes three arguments:

- A pointer to a `pthread_mutex_t object`.
- A pointer to a `pthread_cond_t object`.
- The condition to wait for (expressed as a Boolean expression).
#### Example:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int shared_resource = 0;

// Wait for shared_resource to become non-zero while holding the mutex
WAIT(&mutex, &cond, shared_resource == 0);
```

### LOCK_SIGNAL
The `LOCK_SIGNAL` macro is used to acquire a mutex lock, execute some code, and
signal a condition variable before releasing the lock. It takes three arguments:

- A pointer to a `pthread_mutex_t` object.
- A pointer to a `pthread_cond_t` object.
- The code to execute while the lock is held.

#### Example:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int shared_resource = 0;

LOCK_SIGNAL(&mutex, &cond, {
    // Do something with shared_resource while the mutex is held
    shared_resource++;
});
```

### `LOCK_BROADCAST`
The `LOCK_BROADCAST` macro is similar to `LOCK_SIGNAL`, but it signals all
threads waiting on the condition variable instead of just one. It takes the same
three arguments as `LOCK_SIGNAL`.

#### Example:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int shared_resource = 0;

LOCK_BROADCAST(&mutex, &cond, {
    // Do something with shared_resource while the mutex is held
    shared_resource++;
});
```

#### Example
Here is an example of how to use lock.h to implement a simple producer-consumer
program:

```c
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
```

In this example, there is a shared resource (`resource`) that is accessed by
both a producer thread and a consumer thread. The producer thread produces
values by setting the value of `resource` to a positive integer, while the
consumer thread consumes values by printing them to the console.

Access to the shared resource is synchronized using the `LOCK` and `WAIT`
macros, which acquire a mutex lock and wait on a condition variable. The
`LOCK_SIGNAL` macro is used to signal the condition variable and release the
mutex lock after updating the shared resource. The `WAIT` macro is used to wait
for the shared resource to be available before consuming it.

By using lock.h in this way, the program ensures that only one thread at a time
can access the shared resource, and that the producer and consumer threads can
communicate and coordinate their access to the resource.

## Gotchas
While the `LOCK`, `WAIT`, `LOCK_SIGNAL`, and `LOCK_BROADCAST` macros provided by
lock.h can simplify the implementation of synchronization primitives in C
programs, there are several things that can go wrong if they are not used
correctly.

Here are some potential issues to be aware of when using these macros:

### 1. Deadlocks
Deadlocks can occur if threads hold multiple mutexes and wait on each other to
release them. To avoid deadlocks, make sure to acquire and release mutexes in a
consistent order.

### 2. Race Conditions
Race conditions can occur if multiple threads attempt to access a shared
resource without proper synchronization. To avoid race conditions, make sure to
use the `LOCK`, `WAIT`, `LOCK_SIGNAL`, and `LOCK_BROADCAST` macros appropriately
to ensure that only one thread at a time can access the shared resource.

### 3. Missed Signals
Missed signals can occur if a thread signals a condition variable before another
thread begins waiting on it. To avoid missed signals, make sure to signal the
condition variable only after the mutex lock has been acquired and the waiting
thread is ready to receive the signal.

### 4. Spurious Wakeups
Spurious wakeups can occur if a thread waiting on a condition variable is woken
up without the condition being met. To avoid spurious wakeups, always use a
Boolean condition expression with the `WAIT` macro to check whether the desired
condition has been met.

### 5. Undefined Behavior
The `LOCK`, `WAIT`, `LOCK_SIGNAL`, and `LOCK_BROADCAST` macros rely on specific
behaviors of the pthreads library, and may produce undefined behavior if used
incorrectly. Make sure to consult the pthreads documentation and use these
macros correctly to avoid undefined behavior.

In general, it is important to carefully analyze the potential risks and ensure that the code is correct and safe when using synchronization primitives in C programs.
