#include <stdio.h>
#include "citizen_manager.h"
#include <pthread.h>
#include <math.h>
#include <bits/pthreadtypes.h>

#define SHARED_MEMORY "/SharedMemory"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t start_barrier, end_barrier;
extern memory_t *memory;

memory_t open_shared_memory() {
    int shmd = shm_open(SHARED_MEMORY, O_RDWR,  S_IRUSR | S_IWUSR);
    if (shmd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    } else {
        memory_t *memory = mmap(NULL, 
                                sizeof(memory_t), 
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED, 
                                shmd, 
                                0);
        if (memory == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        } else {
            return *memory;
        }
    }
}

double distance(unsigned int pos1[2], unsigned int pos2[2]) {
    return abs((int)pos1[0] - (int)pos2[0]) + abs((int)pos1[1] - (int)pos2[1]);
}

void init_house(memory_t *memory){
    int fakeHome = rand() % NB_HOMES;
    // printf("=====================================\n");
    // printf("fake home %d\n", fakeHome);
    // printf("La maison %p est la maison avec la boite au lettre piégee\n", &memory->homes[fakeHome]);
    // printf("\n\n=====================================\n");
    for(int i = 0; i < NB_HOMES; i++){
        if (i == fakeHome){
            memory->homes[i].max_capacity = 0;
        }
        else{
            memory->homes[i].max_capacity = 15;
        }
    }
    int home_counter = 0;
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            cell_t cell = memory->map.cells[j][i];
            if (cell.type == RESIDENTIAL_BUILDING) {
                // Attribuer la position à la maison
                memory->homes[home_counter].position[0] = i;
                memory->homes[home_counter].position[1] = j;  
                memory->homes[home_counter].nb_citizen = 0; // Nombre initial de citoyens
                home_counter++;
            }
        }
    }
}

void init_building(memory_t *memory){
    for(int i = 0; i < NB_WORKPLACES; i++){
        if(i < NB_STORE){
            memory->companies[i].type = STORE;
            memory->companies[i].max_workers = 3;
            memory->companies[i].min_workers = 3;
        } else if (i == NB_STORE) {
            memory->companies[i].min_workers = 10;
            memory->companies[i].max_workers = 10;
            memory->companies[i].type = CITY_HALL;
        } else {
            memory->companies[i].min_workers = 5;
            memory->companies[i].max_workers = 50;
            memory->companies[i].type = CORPORATION;
        }
    }

    int supermarket_counter = 0, city_hall_counter = NB_STORE, company_counter = NB_STORE + NB_HALL; 

    for (int i=0; i < MAX_ROWS; i++){
        for (int j = 0; j < MAX_COLUMNS; j++){
            cell_t cell =memory->map.cells[j][i];
            if(cell.type == SUPERMARKET){
                memory->companies[supermarket_counter].type = cell.type;
                memory->companies[supermarket_counter].position[0] = i;
                memory->companies[supermarket_counter].position[1] = j;
                supermarket_counter++;
            }
            if(cell.type == CITY_HALL){
                memory->companies[city_hall_counter].type = cell.type;
                memory->companies[city_hall_counter].position[0] = i;
                memory->companies[city_hall_counter].position[1] = j;
                city_hall_counter++;
            }
            if(cell.type == COMPANY){
                memory->companies[company_counter].type = cell.type;
                memory->companies[company_counter].position[0] = i;
                memory->companies[company_counter].position[1] = j;
                company_counter++;
            }
        }
    }
}

void init_citizens(memory_t *memory) {

    init_house(memory);
    init_building(memory);

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];
        citizen->id = i;
        citizen->type = NORMAL;
        citizen->health = 10;
        // citizen->current_state = create_state(citizen->id, current_state);
        // citizen->next_state = create_state(citizen->id, next_state);
        // citizen->resting_at_home = create_state(citizen->id, resting_at_home);
        // citizen->going_to_company = create_state(citizen->id, going_to_company);
        // citizen->working = create_state(citizen->id, working);
        // citizen->going_to_supermarket = create_state(citizen->id, going_to_supermarket);
        // citizen->doing_some_shopping = create_state(citizen->id, doing_some_shopping);
        // citizen->going_back_home = create_state(citizen->id, going_back_home);
        // citizen->dying = create_state(citizen->id, dying);
        // citizen->change_state = change_state;
        // citizen->begin = citizen_begin;
        // citizen->end = citizen_end;
        // citizen->step = citizen_step;
        // Initialisation des états
        citizen->resting_at_home = new_state(0, rest_at_home);
        citizen->going_to_company = new_state(1, go_to_company);
        citizen->working = new_state(2, work);
        citizen->going_to_supermarket = new_state(3, go_to_supermarket);
        citizen->doing_some_shopping = new_state(4, do_some_shopping);
        citizen->going_back_home = new_state(5, go_back_home);
        citizen->dying = new_state(6, dying);
        citizen->finished = new_state(7, finished);

        // Initialiser l'état actuel et le prochain état
        citizen->current_state = citizen->resting_at_home;

        assign_home_to_citizen(memory, citizen);
        //printf("maison du citoyen %d est la maison %p\n", i+1, citizen->home);
        assign_company_to_citizen(memory, citizen);
        // printf("entreprise du citoyen %d est l'entreprise %p\n", i+1, 
                                                    //  citizen->workplace);
        assign_random_supermarket(memory, citizen);
        // printf("Le supermarché le plus proche du citoyen %d est %p\n", i+1, citizen->supermarket);
    }
        
}

