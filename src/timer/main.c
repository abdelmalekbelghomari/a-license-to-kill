#include "timer.h"

#define SHARED_MEMORY "/spy_simulation"

memory_t *memory;  // Pointer to the shared memory

int main()
{

    access_memory(memory);

    simulated_clock_t timer = new_timer(memory);
    memory->timer = timer;

    struct itimerval it;
    if(STEP >= 1000000) {
        it.it_interval.tv_sec = STEP/1000000;
        it.it_value.tv_sec = STEP/1000000;
    }
    else {
        it.it_interval.tv_usec = STEP;
        it.it_value.tv_usec = STEP;
    }
    
    struct sigaction sa_clock;
    sa_clock.sa_handler = &tick_clock;
    sigaction(SIGALRM, &sa_clock, NULL);
    setitimer(ITIMER_REAL, &it, NULL);

    while(1) {
    }

    return 0;
}