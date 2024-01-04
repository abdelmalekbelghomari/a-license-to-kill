#include <stdio.h>
#include "spy_simulation.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "monitor_common.h"
#include "monitor.h"
#include "memory.h"
#include "spy_simulation.h"
#include "timer.h"



int main(int argc, char **argv)
{
    sem_unlink("/semProducer");
    sem_unlink("/semConsumer");
    shm_unlink("SharedMemory");


    srand(time(NULL));
    memory_t *memory = create_shared_memory("SharedMemory");
    //initialiser le semaphore du timer
    sem_t* semp_producer_timer = sem_open("/semTimerProducer", O_CREAT , 0644 , 0);
    if(semp_producer_timer == SEM_FAILED){
        perror("sem_open timer");
        exit(EXIT_FAILURE);
    }
    sem_t* sem_consumer_timer = sem_open("/semTimerConsumer", O_CREAT, 0644, 1);
    if(sem_consumer_timer == SEM_FAILED){
        perror("sem_open timer");
        exit(EXIT_FAILURE);
    }
    start_simulation_processes();
    
    sem_close(semp_producer_timer);
    sem_close(sem_consumer_timer);
    sem_unlink("/semTimerProducer");
    shm_unlink("SharedMemory");
    return 0;

}
