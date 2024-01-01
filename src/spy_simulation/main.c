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

extern WINDOW *main_window;
extern int old_cursor;

int main(int argc, char **argv)
{
    sem_unlink("/semProducer");
    sem_unlink("/semConsumer");
    shm_unlink("SharedMemory");


    srand(time(NULL));
    memory_t *memory = create_shared_memory("SharedMemory");
    //initialiser le semaphore du timer
    sem_t* semp_producer = sem_open("/semProducer", O_CREAT , 0644 , 0);
    if(semp_producer == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* sem_consumer = sem_open("/semConsumer", O_CREAT, 0644, 1);
    if(sem_consumer == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    start_simulation_processes();
    
    sem_close(semp_producer);
    sem_close(sem_consumer);
    sem_unlink("/sem");
    shm_unlink("SharedMemory");
    return 0;

}
