#include "counter_intelligence.h"
#include <stdlib.h>
#include <stdio.h>

extern memory_t *memory;


void assign_officer_time(counter_intelligence_officer_t *officer){


    // First message retrieval time
    officer->leaving_time.leaving_hour = rand() % (17 - 8) + 8;
    officer->leaving_time.leaving_minute = (rand() % 6) * 10;

}




state_t *new_state(int id, state_t *(*action)(counter_intelligence_officer_t *)) {
    state_t *state = malloc(sizeof(state_t));
    state->id = id;
    state->action = action;
    return state;
}

state_t *monitor(counter_intelligence_officer_t *officer) {
    // printf("je dors devant les cameras\n");
    if (memory->timer.hours <= 19 && memory->timer.hours >= 8){
        memory->surveillanceNetwork.cameras.infrared_camera = 1;
    } else {
        memory->surveillanceNetwork.cameras.standard_camera = 1;
    }
    if (((officer->leaving_time.leaving_hour == memory->timer.hours && officer->leaving_time.leaving_minute == memory->timer.minutes) && officer->new_day) && officer->has_found_mailbox_location){
        // printf("===============================il est temps que je retente de chercher la boite aux lettres\n");
        officer->new_day = false; // avoid searching twice the same day
        return officer->going_to_search_for_mailbox;
    }
    if (memory->surveillanceNetwork.surveillanceAI.suspicious_movement){
        // ================= ABDEL N'IGNORE PAS CES COMMENTAIRES J'AI FAIT CA POUR TOI ===========================
        // go to the location of the suspect
        // here's how to get the location of the suspect :
        // switch (memory->surveillanceNetwork.surveillanceAI.suspect_type) {
        //     case SUSPECT_CITIZEN:
        //         // Accéder à la position du citizen suspect
        //         int suspectX = memory->surveillanceNetwork.surveillanceAI.suspect.citizen->position[0];
        //         int suspectY = memory->surveillanceNetwork.surveillanceAI.suspect.citizen->position[1];
        //         break;
        //     case SUSPECT_SPY:
        //         // Accéder à la position du spy suspect
        //         suspectX = memory->surveillanceNetwork.surveillanceAI.suspect.spy->location_row;
        //         suspectY = memory->surveillanceNetwork.surveillanceAI.suspect.spy->location_column;
        //         break;
        //     case SUSPECT_CASE_OFFICER:
        //         // Accéder à la position du case officer suspect
        //         suspectX = memory->surveillanceNetwork.surveillanceAI.suspect.case_officer->location_row;
        //         suspectY = memory->surveillanceNetwork.surveillanceAI.suspect.case_officer->location_column;
        //         break;
        //     default:
        // break;
    // }
        memory->surveillanceNetwork.cameras.infrared_camera = 0;
        memory->surveillanceNetwork.cameras.standard_camera = 0;
        return officer->going_to_suspect_place; 
    }

    return officer->monitoring;  // Prochain état
}
state_t *go_to_search_for_mailbox(counter_intelligence_officer_t *officer){
    //logique pour aller a la mailbox
    return officer->searching_for_mailbox;
}

state_t *go_to_suspect_place(counter_intelligence_officer_t *officer) {
    // printf("the impostor is sus\n");
    if ((officer->leaving_time.leaving_hour == memory->timer.hours && officer->leaving_time.leaving_minute == memory->timer.minutes) && officer->new_day){
        // printf("===============================il est temps que je retente de chercher la boite aux lettres\n");
        officer->new_day = false; // avoid searching twice the same day
        return officer->going_to_search_for_mailbox;
    }
    // Logique de l'état "go_to_suspect_place"
    return officer->hiding;  // Prochain état
}

state_t *hide(counter_intelligence_officer_t *officer) {
    // printf("je regarde l'imposteur dans un coin : ");
    // remplacer cette logique par le fait que l'espion rentre dans le batiment
    int value = rand()%2;
    if (value == 1){
        // printf("au final il est pas si suspect que ca\n");
        return officer->going_back_to_monitor;
    }
    // printf("c'est un suspect !!! il faut que je le suit apres qu'il fini de voler\n");
    return officer->waiting_for_spy_to_steal;  // Prochain état
}

state_t *go_back_to_monitor(counter_intelligence_officer_t *officer) {
    // printf("je retourne dormir devant les caméras\n");
    if ((officer->leaving_time.leaving_hour == memory->timer.hours && officer->leaving_time.leaving_minute == memory->timer.minutes) && officer->new_day){
        // printf("===============================il est temps que je retente de chercher la boite aux lettres\n");
        officer->new_day = false; // avoid searching twice the same day
        return officer->going_to_search_for_mailbox;
    }
    // il faut implémenter le astar ici
    return officer->monitoring;  // Prochain état
}

