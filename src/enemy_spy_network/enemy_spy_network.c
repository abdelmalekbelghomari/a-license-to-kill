#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include "enemy_spy_network.h"
#include "astar.h"
extern memory_t *memory;
extern mqd_t mq;
extern sem_t *sem_producer_timer, *sem_consumer_timer;

void handle_fatal_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

DIRECTION[NUM_DIRECTIONS][2] = {
    {-1, 0}, // UP
    {1, 0}, // DOWN
    {0, -1}, // LEFT
    {0, 1}, // RIGHT
    {-1, -1}, // UP_LEFT
    {-1, 1}, // UP_RIGHT
    {1, -1}, // DOWN_LEFT
    {1, 1} // DOWN_RIGHT
};

void assign_leaving_time(spy_t *spy) {
    TimeRange ranges[] = {
        {17, 20, 0.02},
        {20, 24, 0.10},
        {0, 3, 0.76},
        {3, 5, 0.10},
        {5, 8, 0.02}
    };

    float rand_prob = (float)rand() / RAND_MAX;
    float cumulative_prob = 0.0;

    int hour, minute;
    for (int i = 0; i < sizeof(ranges)/sizeof(ranges[0]); i++) {
        cumulative_prob += ranges[i].probability;
        if (rand_prob <= cumulative_prob) {
            hour = ranges[i].start_hour + rand() % (ranges[i].end_hour - ranges[i].start_hour);
            int minutes[] = {0, 10, 20, 30, 40, 50};
            minute = minutes[rand() % 6];
            // printf("Assigned Time for Spy: %d:%d\n", hour, minute); 
            break;
        }
    }

    spy->leaving_time.leaving_hour = hour;
    spy->leaving_time.leaving_minute = minute;
}

void assign_officer_times(case_officer_t *officer) {

    leaving_time_t first_leaving_time , second_leaving_time , shopping_time , messaging_time;

    // First message retrieval time
    first_leaving_time.leaving_hour = rand() % (17 - 8) + 8;
    first_leaving_time.leaving_minute = (rand() % 6) * 10;
    // printf("Assigned First Time for Officer: %d:%d\n", first_leaving_time.leaving_hour, first_leaving_time.leaving_minute);

    // Second message retrieval time, at least 2 hours apart
    do {
        second_leaving_time.leaving_hour = rand() % (17 - 8) + 8;
        second_leaving_time.leaving_minute = (rand() % 6) * 10;
    } while(less_than_two_hours(second_leaving_time , first_leaving_time));
    // printf("Assigned Second Time for Officer: %d:%d\n", second_leaving_time.leaving_hour, second_leaving_time.leaving_minute);
    

    // Shopping time
    do{
        shopping_time.leaving_hour = rand() % (19 - 17) + 17;
        shopping_time.leaving_minute = (rand() % 6) * 10;
    } while(less_than_two_hours(shopping_time , second_leaving_time));
    // printf("Assigned Shopping Time for Officer: %d:%d\n", shopping_time.leaving_hour, shopping_time.leaving_minute);

    // messaging time
    messaging_time.leaving_hour = rand() % 2 + 22;
    messaging_time.leaving_minute = (rand() % 6) * 10;

    officer->first_leaving_time = first_leaving_time;
    officer->second_leaving_time = second_leaving_time;
    officer->shopping_time = shopping_time;
    officer->messaging_time = messaging_time;
    
}

bool less_than_two_hours (leaving_time_t time1 , leaving_time_t time2){
    int time1_in_minutes = time1.leaving_hour * 60 + time1.leaving_minute;
    int time2_in_minutes = time2.leaving_hour * 60 + time2.leaving_minute;

    int difference = abs(time1_in_minutes - time2_in_minutes);

    return difference < 120;
}



state_t *new_state_spy(int id, state_t *(*action)(spy_t *)) {
    state_t *state = malloc(sizeof(state_t));
    state->id = id;
    state->action = action;
    return state;
}

