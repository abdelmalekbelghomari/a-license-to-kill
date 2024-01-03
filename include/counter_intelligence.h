#ifndef COUNTER_INTELLIGENCE_H
#define COUNTER_INTELLIGENCE_H

#include "memory.h"

void init_counter_intelligence_officer();
state_t *monitor(counter_intelligence_officer_t *officer);
state_t *go_to_suspect_place(counter_intelligence_officer_t *officer);
state_t *hide(counter_intelligence_officer_t *officer);
state_t *go_back_to_monitor(counter_intelligence_officer_t *officer);
state_t *wait_for_spy_to_steal(counter_intelligence_officer_t *officer);
state_t *follow_spy(counter_intelligence_officer_t *officer);
state_t *wait_for_spy_to_send_message(counter_intelligence_officer_t *officer);
state_t *search_for_mailbox(counter_intelligence_officer_t *officer);
state_t *recover_message(counter_intelligence_officer_t *officer);
state_t *go_to_search_for_mailbox(counter_intelligence_officer_t *officer);
state_t *new_state(int id, state_t *(*action)(counter_intelligence_officer_t *officer));


#endif // COUNTER_INTELLIGENCE_H
