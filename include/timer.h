#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/mman.h>
#include "memory.h"

#define STEP 1000000


simulated_clock_t new_timer();
void update_timer(memory_t *memory);
void tick_clock(int sig, memory_t *memory);
void access_memory(memory_t *memory);


#endif // TIMER_H


/*
void *timer_thread_function(void *arg) {

    int shared_memory_fd = *(int *)arg;
    ftruncate(shared_memory_fd, sizeof(memory_t));
    memory_t *shared_memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory == (void *)-1) {
        handle_fatal_error("Error mapping shared memory using mmap!\n");
    }


    shared_memory_data = (memory_t *)shared_memory;

    struct itimerval it;
    if(STEP >= 1000000) {
        it.it_interval.tv_sec = STEP/1000000;
        it.it_value.tv_sec = STEP/1000000;
    }
    else {
        it.it_interval.tv_usec = STEP;
        it.it_value.tv_usec = STEP;
    }

    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    setitimer(ITIMER_REAL, &it, NULL);


    while (1) {


    }

    return NULL;
} */