void assign_home_to_citizen(memory_t* memory, citizen_t* citizen){

    home_t *houses = memory->homes;
    // Assign a random house, respecting max capacity
    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            citizen->home = &houses[house_index];
            houses[house_index].nb_citizen++;
            break;  // Sortie de la boucle une fois qu'une maison est trouvée
        }
        attempts++;
    }
}

void assign_company_to_citizen(memory_t* memory, citizen_t* citizen){
    
    building_t *company_list = memory->companies;
    // Assign a random company, respecting max capacity
    int company_index;
    int attempts = 0;  // Compteur pour éviter une boucle infinie

    if(company_list[0].nb_workers < company_list[0].max_workers){
        citizen->workplace = &company_list[0];
        company_list[0].nb_workers++;

    } else if (company_list[1].nb_workers < company_list[1].max_workers){
        citizen->workplace = &company_list[1];
        company_list[1].nb_workers++;

    } else if (company_list[2].nb_workers < company_list[2].max_workers){
        citizen->workplace = &company_list[2];
        company_list[2].nb_workers++;

    } else {
        while (attempts < NB_COMPANY) {
            company_index = 3 + rand() % NB_COMPANY;
            building_t *company = &company_list[company_index];
            if (company->nb_workers < company->max_workers 
            && company->nb_workers < company->min_workers) {
                citizen->workplace = &company;
                company->nb_workers++;
                break;
            }else{
                attempts++;
            }
        }
    }

//     while (1) {
//         company_index = rand() % NB_WORKPLACES;
//         building_t *company = &company_list[company_index];

//         if (company->type == STORE && company->nb_workers < company->max_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         if (company->type == CITY_HALL && company->nb_workers < company->max_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         } else if (company->nb_workers < company->max_workers ||
//                    company->nb_workers < company->min_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         }
//     }
}

void assign_random_supermarket(memory_t* memory, citizen_t* citizen){
    
    // Find nearest supermarket
    building_t supermaket_list[NB_STORE] = {memory->companies[0], memory->companies[1]};
    // Les deux premiers emplacements sont donnés aux supermarchés
    double dist1 = distance(supermaket_list[0].position, citizen->workplace->position);
    double dist2 = distance(supermaket_list[1].position, citizen->workplace->position);
    int supermaketChoice = rand() % NB_STORE;
    if(supermaketChoice == 0){
        citizen->supermarket = &supermaket_list[0];
    } else {
        citizen->supermarket = &supermaket_list[1];
    }
    
}



// citizen_t *new_character(state_t *resting_at_home,
//                            state_t *going_to_company,
//                            state_t *working,
//                            state_t *going_back_home,
//                            state_t *going_to_supermarket,
//                            state_t *doing_some_shopping)
// {
//     citizen_t *c = (citizen_t *) malloc(sizeof(citizen_t));

//     c->current_state = resting_at_home;
//     c->resting_at_home = resting_at_home;
//     c->going_to_company = going_to_company;
//     c->working = working;
//     c->going_back_home = going_back_home;
//     c->going_to_supermarket = going_to_supermarket;
//     c->doing_some_shopping = doing_some_shopping;
//     c->change_state = character_change_state;
//     c->begin = character_begin;
//     c->step = character_step;
//     c->end = character_end;

//     return c;
// }

double get_current_simulation_time(memory_t *memory) {
    return memory->timer.hours + round(memory->timer.minutes / 60.0 * 100) / 100;
    /*TO DO in timer*/
}

int is_at_supermarket(citizen_t *character) {
    return (character->position == character->supermarket->position);
}

state_t *new_state(int id, state_t *(*action)(citizen_t *)) {
    state_t *state = malloc(sizeof(state_t));
    state->id = id;
    state->action = action;
    return state;
}

state_t *rest_at_home(citizen_t *c) {
    if(get_current_simulation_time(memory) == 8.00){
        return c->current_state;
    }
    return c->resting_at_home;
}

state_t *go_to_company(citizen_t *c) {

    return c->going_to_company;
}

state_t *work(citizen_t *c) {
    
    return c->working;
}

state_t *go_to_supermarket(citizen_t *c) {

    return c->going_to_supermarket;
}

state_t *go_back_home(citizen_t *c) {

    return c->going_back_home;
}

state_t *do_some_shopping(citizen_t *c) {
    return c->doing_some_shopping;
}

