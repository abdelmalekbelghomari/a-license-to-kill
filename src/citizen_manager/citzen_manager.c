#include <stdio.h>
#include <citizen_manager.h>
#include <pthread.h>
#include <sys/shm.h>

void use_shared_memory() {
    key_t shm_key = ftok("shmfile", 65);
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
}


void *citizen_behavior(void *arg) {
    citizen *character = (citizen *)arg;
    while (simulation_running) {
        int currentTime = get_current_simulation_time(); // Function to get the current time in the simulation
        if (currentTime == 8) { 
            move_citizen_to_work(character);
            go_to_company(character);
        }
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

void move_citizen_to_work(citizen *character) {
    character->position = char_position;
    character->workplace_position = wp_position;
    char_postion[0], char_position[1] = wp_position[0], wp_position[1];
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
