#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

memory_t * new_memory(int (*get_int_value)(memory_t *, int))
{
    memory_t *memory;

    /* Should be a call to mmap() rather than malloc() / It is just for testing convenience. */
    memory = (memory_t *) malloc(sizeof(memory_t));
    memory->fields[0] = 5;
    memory->fields[1] = 7;
    memory->fields[2] = 3;
    memory->get_int_value = memory_get_int_value;

    return memory;
}

int memory_get_int_value(memory_t *memory, int index)
{
    return memory->fields[index];
}
