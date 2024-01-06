#include <stdio.h>
#include "citizen_manager.h"
#include <pthread.h>
#include <math.h>
#include <bits/pthreadtypes.h>
#include <float.h>
#include "astar.h"

#define SHARED_MEMORY "/SharedMemory"

extern sem_t *sem_producer_timer, *sem_consumer_timer;

int DIRECTION[NUM_DIRECTIONS][2] = {{-1, 0},
                                     {1,  0},
                                     {0,  -1},
                                     {0,  1},
                                     {-1, -1},
                                     {-1, 1},
                                     {1,  -1},
                                     {1,  1}};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t end_of_the_day_barrier;
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
    
    for(int i = 0; i < NB_HOMES; i++){
        if (i == fakeHome){
            memory->homes[i].max_capacity = 14;
        }
        else{
            memory->homes[i].max_capacity = 15;
        }
    }
    memory->homes->mailbox.is_occupied = false;
    int home_counter = 0;
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int column = 0; column < MAX_COLUMNS; column++) {
            cell_t cell = memory->map.cells[row][column];
            if (cell.type == RESIDENTIAL_BUILDING) {
                if(home_counter == fakeHome){

                    memory->homes[home_counter].has_mailbox = true;
                    memory->homes[home_counter].mailbox.row = row;
                    memory->map.mailbox_row = row;
                    memory->map.mailbox_column = column;
                    memory->homes[home_counter].mailbox.column = column;
                    memory->homes[home_counter].position[0] = row;
                    memory->homes[home_counter].position[1] = column;  
                    memory->homes[home_counter].nb_citizen = 0;
                    for(int k=0; k < NUM_DIRECTIONS; k++) {
                        int in_front_row = row + DIRECTION[k][0];
                        int in_front_column = column + DIRECTION[k][1];

                        if (in_front_row < 0 || in_front_row >= MAX_ROWS || in_front_column < 0 || in_front_column >= MAX_COLUMNS /*|| is_cell_full(map, x, y)*/ ) {
                            continue; 
                        }

                        if (cell.type != WASTELAND) {
                            continue;
                        }

                        memory->homes[home_counter].mailbox.row_in_front = in_front_row;
                        memory->homes[home_counter].mailbox.column_in_front = in_front_column;
                        break;
                    }

                } else {
                    memory->homes[home_counter].has_mailbox = false;
                    memory->homes[home_counter].position[0] = row;
                    memory->homes[home_counter].position[1] = column;  
                    memory->homes[home_counter].nb_citizen = 0;
                }
                home_counter++;
            }
        }
    }
}

void init_building(memory_t *memory){
    for(int i = 0; i < NB_WORKPLACES; i++){
        memory->companies[i].nb_workers = 0;
        if(i < NB_STORE){
            memory->companies[i].cell_type = SUPERMARKET;
            memory->companies[i].max_workers = 3;
            memory->companies[i].min_workers = 3;
        } else if (i == NB_STORE) {
            memory->companies[i].min_workers = 10;
            memory->companies[i].max_workers = 10;
            memory->companies[i].cell_type = CITY_HALL;
        } else {
            memory->companies[i].min_workers = 5;
            memory->companies[i].max_workers = 50;
            memory->companies[i].cell_type = COMPANY;
        }
    }

    int supermarket_counter = 0, city_hall_counter = NB_STORE, company_counter = NB_STORE + NB_HALL; 
    for (int row=0; row < MAX_ROWS; row++){
        for (int column = 0; column < MAX_COLUMNS; column++){
            cell_t cell =memory->map.cells[row][column];
            if(cell.type == SUPERMARKET){
                memory->companies[supermarket_counter].cell_type = cell.type;
                memory->companies[supermarket_counter].position[0] = row;
                memory->companies[supermarket_counter].position[1] = column;
                supermarket_counter++;
            }
            if(cell.type == CITY_HALL){
                memory->companies[city_hall_counter].cell_type = cell.type;
                memory->companies[city_hall_counter].position[0] = row;
                memory->companies[city_hall_counter].position[1] = column;
                city_hall_counter++;
            }
            if(cell.type == COMPANY){
                memory->companies[company_counter].cell_type = cell.type;
                memory->companies[company_counter].position[0] = row;
                memory->companies[company_counter].position[1] = column;
                company_counter++;
            }
        }
    }
}

