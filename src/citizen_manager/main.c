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
#define SEMAPHORE_CONSUMER "/semTimerConsumer"
#define SEMAPHORE_PRODUCER "/semTimerProducer"

extern memory_t *memory;
sem_t *sem_producer_timer, *sem_consumer_timer;
pthread_mutex_t shared_memory_mutex;
pthread_barrier_t turn_barrier;
int threads_at_barrier = 0;

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
            //modifie ca pour implémenter le patron état
            // printf("current timer round : %d\n", memory->timer.round);
            // printf("citizen id : %d , current state : %d\n", citizen_id, memory->citizens[citizen_id].current_state->id);
            //sem_wait(sem_);
            state_t *next_state = memory->citizens[citizen_id].current_state->action(&memory->citizens[citizen_id]);
            memory->citizens[citizen_id].current_state = next_state;
            //sem_post(sem);
            last_round_checked = current_round;
            threads_at_barrier++;
            // printf("Threads à la barrière: %d\n", threads_at_barrier);
            pthread_mutex_unlock(&shared_memory_mutex);

            //printf("heure dans la simulation : %d\n", memory->timer.hours);
            //printf ("\ncitizen id : %d , state id : %d, walking_citizens : %d , at_home_citizens : %d at_work_citizens : %d\n",citizen_id, memory->citizens[citizen_id].current_state->id, memory->walking_citizens , memory->at_home_citizens, memory->at_work_citizens);
        }
        pthread_barrier_wait(&turn_barrier);
        // if(threads_at_barrier == CITIZENS_COUNT) {
        //     threads_at_barrier = 0;
        //     pthread_barrier_wait(&turn_barrier);
        // }
        usleep(100000); // 100 ms pour réduire la consommation CPU
    }
    return NULL;
}

int main() {
    //printf("\n");
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
    sem_consumer_timer = sem_open(SEMAPHORE_CONSUMER, 0);
    if (sem_consumer_timer == SEM_FAILED) {
        perror("sem_open citizen");
        exit(EXIT_FAILURE);
    }   
    sem_producer_timer = sem_open(SEMAPHORE_PRODUCER, 0);
    if (sem_producer_timer == SEM_FAILED) {
        perror("sem_open citizen");
        exit(EXIT_FAILURE);
    }

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
    pthread_barrier_destroy(&turn_barrier);
    pthread_mutex_destroy(&shared_memory_mutex);
    sem_close(sem_consumer_timer);
    sem_close(sem_producer_timer);
    munmap(memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}

// int main() {
    // // Ouvrir la mémoire partagée
    //memory_t memory = open_shared_memory();
    // // Initialiser les outils de synchronisation
    // initialize_synchronization_tools();

    // // Lancer les threads des citoyens
    // manage_citizens(memory->citizens);

    // // Boucle principale de gestion des citoyens
    // while (!memory->simulation_has_ended) {
    //     pthread_barrier_wait(&start_barrier);

    //     // Logique de gestion des actions des citoyens

    //     pthread_barrier_wait(&end_barrier);
    //     sleep(1); // Pause pour simuler le temps de simulation
    // }

    // // Nettoyer et fermer le programme proprement
    // // Attendre la fin des threads

    // // Destruction des outils de synchronisation
    // pthread_barrier_destroy(&start_barrier);
    // pthread_barrier_destroy(&end_barrier);
    // pthread_mutex_destroy(&mutex);

    // // Libération de la mémoire partagée
    // munmap(memory, sizeof(memory_t));  // Assurez-vous que la taille est correcte

    // // Fermeture du descripteur de mémoire partagée
    // close(shmd);

    //return 0;
// }
