/*
 * A Facade design pattern.
 */
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "facade.h"
#include "logger.h"

int main(void)
{
    memory_t *memory;
    facade_t *facade;

    log_info("Create a new shared memory.");
    memory = new_memory(memory_get_int_value);
    log_info("Create a new facade in front of the shared memory.");
    facade = new_facade(memory);
    log_info("Do some requests to the facade...");
    facade->get_and_display(facade, 0);
    facade->get_and_display(facade, 1);
    facade->get_and_display(facade, 2);

    exit(EXIT_SUCCESS);
}
