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

citizen_t *new_character(state_t *resting_at_home,
                           state_t *going_to_company,
                           state_t *working,
                           state_t *going_back_home,
                           state_t *going_to_supermarket,
                           state_t *doing_some_shopping)
{
    citizen_t *c = (citizen_t *) malloc(sizeof(citizen_t));

    c->current_state = resting_at_home;
    c->resting_at_home = resting_at_home;
    c->going_to_company = going_to_company;
    c->working = working;
    c->going_back_home = going_back_home;
    c->going_to_supermarket = going_to_supermarket;
    c->doing_some_shopping = doing_some_shopping;
    c->change_state = character_change_state;
    c->begin = character_begin;
    c->step = character_step;
    c->end = character_end;

    return c;
}

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

void state_change_state(citizen_t *c, state_t *s) {
    c->current_state = s;
}

state_t *rest_at_home(citizen_t *c) {
    if(get_current_simulation_time(memory) == 8.00){
        c->step(c);
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

void *change_state(citizen_t *c, state_t *state) {
    // int id = state->id;
    if(c->health <= 0){
        c->current_state->change_state(c, c->dying);
    } else if (memory->simulation_has_ended){
        c->current_state->change_state(c, c->finished);
    } else {
        // for(int i = 0; i < DAILY_CITIZEN_STATES; i++){
        //     if(id == i){
        //         c->current_state->change_state(c, c->change_state[(id%DAILY_CITIZEN_STATES)++]);
        //     }
        // }
        c->current_state->change_state(c, state);
    }
}

void *begin(citizen_t *c) {
    c->resting_at_home;
}

void *end(citizen_t *c) {
    c->finished;
}

void *step(citizen_t *c) {
    c->current_state->step(c);
}


void *citizen_behavior(void *arg, memory_t *memory) {
    citizen_t *character = (citizen_t *)arg;

    while (!memory->simulation_has_ended) {
        /* Wait for the start of the round */
        pthread_barrier_wait(&start_barrier);

        /* Check if the simulation is not finished */
        if (memory->simulation_has_ended) {
            break;
        }

        double currentTime = get_current_simulation_time(memory);

        handle_normal_citizen_actions(character, currentTime);

        /* Wait for the end of the round */
        pthread_barrier_wait(&end_barrier);
    }
    
    return NULL;
}

void start_citizen_threads(citizen_t *characters_list) {
    pthread_t thread_citizen[CITIZENS_COUNT];
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        pthread_create(thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
    }
}

void handle_normal_citizen_actions(citizen_t *character, double currentTime) {
    /* Logic for normal citizens */
    if (currentTime == 8.00) { 
        move_citizen_to_work(character);
        citizen_change_state(character, go_to_company(character));
    } else if (currentTime == 17.00) {
        handle_citizen_shopping_and_return_home(character);
    } else if (currentTime == 19.50) {
        move_citizen_to_home(character);
        citizen_change_state(character, go_back_home(character));
    }
}

void handle_citizen_shopping_and_return_home(citizen_t *character) {
    if (!(is_at_supermarket(character))) {
        int random = rand() % 4;
        if (random == 0) {
            citizen_change_state(character, go_to_supermarket(character));
            move_citizen_to_supermarket(character);
            citizen_change_state(character, do_some_shopping(character));
        }
        move_citizen_to_home(character);
        citizen_change_state(character, go_back_home(character));
    }
}

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



void initialize_synchronization_tools() {
    pthread_barrier_init(&start_barrier, NULL, CITIZENS_COUNT);
    pthread_barrier_init(&end_barrier, NULL, CITIZENS_COUNT);
    pthread_mutex_init(&mutex, NULL);
}

void manage_citizens(citizen_t *characters_list) {
    pthread_t thread_citizen[CITIZENS_COUNT];

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        pthread_create(&thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
    }

    pthread_barrier_wait(&start_barrier);

    /* Round processing */

    pthread_barrier_wait(&end_barrier);

    /* Cleanup */
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        pthread_join(thread_citizen[i], NULL);
    }
}