#ifndef TIMER_H
#define TIMER_H
#include "memory.h"
#include <pthread.h>

void set_timer();
void update_timer();
void tick_clock(int sig);
void access_memory();
time_t new_timer();

#endif