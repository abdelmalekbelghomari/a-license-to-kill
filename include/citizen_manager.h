#ifndef CITIZEN_MANAGER_H
#define CITIZEN_MANAGER_H
#include "memory.h"
#define MAX_STEPS 2016
#define NUM_CITIZENS 127

typedef struct state_s state_t;
typedef struct citizen_s citizen_t;
typedef struct building_s building_t;
typedef struct home_s home_t;

struct state_s {
    int id;

    void (*change_state)(citizen_t *, state_t *);
    state_t *(*action)(citizen_t *);
};

typedef enum citizen_type_e {
    NORMAL,
    SPY,
    CASE_OFFICER,
    COUNTER_INTELLIGENCE_OFFICER
} citizen_type_t;

typedef enum building_type_e {
    CORPORATION,
    STORE,
    HALL
}building_type_t;

struct building_s {
    unsigned int position[2];
    building_type_t type;
    unsigned int nb_citizen;
    unsigned int max_capacity;
    citizen_t *citizens[NUM_CITIZENS];
    void (*add_citizen)(building_t *, citizen_t *);
    void (*remove_citizen)(building_t *, citizen_t *);
};

struct home_s {
    unsigned int position[2];
    unsigned int nb_citizen;
    unsigned int max_capacity;
    citizen_t *citizens[NUM_CITIZENS];
    void (*add_citizen)(home_t *, citizen_t *);
    void (*remove_citizen)(home_t *, citizen_t *);
};

struct citizen_s {
    unsigned int id;
    unsigned int position [2];
    unsigned int health ;
    citizen_type_t type;
    building_t *workplace, *supermarket; /*the nearest supermarket from his company*/
    home_t *home; 

    state_t *current_state;
    state_t *next_state;
    state_t *resting_at_home;
    state_t *going_to_company;
    state_t *working;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *going_back_home;
    state_t *dying;

    void (*change_state)(citizen_t *, state_t *);
    void (*begin)(citizen_t *);
    void (*end)(citizen_t *);
    void (*step)(citizen_t *);
};

state_t *new_state(int id, state_t *(*action)(citizen_t *));
state_t *rest_at_home(citizen_t *c);
state_t *go_to_company(citizen_t *c);
state_t *work(citizen_t *c);
state_t *go_to_supermarket(citizen_t *c);
state_t *go_back_home(citizen_t *c);
state_t *do_some_shopping(citizen_t *c);

void state_change_state(citizen_t *c, state_t *s);

citizen_t *new_citizen(state_t *resting_at_home,
                           state_t *going_to_company,
                           state_t *working,
                           state_t *going_back_home,
                           state_t *going_to_supermarket,
                           state_t *doing_some_shopping);
void citizen_begin(citizen_t *c);
void citizen_step(citizen_t *c);
void citizen_end(citizen_t *c);
void citizen_change_state(citizen_t *c, state_t *new_state);

#endif // CITIZEN_MANAGER_H
