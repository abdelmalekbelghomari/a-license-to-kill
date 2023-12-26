#include "timer.h"
#define SHARED_MEMORY "/SharedMemory"

extern memory_t *memory;
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
    memory->memory_has_changed = 0;
    memory->timer.round++;
    if(memory->timer.round == MAX_ROUNDS){
        memory->simulation_has_ended = 1;
    }
    memory->timer.minutes += 10;
    if (memory->timer.minutes >= 60){
        memory->timer.hours++;
        memory->timer.minutes = 0;
    }
    if (memory->timer.hours >= 24){
        memory->timer.days++;
        memory->timer.hours = 0;
    }
    memory->memory_has_changed = 1;
}


void tick_clock(int sig){
    if(sig == SIGALRM){
        pthread_mutex_lock(&mutex);
        update_timer(memory);
        printf("Round: %d\n", memory->timer.round);
        printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        pthread_mutex_unlock(&mutex);
        alarm(1);

    }
    
}