state_t *do_something(spy_t *spy){
  int value = rand()%10;
  if (value == 0 && (memory->timer.hours < 19 && memory->timer.hours > 8)){
    int random_supermarket = rand()%2;
    spy->x_supermarket = memory->companies[random_supermarket].position[1];
    spy->y_supermarket = memory->companies[random_supermarket].position[0];
    return spy->going_to_supermarket;
  } else if (value < 7 && (memory->timer.hours < 19 && memory->timer.hours > 8)){
    // printf(" espion : %d  : je vais aller scout \n",spy->id);
    return spy->scouting;
  }else {
    return spy->resting_at_home;
  }
}

state_t *rest_at_home(spy_t *spy) {
    // printf(" espion : %d  : je me repose chez oim : heure : %d  minute : %d heure de sortie : %d   minute de sortie : %d\n"
    // ,spy->id , memory->timer.hours ,memory->timer.minutes, spy->leaving_time.leaving_hour, spy->leaving_time.leaving_minute);
    spy->location_row = spy->home_row;
    spy->location_column = spy->home_column;
     if( spy->has_a_fake_message || spy->has_a_message){
        if(spy->has_a_message){
            return spy->going_to_send_message;
        }else {
            return spy->going_to_send_message;
        }
    }
    if (spy->leaving_time.leaving_hour == memory->timer.hours && spy->leaving_time.leaving_minute == memory->timer.minutes){
        return spy->going_to_spot;
    }
    if (memory->timer.minutes == 0){
        return spy->is_free;
    }
    return spy->resting_at_home;
}
int is_in_front_of_targeted_company(spy_t *spy){
    if(spy->location_row == spy->x_in_front_of_targeted_company && spy->location_column == spy->y_in_front_of_targeted_company){
        return 1;
    } else {
        return 0;
    }
}

state_t *go_to_spot(spy_t *spy) {
    // printf("Espion : %d : je vais aller repérer\n", spy->id);

    // Vérifier si l'espion est déjà à la position cible
    if (is_in_front_of_targeted_company(spy)) {
        // printf("Turns spent spotting: %d by spy number %d : Nous sommes dans la première condition\n", spy->turns_spent_spotting, spy->id);
        // spy->turns_spent_spotting++;  // Incrémenter le compteur de tours passés à espionner
        return spy->spotting;  // Retourner l'état d'espionnage
    }

    // Calculer le prochain pas
    Node *position_node = calculate_next_step(spy->location_row, spy->location_column, 
                                              spy->y_in_front_of_targeted_company, spy->x_in_front_of_targeted_company, &memory->map);

    // Vérifier si un nouveau nœud a été obtenu
    if (position_node == NULL) {
        return spy->spotting;
    } else {
        spy->location_column = position_node->position[1];
        spy->location_row = position_node->position[0];
        free(position_node);
    }

    // Vérifier à nouveau si l'espion est arrivé à la position cible
    if (is_in_front_of_targeted_company(spy)) {
        // printf("Espion %d est arrivé à la position cible\n", spy->id);
        spy->turns_spent_spotting = 0;  // Réinitialiser le compteur de tours passés à espionner
        return spy->spotting;  // Retourner l'état d'espionnage
    } else {
        // printf("Espion %d n'est pas arrivé à la position cible\n", spy->id);
        return spy->going_to_spot;  // Retourner l'état de déplacement
    }

    // return NULL;  // Retourner NULL si l'espion n'est pas encore arrivé
}


state_t *spot(spy_t *spy) {
    // printf(" espion : %d  : je repère \n",spy->id);
    if (spy->turns_spent_spotting == 12){
        spy->turns_spent_spotting = 0;
        int value = rand() % 100;
        if (value < 85){
            return spy->stealing;
        }
        // printf("l'agent n'a pas pu accéder au batiment il va envoyer un faux message\n");
        spy->has_a_fake_message = true;
        return spy->going_to_send_message; 
    } else {
        if(spy->random_neighbour == NULL){
            spy->random_neighbour = get_random_neighbours(&memory->map, 
                spy->x_in_front_of_targeted_company, spy->y_in_front_of_targeted_company);
        }
        Node* position_node = calculate_next_step(spy->location_row, spy->location_column, 
                spy->random_neighbour->position[0], spy->random_neighbour->position[1], &memory->map);
        if (position_node != NULL) {
            // printf("coucou");
            spy->location_row = position_node->position[0];
            spy->location_column = position_node->position[1];
            free(position_node);
        }
        else if (spy->location_row == spy->random_neighbour->position[0] &&
                spy->location_column == spy->random_neighbour ->position[1]){
            // printf("Going to send message to (%d,%d)\n", spy->random_neighbour->position[0], spy->random_neighbour[1]);
            free(spy->random_neighbour);
        }
        spy->turns_spent_spotting++;
        return spy->spotting;
            
    }
}

