#ifndef CHARACTER_H
#define CHARACTER_H

#define MAX_STEPS 2016

typedef struct state_s state_t;
typedef struct character_s character_t;

struct state_s {
    int id;

    void (*change_state)(character_t *, state_t *);
    state_t *(*action)(character_t *);
};

struct character_s {
    state_t *current_state;
    state_t *next_state;
    state_t *resting_at_home;
    state_t *going_to_company;
    state_t *working;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *going_back_home;
    state_t *dying;

    void (*change_state)(character_t *, state_t *);
    void (*begin)(character_t *);
    void (*end)(character_t *);
    void (*step)(character_t *);
};

state_t *new_state(int id, state_t *(*action)(character_t *));
state_t *rest_at_home(character_t *c);
state_t *go_to_company(character_t *c);
state_t *work(character_t *c);
state_t *go_to_supermarket(character_t *c);
state_t *go_back_home(character_t *c);
state_t *do_some_shopping(character_t *c);

void state_change_state(character_t *c, state_t *s);

character_t *new_character(state_t *resting_at_home,
                           state_t *going_to_company,
                           state_t *working,
                           state_t *going_back_home,
                           state_t *going_to_supermarket,
                           state_t *doing_some_shopping);
void character_begin(character_t *c);
void character_step(character_t *c);
void character_end(character_t *c);
void character_change_state(character_t *c, state_t *new_state);

#endif /* CHARACTER_H */