state_t *dying(citizen_t *c){
    return c->dying;
}

state_t *finished(citizen_t *c){
    return c->finished;
}

// void *change_state(citizen_t *c, state_t *state) {
//     // int id = state->id;
//     if(c->health <= 0){
//         c->current_state->change_state(c, c->dying);
//     } else if (memory->simulation_has_ended){
//         c->current_state->change_state(c, c->finished);
//     } else {
//         // for(int i = 0; i < DAILY_CITIZEN_STATES; i++){
//         //     if(id == i){
//         //         c->current_state->change_state(c, c->change_state[(id%DAILY_CITIZEN_STATES)++]);
//         //     }
//         // }
//         c->current_state->change_state(c, state);
//     }
// }


// void *citizen_behavior(void *arg, memory_t *memory) {
//     citizen_t *character = (citizen_t *)arg;

//     while (!memory->simulation_has_ended) {
//         // Attendre le début du tour
//         pthread_barrier_wait(&start_barrier);

//         // Vérifier si la simulation n'est pas terminée
//         if (memory->simulation_has_ended) {
//             break;
//         }

//         double currentTime = get_current_simulation_time(memory);

//         handle_normal_citizen_actions(character, currentTime);

//         // Attendre la fin du tour
//         pthread_barrier_wait(&end_barrier);
//     }
    
//     return NULL;
// }

// void start_citizen_threads(citizen_t *characters_list) {
//     pthread_t thread_citizen[CITIZENS_COUNT];
//     for (int i = 0; i < CITIZENS_COUNT; i++) {
//         pthread_create(thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
//     }
// }

// void handle_normal_citizen_actions(citizen_t *character, double currentTime) {
//     // Logique pour les citoyens normaux
//     if (currentTime == 8.00) { 
//         move_citizen_to_work(character);
//         citizen_change_state(character, go_to_company(character));
//     } else if (currentTime == 17.00) {
//         handle_citizen_shopping_and_return_home(character);
//     } else if (currentTime == 19.50) {
//         move_citizen_to_home(character);
//         citizen_change_state(character, go_back_home(character));
//     }
// }

// void handle_citizen_shopping_and_return_home(citizen_t *character) {
//     if (!(is_at_supermarket(character))) {
//         int random = rand() % 4;
//         if (random == 0) {
//             change_state(character, go_to_supermarket(character));
//             move_citizen_to_supermarket(character);
//             change_state(character, do_some_shopping(character));
//         }
//         move_citizen_to_home(character);
//         citizen_change_state(character, go_back_home(character));
//     }
// }

void move_citizen_to_home(citizen_t *character) {
    /*Do the A* or BFS*/
    if(character->home->max_capacity > character->home->nb_citizen){
        character->position[0] = character->home->position[0]; 
        character->position[1] = character->home->position[1]; 
        character->home->nb_citizen++;
        // printf("Citizen %d moved to  at position (%d, %d)\n", 
            // character->id, character->position[0], character->position[1]);
    } else {
        // printf(stderr,"home is full\n");
    }
    
}

void move_citizen_to_work(citizen_t *character) {
    /*Do the A* or BFS*/
    if(character->workplace->max_capacity > character->workplace->nb_citizen){
        character->position[0] = character->workplace->position[0]; 
        character->position[1] = character->workplace->position[1]; 
        character->workplace->nb_citizen++;
        // printf("Citizen %d moved to work at position (%d, %d)\n", 
        //     character->id, character->position[0], character->position[1]);
    } else {
        // printf(stderr,"The workplace is full\n");
    }
    
}

void move_citizen_to_supermarket(citizen_t *character) {
    if(character->supermarket->max_capacity > character->supermarket->nb_citizen){
        character->position[0] = character->supermarket->position[0]; 
        character->position[1] = character->supermarket->position[1]; 
        character->supermarket->nb_citizen++;
        // printf("Citizen %d moved to supermarket at position (%d, %d)\n", 
        //     character->id, character->position[0], character->position[1]);
    } else {
        // printf(stderr,"The supermarket is full\n");
    }
     
}


// void initialize_synchronization_tools() {
//     pthread_barrier_init(&start_barrier, NULL, CITIZENS_COUNT);
//     pthread_barrier_init(&end_barrier, NULL, CITIZENS_COUNT);
//     //pthread_mutex_init(&mutex, NULL);
// }

// void manage_citizens(citizen_t *characters_list) {
//     pthread_t thread_citizen[CITIZENS_COUNT];

//     for (int i = 0; i < CITIZENS_COUNT; i++) {
//         pthread_create(&thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
//     }

//     pthread_barrier_wait(&start_barrier);

//     // Traitement du tour

//     pthread_barrier_wait(&end_barrier);

//     // Nettoyage
//     for (int i = 0; i < CITIZENS_COUNT; i++) {
//         pthread_join(thread_citizen[i], NULL);
//     }
// }
