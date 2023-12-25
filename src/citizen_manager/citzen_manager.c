#include <stdio.h>
#include <citizen_manager.h>
#include <pthread.h>
#include <sys/shm.h>

void use_shared_memory() {
    int shmd = shm_open(SHARED_MEMORY_NAME, O_RDWR,  S_IRUSR | S_IWUSR);
    if (shmd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    } else {
        citizen *caracters_list = mmap(NULL, sizeof(citizen*MAX_CITIZENS), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
        if (citizens == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        } else {
            start_citizen_threads();
        }
    }
}


void *citizen_behavior(void *arg) {
    citizen *character = (citizen *)arg;
    while (simulation_running) {
        int currentTime = get_current_simulation_time(); // Function to get the current time in the simulation
        if (currentTime == 8) { 
            move_citizen_to_work(character);
            citizen_change_state(character,go_to_company(character));
        } else if (currentTime == 17) {
            if ((character->workplace_position != supermarket_position[0] 
            || character->workplace_position != supermarket_position[1])){
                int random = rand() % 4;
                if (random == 0) {
                    citizen_change_state(character,go_to_supermarket(character));
                    move_citizen_to_supermarket(character);
                    citizen_change_state(character,do_some_shopping(character));
                }
                move_citizen_to_home(character);
                citizen_change_state(character,go_back_home(character));
            }
        } else if(currentTime == 19,5 && character->workplace_position == (supermarket_position[0] || supermarket_position[1])){
                move_citizen_to_home(character);
                citizen_change_state(character,go_to_home(character));
            }
        } 
    return NULL;
}

void move_citizen_to_work(citizen *character) {
    character->position = char_position;
    character->workplace_position = wp_position;
    char_postion[0], char_position[1] = wp_position[0], wp_position[1];
}

void move_citizen_to_supermarket(citizen *character) {
    character->position = char_position;
    int random = rand() % 2;
    char_postion[0], char_position[1] = supermarket_position[random][0], supermarket_position[random][1];
}


void start_citizen_threads() {
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_create(&characters_list[i].thread, NULL, citizen_behavior, &characters_list[i]);
    }
}

int main() {
    use_shared_memory();
    

    return 0;
}
