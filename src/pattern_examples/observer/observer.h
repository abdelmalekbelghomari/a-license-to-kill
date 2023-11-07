/*
 * An Observer design pattern.
 */

#ifndef OBSERVER_H
#define OBSERVER_H

#include <sys/types.h>
#include <unistd.h>

#include "memory.h"

struct observer_s {
	pid_t pid;
	event_t event;

	void (*update)(observer_t *, event_t e);
};

observer_t * new_observer(pid_t pid, memory_t *memory, event_t e);
void observer_update(observer_t *obs, event_t e);

#endif /* OBSERVER_H */