state_t *steal(spy_t *spy) {
    // printf(" espion : %d  : je vole \n",spy->id);
    if (spy->turns_spent_stealing == 6){
        spy->turns_spent_stealing = 0;
        int value = rand() % 100;
        if (value < 90){
            // printf("l'agent a réussi , il va envoyer un vrai message\n");
            spy->has_a_message = true;
            spy->nb_of_stolen_companies++;
            if(!(memory->timer.hours >= 8 && memory->timer.hours <= 17)){
                // printf("===================== il est trop tard je vais envoyer le message demain\n");
                return spy->going_back_home;
            }
            return spy->going_to_send_message;     
        }
        //ici le message est faux
        // printf("l'agent n'a pas réussi à voler, il va envoyer un faux message\n");
        spy->has_a_fake_message = true;
        if(!(memory->timer.hours >= 8 && memory->timer.hours <= 17)){
            // printf("===================== il est trop tard je vais envoyer le message demain\n");
            return spy->going_back_home;
        }
        return spy->going_to_send_message;
    }
    spy->turns_spent_stealing++;
    return spy->stealing;
}


state_t *arrived_at_mailbox(spy_t *spy){
    //astar vers une case adjacente a la mailbox
    // printf(" espion : %d  : j'arrive a cote de la mailbox \n",spy->id);
    if(memory->homes->mailbox.is_occupied){
        return spy->waiting_for_residence_to_be_clear;
    }
    memory->homes->mailbox.is_occupied = true;

    return spy->sending_message;
}

int is_at_home(spy_t *spy){
    if(spy->location_row == spy->home_row && spy->location_column == spy->home_column){
        return 1;
    } else {
        return 0;
    }
}

state_t *go_back_home(spy_t *spy) {
    // il faut implémenter astar
    // spy->turns_spent_scouting = 0;
    // printf(" espion : %d  : je rentre chez oim \n",spy->id);
    // // printf("Position Spy n°%d: (%d, %d) -> ", spy->id, spy->location_row, spy->location_column);
    // Node* position_node = calculate_next_step(spy->location_row, spy->location_column, spy->home_row, spy->home_column, &memory->map);
    // if(is_at_home(spy)){
    //     printf(" espion : %d  : Enfin à la maison \n",spy->id);
    //     return spy->resting_at_home;
    // } else {
    //     printf(" espion : %d  (%d, %d) : je vais à la maison (%d, %d) \n",spy->id, spy->location_row, spy->location_column, spy->home_row, spy->home_column);
    //     return spy->going_back_home;
    // }

    if (is_at_home(spy)) {
        return spy->resting_at_home;
    }

    // Calculez le prochain pas pour aller à la maison
    Node *position_node = calculate_next_step(spy->location_row, spy->location_column, spy->home_row, spy->home_column, &memory->map);

    if (position_node == NULL) {
        return spy->resting_at_home;
    } else {
        // printf(" espion : %d  : je vais à la maison (%d, %d) \n",spy->id, spy->home_row, spy->home_column);
        spy->location_column = position_node->position[1];
        spy->location_row = position_node->position[0];
        free(position_node);
    }
    // Vérifiez à nouveau si le spy est à la maison après le déplacement
    if (is_at_home(spy)) {
        return spy->resting_at_home;
    } else {
        return spy->going_back_home;
    }
}

int is_at_mailbox(spy_t *spy){
    if(spy->location_row == memory->homes->mailbox.x_in_front && spy->location_column == memory->homes->mailbox.y_in_front){
        return 1;
    } else {
        return 0;
    }
}

int is_at_supermarket(spy_t *spy){
    if(spy->location_row == memory->companies[0].position[0] && spy->location_column == memory->companies[0].position[1]){
        return 1;
    } else if (spy->location_row == memory->companies[1].position[0] && spy->location_column == memory->companies[1].position[1]){
        return 1;
    } else {
        return 0;
    }
}

