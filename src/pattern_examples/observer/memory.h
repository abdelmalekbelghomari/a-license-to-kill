#ifndef MEMORY_OBS_H
#define MEMORY_OBS_H

#define MAX_OBSERVERS 3

typedef enum {
	EVENT_1 = 1,
	EVENT_2,
	EVENT_3,
} event_t;

typedef struct memory_s memory_t;
typedef struct observer_s observer_t;

struct memory_s {
	observer_t *obs[MAX_OBSERVERS];

	void (*attach)(memory_t *, observer_t *);
	void (*detach)(memory_t *, observer_t *);
	void (*notify)(memory_t *, event_t e);
};

memory_t * new_memory(void);
void memory_attach(memory_t *memory, observer_t *obs);
void memory_detach(memory_t *memory, observer_t *obs);
void memory_notify(memory_t *memory, event_t e);

#endif /* MEMORY_OBS_H */
