#include <stdio.h>
#include "citizen_manager.h"
#include <pthread.h>
#include <sys/shm.h>
#include "memory.h"

#define SHARED_MEMORY "/spy_simulation"

memory_t *memory;  // Pointer to the shared memory
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

/*void *citizen_behavior(void *arg) {
    citizen_t *character = (citizen_t *) arg;
    while (memory->turns < MAX_STEPS || memory->end_of_simulation == 0) {
        double currentTime = get_current_simulation_time(); // Function to get the current time in the simulation
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
}*/

void *citizen_behavior(void *arg) {
    citizen_t *character = (citizen_t *)arg;

    while (!memory->simulation_has_ended) {
        // Attendre le début du tour
        pthread_barrier_wait(&start_barrier);

        // Vérifier si la simulation n'est pas terminée
        if (memory->simulation_has_ended) {
            break;
        }

        double currentTime = get_current_simulation_time();

        // Logique spécifique à chaque type de personnage
        switch (character->type) {
            case NORMAL:
                // Logique pour les citoyens normaux
                handle_normal_citizen_actions(character, currentTime);
                break;
            case SPY:
                // Logique pour les espions
                handle_spy_actions(character, currentTime);
                break;
            case CASE_OFFICER:
                // Logique pour l'officier traitant
                handle_case_officer_actions(character, currentTime);
                break;
            case COUNTER_INTELLIGENCE_OFFICER:
                // Logique pour l'officier de contre-espionnage
                handle_counter_intelligence_actions(character, currentTime);
                break;
        }

        // Attendre la fin du tour
        pthread_barrier_wait(&end_barrier);
    }
    
    return NULL;
}

void handle_normal_citizen_actions(citizen_t *character, double currentTime) {
    // Logique pour les citoyens normaux
    if (currentTime == 8.00) { 
        move_citizen_to_work(character);
        citizen_change_state(character, go_to_company(character));
    } else if (currentTime == 17.00) {
        // Condition pour aller au supermarché et rentrer à la maison
        handle_citizen_shopping_and_return_home(character);
    } else if (currentTime == 19.50) {
        move_citizen_to_home(character);
        citizen_change_state(character, go_to_home(character));
    }
}

void handle_spy_actions(citizen_t *character, double currentTime) {
    // Logique pour les espions
    if (character->type == SPY) {
        // Implémenter les actions spécifiques des espions
        // Exemple: collecte d'informations, dépôt de messages, etc.
    }
}

void handle_case_officer_actions(citizen_t *character, double currentTime) {
    // Logique pour l'officier traitant
    if (character->type == CASE_OFFICER) {
        // Implémenter les actions spécifiques de l'officier traitant
        // Exemple: collecte de messages, transmission d'informations, etc.
    }
}

void handle_counter_intelligence_actions(citizen_t *character, double currentTime) {
    // Logique pour l'officier du contre-espionnage
    if (character->type == COUNTER_INTELLIGENCE_OFFICER) {
        // Implémenter les actions spécifiques de l'officier du contre-espionnage
        // Exemple: surveillance, recherche de la boîte aux lettres, etc.
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

pthread_barrier_t start_barrier, end_barrier;

void initialize_synchronization_tools() {
    pthread_barrier_init(&start_barrier, NULL, NUM_CITIZENS);
    pthread_barrier_init(&end_barrier, NULL, NUM_CITIZENS);
    pthread_mutex_init(&mutex, NULL);
}

void manage_citizens(citizen_t *characters_list) {
    pthread_t thread_citizen[NUM_CITIZENS];

    // Création des threads
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_create(&thread_citizen[i], NULL, citizen_behavior, &characters_list[i]);
    }

    // Attente que tous les threads soient prêts
    pthread_barrier_wait(&start_barrier);

    // Traitement du tour
    // ... (logique spécifique à chaque tour)

    // Attente que tous les threads aient terminé leur traitement
    pthread_barrier_wait(&end_barrier);

    // Nettoyage
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_join(thread_citizen[i], NULL);
    }
}