state_t *go_to_send_message(spy_t *spy) {  
    // printf(" espion : %d  : je vais pour envoyer un message \n", spy->id);
    
    if (is_at_mailbox(spy)) {
        return spy->sending_message;
    } else {
        Node* position_node = calculate_next_step(spy->location_row, spy->location_column, 
                memory->homes->mailbox.y_in_front, memory->homes->mailbox.x_in_front, &memory->map);

        if (position_node != NULL) {
            spy->location_row = position_node->position[0];
            spy->location_column = position_node->position[1];
            free(position_node);
        }
    }
    
    if (is_at_mailbox(spy)) {
        return spy->sending_message;
    }

    return spy->going_to_send_message;
}


state_t *send_message(spy_t *spy){
    // printf(" espion : %d : je mets le message dans la boite aux lettres",spy->id);
    memory->homes->mailbox.is_occupied = false;
    // printf(" the spy is about to send a fake message? : %d , or a real message? : %d",spy->has_a_fake_message ,spy->has_a_message);
    // printf(" espion numéro %d va envoyer son message le jour %d  a %d:%d\n",spy->id , memory->timer.days, memory->timer.hours , memory->timer.minutes);
    if(spy->has_a_fake_message){
        // ici il faut implémneter une logique selon le nombre de travailleurs dans une entreprise
        char message[MAX_MESSAGE_SIZE]; 
        strcpy(message, "Deceptive");
        caesar_cipher(message);
        strcpy(memory->homes->mailbox.messages[memory->homes->mailbox.message_count] ,message);
        // printf(" \n\n============================= la boite aux lettres contient le message suivant odnt la priorité est %d : %s\n\n" ,memory->homes->mailbox.priority[memory->homes->mailbox.message_count],memory->homes->mailbox.messages[memory->homes->mailbox.message_count]);
        memory->homes->mailbox.message_count++;
        memory->homes->mailbox.priority[memory->homes->mailbox.message_count] = get_crypted_message_priority(message);
        spy->has_a_fake_message = false;
    }else if(spy->has_a_message){
         char message[MAX_MESSAGE_SIZE]; 
        int randValue = rand() % 100; // Generate a random number between 0 and 99

        if(randValue < 1) { 
            strcpy(message, "Crucial");
        } else if(randValue < 6) {
            strcpy(message, "Strong");
        } else if(randValue < 20) { 
            strcpy(message, "Medium");
        } else if(randValue < 50) { 
            strcpy(message, "Low");
        } else { 
            strcpy(message, "VeryLow");
        }
        caesar_cipher(message);
        memory->homes->mailbox.priority[memory->homes->mailbox.message_count] = get_crypted_message_priority(message);
        // printf("\n\n===================== la priorité du message: %d \n", memory->homes->mailbox.priority[memory->homes->mailbox.message_count]);
        strcpy(memory->homes->mailbox.messages[memory->homes->mailbox.message_count] ,message);
        // printf(" \n\n============================= la boite aux lettres contient le message suivant : %s\n\n" ,memory->homes->mailbox.messages[memory->homes->mailbox.message_count]);
        memory->homes->mailbox.message_count++;
        spy->has_a_message = false;
    }
    memory->memory_has_changed = 1;
    return spy->going_back_home;
}


state_t *wait_for_residence_to_be_clear(spy_t *spy) {
    // Attendre que la résidence soit libre
    // return spy->going_to_send_message;
    // printf(" espion : %d  : j'attends que la résidence soit vide \n",spy->id);
    if(spy->turns_spent_waiting >= 6){
        spy->turns_spent_waiting=0;
        return spy->sending_message;
    }
    spy->turns_spent_waiting++;
    return spy->waiting_for_residence_to_be_clear;
}

