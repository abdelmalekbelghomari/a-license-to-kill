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
    sem_t* sem = create_semaphore("/timer_sem", 1);
    start_simulation_processes();
    memory->walking_citizens = 0;
    memory->at_home_citizens = 127;
    memory->at_work_citizens = 0;
    sem_close(sem);
    sem_unlink("/timer_sem");
    shm_unlink("SharedMemory");
    return 0;

}