void init_citizens(memory_t *memory) {

    sem_wait(sem_consumer_timer);
    init_house(memory);
    init_building(memory);
    sem_post(sem_producer_timer);

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];
        citizen->id = i;
        citizen->type = NORMAL;
        citizen->health = 10;
        citizen->current_step = 0;
        citizen->time_spent_shopping = 0;

        citizen->resting_at_home = new_state(0, rest_at_home);
        citizen->going_to_company = new_state(1, go_to_company);
        citizen->working = new_state(2, work);
        citizen->going_to_supermarket = new_state(3, go_to_supermarket);
        citizen->doing_some_shopping = new_state(4, do_some_shopping);
        citizen->going_back_home = new_state(5, go_back_home);
        citizen->dying = new_state(6, dying);
        citizen->finished = new_state(7, finished);

        citizen->current_state = citizen->resting_at_home;
        memory->at_home_citizens++;

        assign_home_to_citizen(memory, citizen);
        assign_company_to_citizen(memory, citizen);                                                     
        assign_random_supermarket(memory, citizen);

        citizen->position[0] = citizen->home->position[0];
        citizen->position[1] = citizen->home->position[1];

        int column_home = citizen->home->position[1];
        int row_home = citizen->home->position[0];
        int column_company = citizen->workplace->position[1];
        int row_company = citizen->workplace->position[0];
        int column_supermarket = citizen->supermarket->position[1];
        int row_supermarket = citizen->supermarket->position[0];

        Node *end_node_company = astar_search(&memory->map, row_home, column_home, row_company, column_company);
        if (end_node_company != NULL) {
            Path *path_to_work = reconstruct_path(end_node_company);
            if (path_to_work != NULL) {
                citizen->path_to_work = path_to_work;
            } else {
                fprintf(stderr,"Échec de la reconstruction du chemin pour le citoyen à son entreprise %d\n", citizen->id);
                citizen->path_to_work = NULL; 
            }
        } else {
            fprintf(stderr,"Aucun chemin trouvé pour le citoyen à son entreprise %d\n", citizen->id);
            citizen->path_to_work = NULL; 
        }
        

        Node *end_node_supermarket = astar_search(&memory->map, row_company, column_company, row_supermarket, column_supermarket);
        if (end_node_company != NULL) {
                Path *path_supermarket = reconstruct_path(end_node_supermarket);
            if (path_supermarket != NULL) {
                citizen->path_to_supermarket = path_supermarket;
            } else {
                fprintf(stderr, "Échec de la reconstruction du chemin pour le citoyen à son supermarché %d\n", citizen->id);
                citizen->path_to_supermarket = NULL; 
            }
        } else {
            // Échec de la recherche de chemin
            fprintf(stderr,"Aucun chemin trouvé pour le citoyen à son supermarché %d\n", citizen->id);
            citizen->path_to_supermarket = NULL; 
        }

        Node *end_node_from_sprmrkt_to_home = astar_search(&memory->map, row_supermarket, column_supermarket, row_home, column_home);
        if (end_node_from_sprmrkt_to_home != NULL) {
            Path *path_from_supermarket_to_home = reconstruct_path(end_node_from_sprmrkt_to_home);
            if (path_from_supermarket_to_home != NULL) {
                citizen->path_from_supermarket_to_home = path_from_supermarket_to_home;
            } else {
                fprintf(stderr,"Échec de la reconstruction du chemin pour le citoyen à son domicile %d\n", citizen->id);
                citizen->path_from_supermarket_to_home = NULL;
            }
        } else {
            fprintf(stderr,"Aucun chemin trouvé pour le citoyen à son domicile %d\n", citizen->id);
            citizen->path_from_supermarket_to_home = NULL; 
        }
    }
           
}

void assign_home_to_citizen(memory_t* memory, citizen_t* citizen){

    home_t *houses = memory->homes;

    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            citizen->home = &houses[house_index];
            houses[house_index].nb_citizen++;
            break;
        }
        attempts++;
    }
}

