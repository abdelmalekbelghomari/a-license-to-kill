/*
 * A Factory design pattern.
 */

#include <stdio.h>
#include <stdlib.h>

#include "factory.h"
#include "logger.h"

character_factory_t *new_factory(character_t *(*factory_method)(void))
{
    character_factory_t *factory;

    factory = (character_factory_t *) malloc(sizeof(character_factory_t));
    factory->factory_method = factory_method;

    return factory;
}

character_t *new_citizen(void)
{
    character_t *citizen;

    citizen = (character_t *) malloc(sizeof(character_t));
    citizen->operation = operation_citizen;

    return citizen;
}

character_t *new_spy(void)
{
    character_t *spy;

    spy = (character_t *) malloc(sizeof(character_t));
    spy->operation = operation_spy;

    return spy;
}

character_t *new_case_officer(void)
{
    character_t *case_officer;

    case_officer = (character_t *) malloc(sizeof(character_t));
    case_officer->operation = operation_case_officer;

    return case_officer;
}

character_t *new_counterintelligence_officer(void)
{
    character_t *counterintelligence_officer;

    counterintelligence_officer = (character_t *) malloc(sizeof(character_t));
    counterintelligence_officer->operation = operation_counterintelligence_officer;

    return counterintelligence_officer;
}

void operation_citizen(character_t *citizen)
{
    printf(">> Operation of a citizen...\n");
}

void operation_spy(character_t *spy)
{
    printf(">> Operation of a spy...\n");
}

void operation_case_officer(character_t *case_officer)
{
    printf(">> Operation of a case officer...\n");
}

void operation_counterintelligence_officer(character_t *counterintelligence_officer)
{
    printf(">> Operation of a counterintelligence_officer...\n");
}
