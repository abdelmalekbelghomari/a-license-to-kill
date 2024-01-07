/**
 * @file main.c
 * @brief Main entry point for the spy simulation.
 *
 * This file contains the main entry point (the `main` function) for
 * creating and managing citizens in the spy simulation. It initializes
 * the necessary threads for handling citizens in the simulation.
 */

#include "memory.h"
#include "citizen_manager.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define SEMAPHORE_CONSUMER "/semConsumer"
#define SEMAPHORE_PRODUCER "/semProducer"
#define CITIZENS_DEBUG 1

memory_t *memory;
sem_t *sem_producer, *sem_consumer;
pthread_mutex_t shared_memory_mutex;
pthread_barrier_t turn_barrier;
int threads_at_barrier = 0;

/**
 * @brief Fonction exécutée par chaque thread de citoyen.
 *
 * Gère le comportement d'un citoyen dans la simulation. Cette fonction est assignée à chaque
 * thread créé pour un citoyen et s'exécute tant que la simulation n'est pas terminée.
 *
 * @param arg Pointeur vers l'identifiant du citoyen.
 * @return Pointeur void* retourné à la fin de l'exécution du thread.
 */

void* citizen_thread(void* arg) {
    unsigned int citizen_id = *(int*)arg;
    int last_round_checked = -1;
    int current_round = memory->timer.round;
    while(current_round != 2016 /* || memory->simulation_has_ended==0 */) {
        //sem_wait(sem_producer_timer); // Attente pour accéder à la mémoire partagée
        // printf("current timer round : %d\n", memory->timer.round);
        current_round = memory->timer.round;
        // printf("caca\n");
        //sem_post(sem_consumer_timer);

        if (last_round_checked != current_round) {
            pthread_mutex_lock(&shared_memory_mutex);
            state_t *next_state = memory->citizens[citizen_id].current_state->action(&memory->citizens[citizen_id]);
            memory->citizens[citizen_id].current_state = next_state;
            last_round_checked = current_round;
            threads_at_barrier++;
            pthread_mutex_unlock(&shared_memory_mutex);
        }
        pthread_barrier_wait(&turn_barrier);
        usleep(100000); // 100 ms pour réduire la consommation CPU
    }
    return NULL;
}

/**
 * @brief Fonction principale du programme.
 *
 * Crée et gère les threads des citoyens, et nettoie les ressources
 * à la fin de la simulation.
 *
 * @return int Code de sortie du programme.
 */

int main() {
    srand(time(NULL) ^ getpid()	);
    pthread_t threads[CITIZENS_COUNT];
    int shm_fd;

    // Ouvrir la mémoire partagée
    shm_fd = shm_open(SHARED_MEMORY, O_RDWR, 0666);
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
    
    init_building(memory);
    init_citizens(memory);

    // Initialisation de la barrière
    pthread_barrier_init(&turn_barrier, NULL, CITIZENS_COUNT);
    pthread_mutex_init(&shared_memory_mutex, NULL);

    // Créer les threads de citoyens
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        // printf("citizen id : %d\n", i);
        memory->citizens[i].id = i;
        if (pthread_create(&threads[i], NULL, citizen_thread, &memory->citizens[i].id)) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Attente de la fin des threads de citoyens
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Nettoyage
    free(memory->citizens);
    free(memory->companies);
    free(memory->homes);
    pthread_barrier_destroy(&turn_barrier);
    pthread_mutex_destroy(&shared_memory_mutex);
    sem_close(sem_consumer);
    sem_close(sem_producer);
    munmap(memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}