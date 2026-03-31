#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <unistd.h>


#ifdef __linux__ 

// Use posix semaphore
#include <pthread.h>
#include <semaphore.h>
extern sem_t mutex;

#else

// For baremetal this function must be implemented
extern sem_t mutex;

extern int sem_init (sem_t *__sem, int __pshared, unsigned int __value)
extern int sem_destroy (sem_t *__sem) __THROW __nonnull ((1));
extern int sem_wait (sem_t *__sem) __nonnull ((1));
sem_post(&mutex);
extern int sem_post (sem_t *__sem) __THROWNL __nonnull ((1));


#endif

#endif