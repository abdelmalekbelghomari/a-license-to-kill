/*
 * A Factory design pattern.
 */

#include <stdio.h>
#include <stdlib.h>

#include "factory.h"
#include "logger.h"

int main(void)
{
    character_factory_t *factory;
    character_t *a_citizen;
    character_t *a_spy;
    character_t *a_case_officer;
    character_t *a_counterintelligence_officer;

    log_info("Create a new factory of citizens.");
    factory = new_factory(new_citizen);
    log_info("Create a new citizen.");
    a_citizen = factory->factory_method();
    log_info("Work with this citizen...");
    a_citizen->operation(a_citizen);
    free(factory);
    free(a_citizen);

    log_info("Create a new factory of spies.");
    factory = new_factory(new_spy);
    log_info("Create a new spy.");
    a_spy = factory->factory_method();
    log_info("Work with this spy...");
    a_spy->operation(a_spy);
    free(factory);
    free(a_spy);

    log_info("Create a new factory of case officer.");
    factory = new_factory(new_case_officer);
    log_info("Create a new case officer.");
    a_case_officer = factory->factory_method();
    log_info("Work with this case officer...");
    a_case_officer->operation(a_case_officer);
    free(factory);
    free(a_case_officer);

    log_info("Create a new factory of counterintelligence officers.");
    factory = new_factory(new_counterintelligence_officer);
    log_info("Create a new counterintelligence officer.");
    a_counterintelligence_officer = factory->factory_method();
    log_info("Work with this counterintelligence officer...");
    a_counterintelligence_officer->operation(a_counterintelligence_officer);
    free(factory);
    free(a_counterintelligence_officer);

    exit(EXIT_SUCCESS);
}
