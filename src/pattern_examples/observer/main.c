/*
 * An Observer design pattern.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "memory.h"
#include "observer.h"
#include "logger.h"

int main(void)
{
    memory_t *memory = new_memory();

    observer_t *subprocess1 = new_observer(getpid(), memory, EVENT_1);
    observer_t *subprocess2 = new_observer(getpid()+1, memory, EVENT_2);
    observer_t *subprocess3 = new_observer(getpid()+2, memory, EVENT_3);

    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_2);
    memory->notify(memory, EVENT_3);
    memory->notify(memory, EVENT_2);

    log_info("Only subprocesses 2 and 3 will be notified!");
    memory->detach(memory, subprocess1);
    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_2);
    memory->notify(memory, EVENT_3);
    memory->notify(memory, EVENT_2);

    log_info("No more observers to be notified!");
    memory->detach(memory, subprocess2);
    memory->detach(memory, subprocess3);
    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_1);
    memory->notify(memory, EVENT_2);
    memory->notify(memory, EVENT_3);
    memory->notify(memory, EVENT_2);

    exit(EXIT_SUCCESS);
}
