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
    srand(time(NULL));
    memory_t *memory = create_shared_memory("SharedMemory");
    //initialiser le semaphore du timer 
    sem_t* sem = create_semaphore("/sem", 1);
    start_simulation_processes();
    
    sem_close(sem);
    sem_unlink("/sem");
    shm_unlink("SharedMemory");
    return 0;

}
