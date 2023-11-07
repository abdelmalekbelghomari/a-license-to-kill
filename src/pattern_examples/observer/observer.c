/*
 * An Observer design pattern.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger.h"
#include "observer.h"

observer_t * new_observer(pid_t pid, memory_t *memory, event_t e)
{
    observer_t *observer;

    observer = (observer_t *) malloc(sizeof(observer_t));
    observer->pid = pid;
    observer->event = e;

    observer->update = observer_update;
    memory->attach(memory, observer);

    return observer;
}

void observer_update(observer_t *obs, event_t e)
{
    if (obs->event == e) {
        log_info("Observer %d has received the update for event %d", obs->pid, e);
    }
}