void assign_company_to_citizen(memory_t* memory, citizen_t* citizen){
    building_t *company_list = memory->companies;
    int company_index;
    int attempts = 0; 
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
        for (company_index = 3; company_index < NB_COMPANY + 3; company_index++) {
            if (company_list[company_index].nb_workers < 5) {
                citizen->workplace = &company_list[company_index];
                company_list[company_index].nb_workers++;
                return; 
            }
        }
        while (attempts < NB_COMPANY) {
            company_index = 3 + rand() % NB_COMPANY;
            if (company_list[company_index].nb_workers < company_list[company_index].max_workers) { 
                citizen->workplace = &company_list[company_index]; 
                company_list[company_index].nb_workers++;
                break;
            } else {
                attempts++;
            }
        }
    }
}

void assign_random_supermarket(memory_t* memory, citizen_t* citizen){
    
    building_t supermaket_list[NB_STORE] = {memory->companies[0], memory->companies[1]};

    int supermaketChoice = rand() % NB_STORE;
    if(supermaketChoice == 0){
        citizen->supermarket = &supermaket_list[0];
    } else {
        citizen->supermarket = &supermaket_list[1];
    }
    
}


double get_current_simulation_time(memory_t *memory) {
    return memory->timer.hours + round(memory->timer.minutes * 100.0) / 100;
}


state_t *new_state(int id, state_t *(*action)(citizen_t *)) {
    state_t *state = malloc(sizeof(state_t));
    state->id = id;
    state->action = action;
    return state;
}

state_t *rest_at_home(citizen_t *c) {
    c->is_coming_from_company = 0;
    c->current_step = 0;
    if (memory->timer.hours == 8 && memory->timer.minutes >= 0) { 
        pthread_mutex_lock(&mutex);
        
        memory->at_home_citizens--;
        if(memory->at_home_citizens < 0){
            memory->at_home_citizens = 0;
        }
        memory->walking_citizens++;

        pthread_mutex_unlock(&mutex);
        return c->going_to_company;
    } else {
        return c->resting_at_home;
    }
}

state_t *go_to_company(citizen_t *c) {
    // printf("je vais vers mon boulot\n");
    
    if (c->position[0] == c->path_to_work->nodes[c->path_to_work->length -1]->position[0] && c->position[1] == c->path_to_work->nodes[c->path_to_work->length -1]->position[1]){
        c->current_step = 0;
        pthread_mutex_lock(&mutex);
        
        memory->walking_citizens--;
        if(memory->walking_citizens < 0){
            memory->walking_citizens = 0;
        }
        memory->at_work_citizens++;
        
        pthread_mutex_unlock(&mutex);
        return c->working;
    } else { 
        if (c->path_to_work != NULL && c->current_step < c->path_to_work->length) {

            Node *next_node = c->path_to_work->nodes[c->current_step];

            c->position[0] = next_node->position[0];
            c->position[1] = next_node->position[1];

            c->current_step++;

            return c->going_to_company;
        }
        
    }

}

state_t *work(citizen_t *c) {
    if(c->workplace->cell_type == SUPERMARKET){
        if(memory->timer.hours == 19 && memory->timer.minutes >= 30){
            pthread_mutex_lock(&mutex);
            
            memory->at_work_citizens--;
            if(memory->at_work_citizens < 0){
                memory->at_work_citizens = 0;
            }
            memory->walking_citizens++;
            
            pthread_mutex_unlock(&mutex);
            c->current_step = c->path_to_work->length - 1;
            
            c->is_coming_from_company = 1;
            return c->going_back_home;
        } else {
            return c->working;
        }
    } else if (memory->timer.hours == 17 && memory->timer.minutes >= 0) { 
        int value = rand() % 4;
        if (value < 1) {   
            pthread_mutex_lock(&mutex);
            
            memory->at_work_citizens--;
            if(memory->at_work_citizens < 0){
                memory->at_work_citizens = 0;
            }
            memory->walking_citizens++;
            
            pthread_mutex_unlock(&mutex);
            c->current_step = 0;
            
            return c->going_to_supermarket;
        } else {
            pthread_mutex_lock(&mutex);
            
            memory->at_work_citizens--;
            if(memory->at_work_citizens < 0){
                memory->at_work_citizens = 0;
            }
            memory->walking_citizens++;
            
            pthread_mutex_unlock(&mutex);
            c->current_step = c->path_to_work->length - 1;
            c->is_coming_from_company = 1;
            
            return c->going_back_home;
        }
        
    } else {
        return c->working;
    }
    return c->working;
}

