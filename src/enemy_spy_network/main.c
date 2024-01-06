#include "memory.h"
#include "enemy_spy_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define SPY_DEBUG 3
#define START_HOUR_OF_DAY 0 
#define SEMAPHORE_CONSUMER "/semConsumer"
#define SEMAPHORE_PRODUCER "/semProducer"

memory_t *memory;
sem_t *sem, *sem_producer, *sem_consumer;
mqd_t mq;
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
        int hour_of_day = (current_round / 6) % 24; // Calcule l'heure actuelle du jour
        // printf("caca\n");
        //sem_post(sem);

        if (last_round_checked != current_round) {
            pthread_mutex_lock(&shared_memory_mutex);
            //modifie ca pour implémenter le patron état
            // printf("spy id : %d , current state : %d\n", spy_id, memory->spies[spy_id].current_state->id);
            if (hour_of_day == START_HOUR_OF_DAY && memory->timer.minutes == 0) {
                assign_leaving_time(&memory->spies[spy_id]);
                // printf("========================= NEW DAY ==========================");
            }
            // state_t *next_state = memory->spies[spy_id].current_state->action(&memory->spies[spy_id]);
            // memory->spies[spy_id].current_state = next_state;
            // sem_wait(sem);
            state_t *next_state = memory->spies[spy_id].current_state->action(&memory->spies[spy_id]);
            memory->spies[spy_id].current_state = next_state;
            // printf("numero de l'état de l'espion %d est %d, heure  %d:%d : \n",spy_id, memory->spies[spy_id].current_state->id , memory->timer.hours , memory->timer.minutes);
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
                // printf("first leaving time : %d : %d  , second leaving_time : %d : %d , shopping_time_time : %d : %d , messaging_time : %d:%d\n", 
                // memory->case_officer.first_leaving_time.leaving_hour, memory->case_officer.first_leaving_time.leaving_minute, memory->case_officer.second_leaving_time.leaving_hour,
                // memory->case_officer.second_leaving_time.leaving_minute,memory->case_officer.shopping_time.leaving_hour,memory->case_officer.shopping_time.leaving_minute,
                // memory->case_officer.messaging_time.leaving_hour, memory->case_officer.messaging_time.leaving_minute);
            }
            // printf("first leaving time : %d : %d  , second leaving_time : %d : %d , third_leaving_time : %d : %d", 
            // memory->case_officer.first_leaving_time.leaving_hour, memory->case_officer.first_leaving_time.leaving_minute, memory->case_officer.second_leaving_time.leaving_hour,
            // memory->case_officer.second_leaving_time.leaving_minute,memory->case_officer.shopping_time.leaving_hour,memory->case_officer.shopping_time.leaving_minute);
            state_t *next_state = memory->case_officer.current_state->action(&memory->case_officer);
            memory->case_officer.current_state = next_state;
            // printf("numero de l'état de l'officier est %d, heure  %d:%d : \n", memory->case_officer.current_state->id , memory->timer.hours , memory->timer.minutes);
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
    // printf("\n");
    srand(time(NULL) ^ getpid());// la graine doit être différente du parent
    pthread_t threads[SPY_DEBUG];
    pthread_t officer_thread;
    int spy_ids[SPY_DEBUG];
    int shm_fd;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open("/spy_message_queue", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Ouvrir la mémoire partagée
    shm_fd = shm_open(SHARED_MEMORY, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open enemy_spy_network");
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
    // sem = sem_open(SEMAPHORE_NAME, 0);
    // if (sem == SEM_FAILED) {
    //     perror("sem_open enemy_spy_network opening");
    //     exit(EXIT_FAILURE);
    // }   

    // Ouvrir le sémaphore
    sem_consumer = sem_open(SEMAPHORE_CONSUMER, 0);
    if (sem_consumer == SEM_FAILED) {
        perror("sem_open citizen");
        exit(EXIT_FAILURE);
    }   
    sem_producer = sem_open(SEMAPHORE_PRODUCER, 0);
    if (sem_producer == SEM_FAILED) {
        perror("sem_open citizen");
        exit(EXIT_FAILURE);
    }

    init_spies(memory);
    init_officer(memory);
    // Initialisation de la barrière
    pthread_barrier_init(&turn_barrier, NULL, SPY_DEBUG+1);
    pthread_mutex_init(&shared_memory_mutex, NULL);

    // Créer les threads de spy
    for (int i = 0; i < SPY_DEBUG; i++) {
        // printf("spy id : %d\n", i);
        spy_ids[i] = i;
        if (pthread_create(&threads[i], NULL, &spy_thread, &spy_ids[i])) {
            perror("Failed to create thread");
            return 1;
        }
        
    }
    pthread_create(&officer_thread, NULL, officer_function, NULL);

    // Attente de la fin des threads de citoyens
    for (int i = 0; i < SPY_DEBUG; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(officer_thread, NULL);

    // Nettoyage
    free(memory->spies);
    free(memory->messages);
    pthread_barrier_destroy(&turn_barrier);
    pthread_mutex_destroy(&shared_memory_mutex);
    sem_close(sem_consumer);
    sem_close(sem_producer);
    munmap(memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}
