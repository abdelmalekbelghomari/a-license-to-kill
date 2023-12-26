#ifndef ENEMY_SPY_NETWORK_H
#define ENEMY_SPY_NETWORK_H

#include "memory.h"
#include "monitor.h"
#include "enemy_spy_network.h"


int spy_choice(spy_t spy);

/* Return 1 if the spy already stolen this company */
int already_stolen(spy_t spy);

/* Return 1 if someone already stolen this company */
int someone_stolen(int row, int column);

/* Return 1 if the spy steal the company */
int possible_theft(spy_t spy, int t);

void check_position(memory_t* memory, int choice);

void spy_move_around_company(spy_t* spy, cell_t pos);

/* Return 1 if the spy wants to steal the company */
int consider_theft(spy_t* spy, int* pos);

void theft(spy_t* spy);

void move_spy(spy_t* spy);

spyInfo* get_info_spy(int is_killer, int id);

caseOfficerInfo* get_info_case_officer();

counterIntelligenceOfficer* get_info_counter_intelligence_officer();

void fill_company_spy(memory_t* memory, int n);

void go_to_mail_box(memory_t* memory, int choice);

int* is_around(int row, int column, int cell_type);

void case_officer_go_shopping(case_officer_t* case_officer);

void case_officer_go_home(case_officer_t* case_officer);

void* spy_life(void* thread);
  
void* case_officer_life(void* thread);

#endif /* ENEMY_SPY_NETWORK_H */


