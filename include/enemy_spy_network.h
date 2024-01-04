#ifndef ENEMY_SPY_NETWORK_H
#define ENEMY_SPY_NETWORK_H

#include "memory.h"
#include "monitor.h"
#include "enemy_spy_network.h"

#define SPY_COUNT 3


typedef struct {
    int start_hour;
    int end_hour;
    float probability;
} TimeRange;

void init_spies();
state_t *rest_at_home(spy_t *spy);
state_t *go_to_spot(spy_t *spy);
state_t *spot(spy_t *spy);
state_t *steal(spy_t *spy);
state_t *go_to_send_fake_message(spy_t *spy);
state_t *send_message(spy_t *spy);
state_t *arrived_at_mailbox(spy_t *spy);
state_t *going_back_home(spy_t *spy);
state_t *go_to_send_message(spy_t *spy);
state_t *go_to_supermarket(spy_t *spy);
state_t *do_some_shopping(spy_t *spy);
state_t *wait_for_residence_to_be_clear(spy_t *spy);
state_t *scout(spy_t *spy);
state_t *is_hurt(spy_t *spy);
state_t *is_free(spy_t *spy);
state_t *riposte(spy_t *spy);
state_t *is_in_conflict(spy_t *spy);
state_t *dying(spy_t *spy);
state_t *finished(spy_t *spy);
state_t *do_something(spy_t *spy);
state_t *new_state_spy(int id, state_t *(*action)(spy_t *));
void assign_leaving_time(spy_t *spy);
void assign_home_to_spy(memory_t * memory,  spy_t *spy);

void init_officer();
state_t *rest_at_home_officer(case_officer_t *offcier);
state_t *send_messages(case_officer_t *officer);
state_t *go_back_home_officer(case_officer_t *officer);
state_t *go_to_supermarket_officer(case_officer_t *officer);
state_t *do_some_shopping_officer(case_officer_t *officer);
state_t *go_to_mailbox(case_officer_t *officer);
state_t *recover_messages(case_officer_t *officer);
state_t *new_state_officer(int id, state_t *(*action_officer)(case_officer_t *));
void assign_officer_times(case_officer_t *officer);
bool less_than_two_hours (leaving_time_t time1 , leaving_time_t time2);
void caesar_cipher(char *message);


// int spy_choice(spy_t spy);
// int already_stolen(spy_t spy);
// int someone_stolen(int row, int column);
// int possible_theft(spy_t spy, int t);
// void check_position(memory_t* memory, int choice);
// void spy_move_around_company(spy_t* spy, cell_t pos);
// int consider_theft(spy_t* spy, int* pos);
// void theft(spy_t* spy);
// void move_spy(spy_t* spy);
// spyInfo* get_info_spy(int is_killer, int id);
// caseOfficerInfo* get_info_case_officer();
// counterIntelligenceOfficer* get_info_counter_intelligence_officer();
// void fill_company_spy(memory_t* memory, int n);
// void go_to_mail_box(memory_t* memory, int choice);
// int* is_around(int row, int column, int cell_type);
// void case_officer_go_shopping(case_officer_t* case_officer);
// void case_officer_go_home(case_officer_t* case_officer);
// void* spy_life(void* thread); 


#endif /* ENEMY_SPY_NETWORK_H */