state_t *scout(spy_t *spy){
    // printf(" espion : %d  : je cherche une entreprise cible \n",spy->id);
    // printf("Time : (%dh%d), Position Spy n°%d: (%d, %d)", memory->timer.hours ,memory->timer.minutes,spy->id, spy->location_row, spy->location_column);

    // Déplacement aléatoire sur WASTELAND
    Node* random_neighbor = get_random_neighbours_spy(&memory->map, spy);

    if (random_neighbor != NULL) {
        // Déplacer l'espion vers le voisin aléatoire
        spy->location_row = random_neighbor->position[0];
        spy->location_column = random_neighbor->position[1];
        free(random_neighbor);
        for(int i = 0; i < 8; i++){
            // printf("%d\n", i);
            int x = spy->location_row + DIRECTION[i][0];
            int y = spy->location_column + DIRECTION[i][1];
            // Si une entreprise est trouvée à côté
            if ( x >= 0 && x < MAX_ROWS && y >= 0 && y < MAX_COLUMNS){
                // printf("coucou1\n");
                // sem_wait(sem_producer_timer);
                if (memory->map.cells[x][y].type == COMPANY) {
                    // printf("coucou2\n");
                    if (rand() % 10 == 0) {  // 10% de chance de choisir l'entreprise
                        // printf("coucou3\n");
                        // printf("Position Company : (%d, %d)\n", x, y);
                        spy->targeted_company->position[0] = x;
                        spy->targeted_company->position[1] = y;
                        spy->x_in_front_of_targeted_company = spy->location_column;
                        spy->y_in_front_of_targeted_company = spy->location_row;
                        // printf(" espion : %d  : je vais aller à l'entreprise : (%d, %d) \n",spy->id, spy->targeted_company->position[0], spy->targeted_company->position[1]);
                        return spy->going_back_home;
                    }
                    
                }
                // sem_post(sem_producer_timer);
            }
            
        }
        

    // } else if (spy->turns_spent_scouting >= 12) {
    //     return spy->going_back_home;
    // }

    // Parcourir les 8 directions possibles
    // for(int i = 0; i < 8; i++){
    //     int x = spy->location_row + DIRECTION[i][0];
    //     int y = spy->location_column + DIRECTION[i][1];
    //     // Si une entreprise est trouvée à côté
    //     if (memory->map.cells[x][y].type == COMPANY) {
    //         if (rand() % 2 == 0) {  // 50% de chance de choisir l'entreprise
    //             spy->targeted_company->position[0] = x;
    //             spy->targeted_company->position[1] = y;
    //             return spy->going_back_home;
    //         }
    //     }
    // }
    }
    // spy->turns_spent_scouting++;
    return spy->scouting;
}


state_t *go_to_supermarket(spy_t *spy) {
    // Aller au supermarché
    // return spy->doing_some_shopping;
    spy->turns_spent_shopping = 0;
    if(is_at_supermarket(spy)){
        return spy->doing_some_shopping;
    } else {
        Node* position_node = calculate_next_step(spy->location_row, spy->location_column, 
                spy->x_supermarket, spy->y_supermarket, &memory->map);

        if (position_node != NULL) {
            spy->location_row = position_node->position[0];
            spy->location_column = position_node->position[1];
            free(position_node);
        }
        // printf("Going to supermarket (%d,%d)", spy->x_supermarket, spy->y_supermarket);
        return spy->going_to_supermarket;
    }

    if(is_at_supermarket(spy)){
        return spy->doing_some_shopping;
    }

}

state_t *do_some_shopping(spy_t *spy) {
    // Faire des courses
    // return spy->resting_at_home;
    // printf(" espion : %d  : je fais du shoopinje \n",spy->id);
    if(spy->turns_spent_shopping == 6){
        spy->turns_spent_shopping = 0;
        return spy->going_back_home;
    }
    spy->turns_spent_shopping++;
    return spy->doing_some_shopping;
}

state_t *is_hurt(spy_t *spy) {
    // L'espion est blessé
    return (spy->health_point <= 0) ? spy->dying : spy->current_state;
}

state_t *riposte(spy_t *spy) {
    // Riposter si attaqué
    return spy->is_in_conflict;
}

state_t *is_in_conflict(spy_t *spy) {
    // En conflit avec l'officier du contre-espionnage
    return spy->dying; // Ou fuir si possible
}

state_t *dying(spy_t *spy) {
    // L'espion meurt
    return spy->finished;
}

state_t *finished(spy_t *spy) {
  return spy->finished;
}





state_t *new_state_officer(int id, state_t *(*action)(case_officer_t *)) {
    state_t *state = malloc(sizeof(state_t));
    state->id = id;
    state->action = action;
    return state;
}

