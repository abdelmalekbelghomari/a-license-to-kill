#ifndef CITIZEN_MANAGER_H
#define CITIZEN_MANAGER_H
#include "memory.h"

void initialize_synchronization_tools();
void manage_citizens(citizen_t *characters_list);
void *citizen_behavior(void *arg, memory_t *memory);
void start_citizen_threads(citizen_t *characters_list);
void handle_normal_citizen_actions(citizen_t *character, double currentTime);
void handle_citizen_shopping_and_return_home(citizen_t *character);
void move_citizen_to_home(citizen_t *character);
void move_citizen_to_work(citizen_t *character);
void move_citizen_to_supermarket(citizen_t *character);
void state_change_state(citizen_t *c, state_t *s);
state_t *new_state(int id, state_t *(*action)(citizen_t *));
state_t *rest_at_home(citizen_t *c);
state_t *go_to_company(citizen_t *c);
state_t *work(citizen_t *c);
state_t *go_to_supermarket(citizen_t *c);
state_t *go_back_home(citizen_t *c);
state_t *do_some_shopping(citizen_t *c);
double get_current_simulation_time(memory_t *memory);
int is_at_supermarket(citizen_t *character);
memory_t open_shared_memory();

// void character_begin(character_t *c);
// void character_step(character_t *c);
// void character_end(character_t *c);
// void character_change_state(character_t *c, state_t *new_state);

#endif // CITIZEN_MANAGER_H