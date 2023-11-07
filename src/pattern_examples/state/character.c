#include <stdlib.h>

#include "../logger.h"
#include "character.h"

state_t *new_state(int id, state_t *(*action)(character_t *))
{
    state_t *s = (state_t *) malloc(sizeof(state_t));

    s->id = id;
    s->change_state = state_change_state;
    s->action = action;

    return s;
}

state_t *rest_at_home(character_t *c)
{
    log_info("Resting at home --- [state %d]", c->current_state->id);

    return c->going_to_company;
}

state_t *go_to_company(character_t *c)
{
    log_info("Going to company --- [state %d]", c->current_state->id);

    return c->working;
}

state_t *work(character_t *c)
{
    int choice;

    log_info("Working :-( --- [state %d]", c->current_state->id);

    choice = rand() % 101;
    if (choice < 25) {
        return c->going_to_supermarket;
    }
    return c->going_back_home;
}

state_t *go_to_supermarket(character_t *c)
{
    log_info("Going to supermarket --- [state %d]", c->current_state->id);

    return c->doing_some_shopping;
}

state_t *do_some_shopping(character_t *c)
{
    log_info("Doing some shopping --- [state %d]", c->current_state->id);

    return c->going_back_home;
}

state_t *go_back_home(character_t *c)
{
    log_info("Going back home--- [state %d]", c->current_state->id);

    return c->resting_at_home;
}

void state_change_state(character_t *c, state_t *s)
{
    state_t *old_state = c->current_state;
    state_t *next_state = s->action(c);
    state_t *new_state = s;

    c->current_state = s;
    c->next_state = next_state;

    log_info(">> Changed from %d to %d (next is %d)\n",
             old_state->id,
             new_state->id,
             next_state->id);
}

character_t *new_character(state_t *resting_at_home,
                           state_t *going_to_company,
                           state_t *working,
                           state_t *going_back_home,
                           state_t *going_to_supermarket,
                           state_t *doing_some_shopping)
{
    character_t *c = (character_t *) malloc(sizeof(character_t));

    c->current_state = resting_at_home;
    c->resting_at_home = resting_at_home;
    c->going_to_company = going_to_company;
    c->working = working;
    c->going_back_home = going_back_home;
    c->going_to_supermarket = going_to_supermarket;
    c->doing_some_shopping = doing_some_shopping;

    c->change_state = character_change_state;
    c->begin = character_begin;
    c->step = character_step;
    c->end = character_end;

    return c;
}

void character_begin(character_t *c)
{
    log_info("---- Beginning his day ----");
    c->change_state(c, c->resting_at_home);
}

void character_step(character_t *c)
{
    log_info("----------- Step -----------");
    c->change_state(c, c->next_state);
}

void character_end(character_t *c)
{
    log_info("------ Ending his day ------");
    c->change_state(c, c->resting_at_home);
}

void character_change_state(character_t *c, state_t *new_state)
{
    c->current_state->change_state(c, new_state);
}