state_t *rest_at_home_officer(case_officer_t *officer){
    // printf(" officier traitant : je me repose chez oim \n");
    if(officer->first_leaving_time.leaving_hour == memory->timer.hours && officer->first_leaving_time.leaving_minute == memory->timer.minutes){
        return officer->going_to_mailbox;
    } else if (officer->second_leaving_time.leaving_hour == memory->timer.hours && officer->second_leaving_time.leaving_minute == memory->timer.minutes){
        return officer->going_to_mailbox;
    }else if (officer->shopping_time.leaving_hour == memory->timer.hours && officer->shopping_time.leaving_minute == memory->timer.minutes){
        int random_supermarket = rand()%2;
        officer->x_supermarket = memory->companies[random_supermarket].position[1];
        officer->y_supermarket = memory->companies[random_supermarket].position[0];
        return officer->going_to_supermarket;
    }else if (officer->messaging_time.leaving_hour == memory->timer.hours && officer->messaging_time.leaving_minute == memory->timer.minutes){
        return officer->sending_messages;
    }
    return officer->resting_at_home;
}

state_t *send_messages(case_officer_t *officer){
    // printf(" officier traitant : j'evoie les messages à l'autre pays \n");
    // printf("=================== j'envoie ces messages à l'autre pays :  \n");
    for (int i=0; i < officer->message_count; i++){
        // printf("=================> %s\n", officer->messages[i]);
    }
    // printf("===========================================\n");
    if (officer->message_count != 0){
        send_messages_to_enemy_country(officer);
    }
    for (int i=0 ; i < officer->message_count; i++){
        memset(memory->case_officer.messages[i], 0, sizeof(memory->case_officer.messages[i]));
    }
    officer->message_count = 0;
    return officer->resting_at_home;
}

int is_at_home_officer(case_officer_t *officer){
    if(officer->location_row == officer->home_row && officer->location_column == officer->home_column){
        return 1;
    } else {
        return 0;
    }
}

state_t *go_back_home_officer(case_officer_t *officer){
    if (is_at_home(officer)){
        return officer->resting_at_home;
    } else {
        Node* position_node = calculate_next_step(officer->location_row, officer->location_column, 
                officer->home_row, officer->home_column, &memory->map);

        if (position_node != NULL) {
            officer->location_row = position_node->position[0];
            officer->location_column = position_node->position[1];
            free(position_node);
            return officer->going_back_home;
        } else {
            return officer->resting_at_home;
        }
    }
}

bool is_at_supermarket_officer(case_officer_t *officer){
    if(officer->location_row == officer->y_supermarket && officer->location_column == officer->x_supermarket){
        return true;
    } else {
        return false;
    }
}


state_t *go_to_supermarket_officer(case_officer_t *officer){
    // printf(" officier traitant : je vais au supermarché \n");
    if(is_at_supermarket_officer(officer)){
        return officer->doing_some_shopping;
    } else {
        Node* position_node = calculate_next_step(officer->location_row, officer->location_column, 
                officer->y_supermarket, officer->x_supermarket, &memory->map);

        if (position_node != NULL) {
            officer->location_row = position_node->position[0];
            officer->location_column = position_node->position[1];
            free(position_node);
            return officer->going_to_supermarket;
        } else {
            return officer->doing_some_shopping;
        }
    }
}

state_t *do_some_shopping_officer(case_officer_t *officer){
    // printf(" officier traitant : je fais du shoppinje \n");
    if(officer->turns_spent_shopping >= 6){
        officer->turns_spent_shopping = 0;
        return officer->going_to_mailbox;
    } else {
        officer->turns_spent_shopping++;
        return officer->doing_some_shopping;
    }
}

state_t *go_to_mailbox(case_officer_t *officer){
    // printf(" officier traitant : je vais a la boite aux lettres \n");
    if(is_at_mailbox(officer)){
        return officer->recovering_messages;
    } else {
        Node* position_node = calculate_next_step(officer->location_row, officer->location_column, 
                officer->mailbox_row, officer->mailbox_column, &memory->map);

        if (position_node != NULL) {
            officer->location_row = position_node->position[0];
            officer->location_column = position_node->position[1];
            free(position_node);
            return officer->going_to_mailbox;
        } else {
            return officer->recovering_messages;
        }
    }
}

