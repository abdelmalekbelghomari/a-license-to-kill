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
    /*TEMPORAIRE*/
    
    memory_t *memory = create_shared_memory("SharedMemory");
    //initialiser le semaphore du timer 
    sem_t* sem = sem_open("/timer_sem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    start_simulation_processes();
    
    sem_close(sem);
    sem_unlink("/timer_sem");

}
