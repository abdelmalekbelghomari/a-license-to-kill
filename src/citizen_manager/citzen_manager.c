#include <stdio.h>
#include "citizen_manager.h"
#include <pthread.h>
#include <sys/shm.h>
#include "memory.h"

#define SHARED_MEMORY "/spy_simulation"

memory_t *memory;  // Pointer to the shared memory
pthread_barrier_t barrier;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void use_shared_memory() {
    int shmd = shm_open(SHARED_MEMORY, O_RDWR,  S_IRUSR | S_IWUSR);
    if (shmd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    } else {
        citizen_t *characters_list = mmap(NULL, 
                                sizeof(citizen_t) * NUM_CITIZENS, 
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED, 
                                shmd, 
                                0);
        if (characters_list == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        } else {
            start_citizen_threads(characters_list);
        }
    }
}

double get_current_simulation_time() {
    return memory->timer.hours + round(memory->timer.minutes/60, 2);
    /*TO DO in timer*/
}

int is_at_supermarket(citizen_t *character) {
    return (character->position == character->supermarket->position);
}

void *citizen_behavior(citizen_t *character) {
    while (memory->turns < MAX_STEPS) {
        double currentTime = /*TO DO*/get_current_simulation_time(); // Function to get the current time in the simulation
        if (currentTime == 8.00) { 
            move_citizen_to_work(character);
            citizen_change_state(character,go_to_company(character));
        } else if (currentTime == 17.00) {
            if (!(is_at_supermarket(character))){
                int random = rand() % 4;
                if (random == 0) {
                    citizen_change_state(character,go_to_supermarket(character));
                    move_citizen_to_supermarket(character);
                    citizen_change_state(character,do_some_shopping(character));
                }
                move_citizen_to_home(character);
                citizen_change_state(character,go_back_home(character));
            }
        } else if(currentTime == 19.50 && is_at_supermarket(character)){
                move_citizen_to_home(character);
                citizen_change_state(character,go_to_home(character));
            }
        } 
    return NULL;
}

void move_citizen_to_work(citizen_t *character) {
    /*Do the A* or BFS*/
    int char_position[] = character->position;
    int wp_position[] = character->workplace->position;
    if(character->workplace->capacity > character->workplace->size){
        char_position[0], char_position[1] = wp_position[0], wp_position[1];
        character->workplace->size++;
    } else {
        printf(stderr,"The workplace is full\n");
    }
    
}

void move_citizen_to_supermarket(citizen_t *character) {
    if(character->supermarket->capacity > character->supermarket->size){
        character->position[0] = character->supermarket->position[0]; 
        character->position[1] = character->supermarket->position[1]; 
        character->supermarket->size++;
    } else {
        printf(stderr,"The supermarket is full\n");
    }
     
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

void citizen_change_state(citizen_t *c, state_t *new_state) {
    c->current_state->change_state(c, new_state);
}

void start_citizen_threads(citizen_t *characters_list) {
    pthread_t thread_citizen[NUM_CITIZENS];
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_create(thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
    }
}