state_t *wait_for_spy_to_steal(counter_intelligence_officer_t *officer) {
    // printf("j'attends que l'imposteur arrete de chipper\n");
    // Logique de l'état "wait_for_spy_to_steal"
    return officer->following_spy;  // Prochain état
}

state_t *follow_spy(counter_intelligence_officer_t *officer) {
    // printf("je suit le chippeur jusqu'à la boite aux lettres\n");
    // Logique de l'état "follow_spy"
    return officer->waiting_for_spy_to_send_message;  // Prochain état
}

state_t *wait_for_spy_to_send_message(counter_intelligence_officer_t *officer) {
    // printf("j'attends que l'imposteur envoie des messages\n");
    // Logique de l'état "wait_for_spy_to_send_message"
    officer->has_found_mailbox_location = true;
    return officer->searching_for_mailbox;  // Prochain état
}

state_t *search_for_mailbox(counter_intelligence_officer_t *officer) {
    // printf("je cherche la boite aux lettres : ");
    
    // Logique de l'état "search_for_mailbox"
    int value = rand()%10;
    if (value < 7){
        // printf("j'ai réussi à la trouver !\n");
        officer->has_found_mailbox = true;
        return officer->recovering_messages;
    }
    assign_officer_time(officer);
    // printf("j'ai pas réussi , je reviendrais demain à  %d:%d\n", officer->leaving_time.leaving_hour , officer->leaving_time.leaving_minute);
    return officer->going_back_to_monitor;  
}

state_t *recover_message(counter_intelligence_officer_t *officer) {
    // printf("je prends les messages dans la boite aux lettres\n");
    // Logique de l'état "recover_message"
    return officer->going_back_to_monitor;  
}


void detect_suspicious_person(memory_t *memory) {
    for (int i = 0; i < NUM_CITIZENS; i++) {
        if (is_movement_suspicious(&memory->citizens[i].movement)) {
            memory->surveillanceNetwork.surveillanceAI.suspicious_movement = true;
            memory->surveillanceNetwork.surveillanceAI.suspect.citizen = &memory->citizens[i];
            memory->surveillanceNetwork.surveillanceAI.suspect_type = SUSPECT_CITIZEN;
            return;
        }
    }

    for (int i = 0; i < SPIES_COUNT; i++) {
        if (is_movement_suspicious(&memory->spies[i].movement)) {
            memory->surveillanceNetwork.surveillanceAI.suspicious_movement = true;
            memory->surveillanceNetwork.surveillanceAI.suspect.spy = &memory->spies[i];
            memory->surveillanceNetwork.surveillanceAI.suspect_type = SUSPECT_SPY;
            return;
        }
    }

    if (is_movement_suspicious(&memory->case_officer.movement)) {
        memory->surveillanceNetwork.surveillanceAI.suspicious_movement = true;
        memory->surveillanceNetwork.surveillanceAI.suspect.case_officer = &memory->case_officer;
        memory->surveillanceNetwork.surveillanceAI.suspect_type = SUSPECT_CASE_OFFICER;
    }
}

bool is_movement_suspicious(characterMovement *movement) {
    const int MOVEMENT_THRESHOLD = 3;

    if (movement->currentRow != movement->previousRow || movement->currentColumn != movement->previousColumn) {
        if (abs(movement->currentRow - movement->previousRow) <= MOVEMENT_THRESHOLD &&
            abs(movement->currentColumn - movement->previousColumn) <= MOVEMENT_THRESHOLD) {
            return true;
        }
    }
    return false;
}

void init_counter_intelligence_officer(memory_t * memory){

    counter_intelligence_officer_t *officer = &memory->counter_intelligence_officer;
    officer->monitoring = new_state(0, monitor);
    officer->going_to_suspect_place = new_state(1, go_to_suspect_place);
    officer->hiding = new_state(2, hide);
    officer->going_back_to_monitor = new_state(3, go_back_to_monitor);
    officer->waiting_for_spy_to_steal = new_state(4, wait_for_spy_to_steal);
    officer->following_spy = new_state(5, follow_spy);
    officer->waiting_for_spy_to_send_message = new_state(6, wait_for_spy_to_send_message);
    officer->searching_for_mailbox = new_state(6, search_for_mailbox);
    officer->recovering_messages = new_state(6, recover_message);
    officer->going_to_search_for_mailbox = new_state(7, go_to_search_for_mailbox);

    officer->current_state = officer->monitoring;

    officer->has_found_mailbox = false;
    officer->has_found_mailbox_location = false;
    officer->leaving_time.leaving_hour = -1;
    officer->leaving_time.leaving_minute = -1;
}
