#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "character.h"

int main(void)
{
    srand(time(NULL));

    state_t *resting_at_home = new_state(1, rest_at_home);
    state_t *going_to_company = new_state(2, go_to_company);
    state_t *working = new_state(3, work);
    state_t *going_to_supermarket = new_state(4, go_to_supermarket);
    state_t *doing_some_shopping = new_state(5, do_some_shopping);
    state_t *going_back_home = new_state(6, go_back_home);

    character_t *c = new_character(resting_at_home,
                               going_to_company,
                               working,
                               going_back_home,
                               going_to_supermarket,
                               doing_some_shopping);

    c->begin(c);
    for (int i = 0; i < 6; i++)
        c->step(c);
    c->end(c);

    return 0;
}