#ifndef TIMER_H
#define TIMER_H

#include "memory.h"
#include <pthread.h>


time_t new_timer(memory_t *memory);
void update_timer();
void tick_clock(int sig);
void access_memory();


#endif // TIMER_H