#ifndef MEMORY_H
#define MEMORY_H

#include "facade.h"


struct memory_s {
    /*
     * Many fields such as integer values...
     */
    int fields[3];

    int (*get_int_value)(memory_t *memory, int index);
};

memory_t * new_memory(int (*get_int_value)(memory_t *, int));
int memory_get_int_value(memory_t *memory, int index);

#endif
