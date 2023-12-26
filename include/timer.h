#ifndef TIMER_H
#define TIMER_H
#define STEP 1000000

#include "memory.h"
#include <pthread.h>


simulated_clock_t new_timer(memory_t *memory);
void update_timer(memory_t *memory);
void tick_clock(int sig, memory_t *memory);
void access_memory();


#endif // TIMER_H