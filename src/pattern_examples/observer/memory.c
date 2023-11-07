#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger.h"
#include "observer.h"
#include "memory.h"

memory_t * new_memory(void)
{
    memory_t *memory;

    /* Should be a call to mmap() rather than malloc() / It is just for testing convenience. */
    memory = (memory_t *) malloc(sizeof(memory_t));

    for (int i = 0; i < MAX_OBSERVERS; i++) {
        memory->obs[i] = NULL;
    }

    memory->attach = memory_attach;
    memory->detach = memory_detach;
    memory->notify = memory_notify;

    return memory;
}

void memory_attach(memory_t *memory, observer_t *obs)
{
    for (int i = 0; i < MAX_OBSERVERS; i++) {
        if (memory->obs[i] == NULL) {
            memory->obs[i] = obs;
            log_info("Shared memory attaching observer %d", obs);
            break;
        }
    }
}

void memory_detach(memory_t *memory, observer_t *obs)
{
    for (int i = 0; i < MAX_OBSERVERS; i++) {
        if (memory->obs[i] == obs) {
            log_info("Memory detaching observer %d", obs);
            memory->obs[i] = NULL;
            break;
        }
    }
}

void memory_notify(memory_t *memory, event_t e)
{
    log_info("Memory notifies event %u", (unsigned) e);

    for (int i = 0; i < MAX_OBSERVERS; i++) {
        observer_t *obs = memory->obs[i];

        if (obs) {
            obs->update(obs, e);
        }
    }
}