state_t *go_to_supermarket(citizen_t *c) {
    // printf("je vais au marché\n");
    if (c->position[0] == c->path_to_supermarket->nodes[c->path_to_supermarket->length -1]->position[0] && c->position[1] == c->path_to_supermarket->nodes[c->path_to_supermarket->length -1]->position[1]){
        c->current_step = 0;
        pthread_mutex_lock(&mutex);
        
        memory->walking_citizens--;
        if(memory->walking_citizens < 0){
            memory->walking_citizens = 0;
        }
        
        pthread_mutex_unlock(&mutex);
        c->time_spent_shopping = 0;
        return c->doing_some_shopping;
    } else {
        if (c->path_to_supermarket != NULL && c->current_step < c->path_to_supermarket->length) {
            Node *next_node = c->path_to_supermarket->nodes[c->current_step];

            c->position[0] = next_node->position[0];
            c->position[1] = next_node->position[1];

            c->current_step++;

            return c->going_to_supermarket;
        }
        
        
    }
}



state_t *go_back_home(citizen_t *c) {
    // printf("je rentre chez oim\n");

    if(!c->is_coming_from_company){
        if (c->position[0] == c->path_from_supermarket_to_home->nodes[c->current_step]->position[0] && c->position[1] == c->path_from_supermarket_to_home->nodes[c->current_step]->position[1]){
            c->current_step = 0;
            pthread_mutex_lock(&mutex);
            
            memory->walking_citizens--;
            if(memory->walking_citizens < 0){
                memory->walking_citizens = 0;
            }
            memory->at_home_citizens++;
            
            pthread_mutex_unlock(&mutex);
            return c->resting_at_home;
        } else {
            if (c->path_from_supermarket_to_home != NULL && c->current_step < c->path_to_work->length) {
                Node *next_node = c->path_from_supermarket_to_home->nodes[c->current_step];

                c->position[0] = next_node->position[0];
                c->position[1] = next_node->position[1];

                c->current_step++;
                
                return c->going_back_home;
            } else if (c->path_from_supermarket_to_home == NULL) {
                printf("Citizen %d - Path to work is NULL\n", c->id);
                return c->going_back_home;
            } else {
                return c->going_back_home;
            }
            
        }
    } else {
        if(c->position[0] == c->home->position[0] && c->position[1] == c->home->position[1]){
            c->current_step = 0;
            pthread_mutex_lock(&mutex);
            
            memory->walking_citizens--;
            if(memory->walking_citizens < 0){
                memory->walking_citizens = 0;
            }
            memory->at_home_citizens++;
            
            pthread_mutex_unlock(&mutex);
            return c->resting_at_home;
        } else {
            if (c->path_to_work != NULL && c->current_step >= 0 && c->current_step < c->path_to_work->length) {
                Node *next_node = c->path_to_work->nodes[c->current_step];
                c->position[0] = next_node->position[0];
                c->position[1] = next_node->position[1];

                c->current_step--;
                return c->going_back_home;
            }
            
        }
    }
    
}

    

state_t *do_some_shopping(citizen_t *c) {
    if(c->time_spent_shopping >= 30 || (memory->timer.hours == 19 && memory->timer.minutes >= 30)){
        c->current_step = 0;
        c->time_spent_shopping = 0;
        c->is_coming_from_company = 0;
        pthread_mutex_lock(&mutex);
        
        memory->walking_citizens++;
        
        pthread_mutex_unlock(&mutex);
        return c->going_back_home;
    } else {
        c->time_spent_shopping+=10;
        return c->doing_some_shopping;
    }
    
}

state_t *dying(citizen_t *c){
    return c->dying;
}

state_t *finished(citizen_t *c){
    return c->finished;
}
