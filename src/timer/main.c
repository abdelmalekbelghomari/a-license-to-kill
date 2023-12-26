#include "timer.h"

#define SHARED_MEMORY "/spy_simulation"

memory_t *memory;  // Pointer to the shared memory

int main()
{

    access_memory();

    time_t timer = new_timer();
    memory->timer = timer;

    struct sigaction sa_clock;
    sa_clock.sa_handler = &tick_clock;

    while(1) {

        if(timer.round < MAX_ROUNDS){
            sigaction(SIGALRM, &sa_clock, NULL);
            //fin de la partie on arrête le timer
        }else{
            if(memory){
                memory->end_of_simulation = 1;
                kill(memory->pids[0],SIGUSR2);
                kill(memory->pids[1],SIGUSR2);
                //terminer les autres processus
                /*
                for(int i=0;i<7;i++){
                    kill(memory->pids[i],SIGUSR2);
                }
                */
                munmap(memory, sizeof(memory_t*));
                exit(0);
            } else{
                perror("Error when accessing memory");
                exit(EXIT_FAILURE);
        }
        }
        pause();
    }

    return 0;
}