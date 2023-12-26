#include "timer.h"
#define SHARED_MEMORY "/spy_simulation"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

simulated_clock_t new_timer(){
    simulated_clock_t time;
    time.round = 0;
    time.hours = 0;
    time.minutes = 0;
    time.days = 0;
    return time;
}

void update_timer(memory_t *memory){
    memory->timer.round++;
    memory->timer.minutes += 10;
    if (memory->timer.minutes == 60){
        memory->timer.hours++;
        memory->timer.minutes = 0;
    }
    if (memory->timer.hours == 24){
        memory->timer.days++;
        memory->timer.hours = 0;
    }
}


void tick_clock(int sig, memory_t *memory){
    if(sig == SIGALRM){
        pthread_mutex_lock(&mutex);
        update_timer(memory);
        pthread_mutex_unlock(&mutex);
        alarm(1);
    }
    
}