state_t *recover_messages(case_officer_t *officer){
    // printf(" officier traitant : je récupère les messages \n");
    for (int i=0 ; i < memory->homes->mailbox.message_count ; i++){
        // printf("\n================================= contenu de la boite aux lettres ================\n");
        // printf(" ===============> %s\n", memory->homes->mailbox.messages[i]);
        strcpy(officer->messages[i], memory->homes->mailbox.messages[i]);
        // printf("j'ai récupéré le message  : %s", officer->messages[i]);
        memset(memory->homes->mailbox.messages[i], 0, sizeof(memory->homes->mailbox.messages[i]));
        officer->message_count++;
        // printf("\n================================= la boite aux lettres a été vidée ================\n");
    }
    memory->homes->mailbox.message_count = 0;
    return officer->going_back_home;
}

void caesar_cipher(char *message) {
    for (int i = 0; message[i] != '\0'; ++i) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch += SHIFT;
            if (ch > 'z') ch -= 26;
            message[i] = ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch += SHIFT;
            if (ch > 'Z') ch -= 26;
            message[i] = ch;
        }
    }
}

void caesar_decipher(char *message) {
    for (int i = 0; message[i] != '\0'; ++i) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - SHIFT;
            if (ch < 'a') ch += 26;
            message[i] = ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch = ch - SHIFT;
            if (ch < 'A') ch += 26;
            message[i] = ch;
        }
    } 
}

unsigned int get_message_priority(const char* message) {
    if (strcmp(message, "Deceptive") == 0) {
        return 1;
    } else if (strcmp(message, "VeryLow") == 0) {
        return 2;
    } else if (strcmp(message, "Low") == 0) {
        return 3;
    } else if (strcmp(message, "Medium") == 0) {
        return 6;
    } else if (strcmp(message, "Strong") == 0) {
        return 9;
    } else if (strcmp(message, "Crucial") == 0) {
        return 10;
    }
    return 0; // Valeur par défaut si aucune correspondance
}

unsigned int get_crypted_message_priority(const char* message) {
    if (strcmp(message, "Ghfhswlyh") == 0) { // Deceptive
        return 1;
    } else if (strcmp(message, "YhubOrz") == 0) { // VeryLow
        return 2;
    } else if (strcmp(message, "Orz") == 0) { //Low
        return 3;
    } else if (strcmp(message, "Phglxp") == 0) { //Medium
        return 6;
    } else if (strcmp(message, "Vwurqj") == 0) { //Strong
        return 9;
    } else if (strcmp(message, "Fuxfldo") == 0) { //Crucial
        return 10;
    }
    return 0; // Valeur par défaut si aucune correspondance
}

void send_messages_to_enemy_country(case_officer_t *officer) {
    for (int i = 0; i < officer->message_count; i++) {
        // printf("\nCiphered message: %s\n", officer->messages[i]);
        // printf("Deciphered message: %s\n", deciphered_message);
        unsigned int priority = get_crypted_message_priority(officer->messages[i]);
        // printf("Message priority: %u\n", priority);

        if (mq_send(mq, officer->messages[i], strlen(officer->messages[i]) + 1, priority) == -1) {
            perror("mq_send");
            fflush(stdout); // Force output buffer to flushw_state
        }
    }
}



