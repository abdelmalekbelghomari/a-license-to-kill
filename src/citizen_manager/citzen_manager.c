#include <stdio.h>
#include <citizen_manager.h>
#include <pthread.h>
#include <sys/shm.h>

int shm_id = shmget(shm_key, sizeof(citizen) * NUM_CITIZENS, 0666);
if (shm_id < 0) {
    perror("shmget error");
    exit(1);
}

citizen *caracters_list = (citizen *)shmat(shm_id, NULL, 0);
if (citizens == (citizen *)(-1)) {
    perror("shmat error");
    exit(1);
}


void *citizen_behavior(void *arg) {
    citizen *character = (citizen *)arg;
    while (simulation_running) {
        if(character->going_to_company == 1){
            go_to_company(character);
        }
        if(character->resting_at_home == 1){
            rest_at_home(character);
        }
        else if(character->going_to_home == 1){
        }
    return NULL;
}

void start_citizen_threads() {
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_create(&characters_list[i].thread, NULL, citizen_behavior, &characters_list[i]);
    }
}

int main() {
    // Code à ajouter ici

    return 0;
}
