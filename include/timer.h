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
#include <semaphore.h>

#include "memory.h"

#define TIME_STEP 200000

simulated_clock_t new_timer();
void update_timer(memory_t *memory);
void tick_clock(int sig);
void set_timer(void);


#endif // TIMER_H