void init_spies(memory_t * memory){
    
    sem_wait(sem_producer_timer);
    for (int i = 0; i < SPY_COUNT; i++) {
        spy_t *spy = &memory->spies[i];
        spy->resting_at_home = new_state_spy(0, rest_at_home);
        spy->going_to_spot = new_state_spy(1, go_to_spot);
        spy->spotting = new_state_spy(2, spot);
        spy->stealing = new_state_spy(3, steal);
        spy->going_back_home = new_state_spy(4, go_back_home);
        spy->going_to_send_message = new_state_spy(5, go_to_send_message);
        spy->sending_message = new_state_spy(6, send_message);
        spy->waiting_for_residence_to_be_clear = new_state_spy(7, wait_for_residence_to_be_clear);
        spy->going_to_supermarket = new_state_spy(8, go_to_supermarket);
        spy->doing_some_shopping = new_state_spy(9, do_some_shopping);
        spy->is_hurt = new_state_spy(10, is_hurt);
        spy->riposte = new_state_spy(11, riposte);
        spy->is_in_conflict = new_state_spy(12, is_in_conflict);
        spy->dying = new_state_spy(13, dying);
        spy->finished = new_state_spy(14, finished);
        spy->scouting = new_state_spy(15, scout);
        spy->arriving_at_mailbox = new_state_spy(16, arrived_at_mailbox);
        spy->is_free = new_state_spy(17, do_something);

        spy->current_state = spy->resting_at_home;

        spy->turns_spent_spotting = 0;
        spy->turns_spent_stealing = 0;
        spy->turns_spent_waiting = 0;
        spy->turns_spent_shopping = 0;
        spy->has_a_message = false;
        spy->has_a_fake_message = 0;
        spy->id = i;
        spy->nb_of_stolen_companies = 0;
        
        spy->health_point = 10;
        spy->random_neighbour = NULL;
        if(spy->id == 1){
            spy->has_license_to_kill = 1;
        }

        int random = rand() % NB_HOMES;

        assign_home_to_spy(memory, spy);

        // printf("Position Maison du Spy n°%d: (%d, %d)\n", spy->id, spy->home_row, spy->home_column);
        spy->location_row = spy->home_row;
        spy->location_column = spy->home_column;

        spy->x_in_front_of_targeted_company = 0;
        spy->y_in_front_of_targeted_company = 0;

        spy->targeted_company = malloc(sizeof(building_t));
        if (spy->targeted_company == NULL) {
            free(spy->targeted_company);
            return NULL;
        }

        // spy -> location_row = spy->home_row;
        // spy -> location_column = spy->home_column;

    }
    sem_post(sem_consumer_timer);
}


void assign_home_to_spy(memory_t* memory, spy_t* s){

    home_t *houses = memory->homes;
    // Assign a random house, respecting max capacity
    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            s->home_row = houses[house_index].position[0];
            s->home_column = houses[house_index].position[1];
            houses[house_index].nb_citizen++;
            break;  // Sortie de la boucle une fois qu'une maison est trouvée
        }
        attempts++;
    }

    // for(int i = 0; i < NB_HOMES; i++){
    //     if(houses[i].nb_citizen < houses[i].max_capacity){
    //         s->home_row = houses[i].position[0];
    //         s->home_column = houses[i].position[1];
    //         houses[i].nb_citizen++;
    //         break;
    //     }
    // }

    // int random = rand() % NB_HOMES;
    // s->home_row = houses[random].position[0];
    
}

void init_officer(memory_t * memory){

    case_officer_t *officer = &memory->case_officer;
    officer->resting_at_home = new_state_officer(0, rest_at_home_officer);
    officer->going_back_home = new_state_officer(1, go_back_home_officer);
    officer->sending_messages = new_state_officer(2, send_messages);
    officer->going_to_supermarket = new_state_officer(3, go_to_supermarket_officer);
    officer->doing_some_shopping = new_state_officer(4, do_some_shopping_officer);
    officer->going_to_mailbox = new_state_officer(5, go_to_mailbox);
    officer->recovering_messages = new_state_officer(6, recover_messages);

    officer->current_state = officer->resting_at_home;
    officer->message_count = 0;
    officer->turns_spent_shopping = 0;

    assign_home_to_officer(memory, officer);
    // Since as the starting state is *resting_at_home*, the officer is already at home
    officer->location_row = officer->home_row;
    officer->location_column = officer->home_column;

    for (int i = 0; i< NB_HOMES; i++){
        if(memory->homes[i].has_mailbox == true){
            officer->mailbox_row = memory->homes[i].mailbox.y;
            officer->mailbox_column = memory->homes[i].mailbox.x;
            break;
        }
    }

}

void assign_home_to_officer(memory_t* memory, case_officer_t* c){

    home_t *houses = memory->homes;
    // Assign a random house, respecting max capacity
    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            c->home_row = houses[house_index].position[0];
            c->home_column = houses[house_index].position[1];
            houses[house_index].nb_citizen++;
            break;  // Sortie de la boucle une fois qu'une maison est trouvée
        }
        attempts++;
    }
    
}


