/*
 * A Facade design pattern.
 */
#include <stdio.h>
#include <stdlib.h>

#include "facade.h"

facade_t *new_facade(memory_t *memory)
{
    facade_t *facade;

    facade = (facade_t *) malloc(sizeof(facade_t));
    facade->memory = memory;
    facade->get_and_display = facade_get_and_display;

    return facade;
}

void facade_get_and_display(facade_t *facade, int index)
{
    int result;

    result = facade->memory->get_int_value(facade->memory, index);
    printf(">> %d\n", result);
}
