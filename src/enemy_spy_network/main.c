#include "memory.h"
#include "enemy_spy_network.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define SPIES_COUNT 3
#define SEMAPHORE_NAME "/sem"

memory_t *memory;
sem_t *sem;
pthread_mutex_t shared_memory_mutex;
pthread_barrier_t turn_barrier;

void* spy_thread(void* arg) {
    int spy_id = *(int*)arg;
    int last_round_checked = -1;
    int last_day_checked = -1;
    int current_round = memory->timer.round;
    int current_day = memory->timer.days;
    while(current_round != 2016 /* || memory->simulation_has_ended==0 */) {
        //sem_wait(sem); // Attente pour accéder à la mémoire partagée
        //printf("current timer round : %d\n", memory->timer.round);
        current_round = memory->timer.round;
        current_day = memory->timer.days;
        // printf("caca\n");
        //sem_post(sem);

        if (last_round_checked != current_round) {
            pthread_mutex_lock(&shared_memory_mutex);
            //modifie ca pour implémenter le patron état
            // printf("spy id : %d , current state : %d\n", spy_id, memory->spies[spy_id].current_state->id);
            if(last_day_checked != current_day){    
                assign_leaving_time(&memory->spies[spy_id]);
            }
            // state_t *next_state = memory->spies[spy_id].current_state->action(&memory->spies[spy_id]);
            // memory->spies[spy_id].current_state = next_state;
            // sem_wait(sem);
            // state_t *next_state = memory->spies[spy_id].current_state->action(&memory->spies[spy_id]);
            // memory->spies[spy_id].current_state = next_state;
            // sem_post(sem);
            last_day_checked = current_day;
            last_round_checked = current_round;
            pthread_mutex_unlock(&shared_memory_mutex);
            pthread_barrier_wait(&turn_barrier);
            // printf ("\nspy id : %d , walking_spies : %d , at_home_spies : %d at_work_spies : %d\n",spy_id, memory->walking_spies , memory->at_home_spies, memory->at_work_spies);
        }

        usleep(100000); // 100 ms pour réduire la consommation CPU
    }

    return NULL;
}

void* officer_function(){
    int last_round_checked = -1;
    int current_round = memory->timer.round;
    int last_day_checked = -1;
    int current_day = memory->timer.days;
    while(current_round != 2016 /* || memory->simulation_has_ended==0 */) {
        //sem_wait(sem); // Attente pour accéder à la mémoire partagée
        //printf("current timer round : %d\n", memory->timer.round);
        current_round = memory->timer.round;
        current_day = memory->timer.days;
        // printf("caca\n");
        //sem_post(sem);

        if (last_round_checked != current_round) {
            pthread_mutex_lock(&shared_memory_mutex);
            if(last_day_checked != current_day){    
                assign_officer_times(&memory->case_officer);
                printf("first leaving time : %d : %d  , second leaving_time : %d : %d , shopping_time_time : %d : %d , messaging_time : %d:%d\n", 
            memory->case_officer.first_leaving_time.leaving_hour, memory->case_officer.first_leaving_time.leaving_minute, memory->case_officer.second_leaving_time.leaving_hour,
            memory->case_officer.second_leaving_time.leaving_minute,memory->case_officer.shopping_time.leaving_hour,memory->case_officer.shopping_time.leaving_minute,
            memory->case_officer.messaging_time.leaving_hour, memory->case_officer.messaging_time.leaving_minute);
            }
            // printf("first leaving time : %d : %d  , second leaving_time : %d : %d , third_leaving_time : %d : %d", 
            // memory->case_officer.first_leaving_time.leaving_hour, memory->case_officer.first_leaving_time.leaving_minute, memory->case_officer.second_leaving_time.leaving_hour,
            // memory->case_officer.second_leaving_time.leaving_minute,memory->case_officer.shopping_time.leaving_hour,memory->case_officer.shopping_time.leaving_minute);
            state_t *next_state = memory->case_officer.current_state->action(&memory->case_officer);
            memory->case_officer.current_state = next_state;
            // sem_wait(sem);
            // sem_post(sem);
            last_day_checked = current_day;
            last_round_checked = current_round;
            pthread_mutex_unlock(&shared_memory_mutex);
            pthread_barrier_wait(&turn_barrier);
            // printf ("\nspy id : %d , walking_spies : %d , at_home_spies : %d at_work_spies : %d\n",spy_id, memory->walking_spies , memory->at_home_spies, memory->at_work_spies);
        }

        usleep(100000); // 100 ms pour réduire la consommation CPU
    }

    return NULL;
}

int main() {
    printf("\n");
    srand(time(NULL) ^ getpid());// la graine doit être différente du parent
    pthread_t threads[SPIES_COUNT];
    pthread_t officer_thread;
    int spy_ids[SPIES_COUNT];
    int shm_fd;

    // Ouvrir la mémoire partagée
    shm_fd = shm_open(SHARED_MEMORY, O_RDWR, 0666);
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "memory.h" 


#define MAX_MESSAGE_SIZE 128
#define SHIFT 3

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

int main() {

    sem_t *sem = sem_open("/Mysemaphore", O_RDWR, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    int shm_fd;
    memory_t *shared_memory;
    SpyMessage messages[6];

    // Ouvrir la mémoire partagée existante
    shm_fd = shm_open("/SharedMemory", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapper la mémoire partagée
    memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Ouvrir le sémaphore
    sem = sem_open(SEMAPHORE_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }   
    init_spies(memory);
    init_officer(memory);
    // Initialisation de la barrière
    pthread_barrier_init(&turn_barrier, NULL, SPIES_COUNT+1);
    pthread_mutex_init(&shared_memory_mutex, NULL);

    // Créer les threads de citoyens
    for (int i = 0; i < SPIES_COUNT; i++) {
        // printf("spy id : %d\n", i);
        spy_ids[i] = i;
        if (pthread_create(&threads[i], NULL, &spy_thread, &spy_ids[i])) {
            perror("Failed to create thread");
            return 1;
        }
        
    }
    pthread_create(&officer_thread, NULL, officer_function, NULL);

    // Attente de la fin des threads de citoyens
    for (int i = 0; i < SPIES_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(officer_thread, NULL);

    // Nettoyage
    pthread_barrier_destroy(&turn_barrier);
    pthread_mutex_destroy(&shared_memory_mutex);
    sem_close(sem);
    munmap(memory, sizeof(memory_t));
    shared_memory = mmap(0, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialiser les messages
    strcpy(messages[0].content, "Very Low");
    messages[0].priority = 2;

    strcpy(messages[1].content, "Low");
    messages[1].priority = 3;

    strcpy(messages[2].content, "Medium");
    messages[2].priority = 6;

    strcpy(messages[3].content, "Strong");
    messages[3].priority = 9;

    strcpy(messages[4].content, "Crucial");
    messages[4].priority = 10;

    strcpy(messages[5].content, "Deceptive");
    messages[5].priority = 1;

    // Chiffrer tous les messages
    for (int i = 0; i < 6; ++i) {
        caesar_cipher(messages[i].content);
    }

    // Envoyer les messages à la queue de messages de la mémoire partagée
    for (int i = 0; i < 6; ++i) {
        printf("\nDescripteur de la queue de messages: %d\n", shared_memory->mqInfo.mq);
        printf("sizeof(SpyMessage : %ld)" , sizeof(SpyMessage));
        printf("priority : %d", messages[i].priority);
        if (mq_send(shared_memory->mqInfo.mq, (char *)&messages[i], sizeof(SpyMessage), messages[i].priority) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
    }

    // Fermer et désallouer les ressources
    munmap(shared_memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}

