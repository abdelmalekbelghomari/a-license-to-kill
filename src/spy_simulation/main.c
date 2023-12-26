#include <stdio.h>
#include "spy_simulation.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../../include/monitor_common.h"
#include "../../include/monitor.h"

extern WINDOW *main_window;
extern int old_cursor;

int main(int argc, char **argv)
{
    memory_t *memory = create_shared_memory("SharedMemory");
    start_simulation_processes();

}