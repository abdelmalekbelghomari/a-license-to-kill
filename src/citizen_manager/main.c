//#include "citizen_manager.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define NUM_CITIZENS 127

extern pthread_mutex_t mutex;
extern pthread_barrier_t start_barrier, end_barrier;
 
#include "memory.h"
// #include "citizen_manager.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define CITIZENS_COUNT 127

pthread_barrier_t turn_barrier;
pthread_mutex_t shared_memory_mutex;


memory_t *memory;
sem_t *sem;
pthread_barrier_t turn_barrier;

void* citizen_thread(void* arg) {
    int citizen_id = *(int*)arg;
    int last_round_checked = -1;
    int current_round = memory->timer.round;
    while(current_round != 2016) {
        sem_wait(sem); // Attente pour accéder à la mémoire partagée
        current_round = memory->timer.round;
        sem_post(sem);

        if (last_round_checked != current_round) {
            pthread_mutex_lock(&shared_memory_mutex);
            //modifie ca pour implémenter le patron état
            sem_wait(sem);
            state_t *next_state = memory->citizens[citizen_id].current_state->action(&memory->citizens[citizen_id]);
            memory->citizens[citizen_id].current_state = next_state;
            sem_post(sem);

            last_round_checked = current_round;
            pthread_mutex_unlock(&shared_memory_mutex);
            pthread_barrier_wait(&turn_barrier);
            // printf ("\ncitizen id : %d , walking_citizens : %d , at_home_citizens : %d at_work_citizens : %d\n",citizen_id, memory->walking_citizens , memory->at_home_citizens, memory->at_work_citizens);
        }

        usleep(100000); // 100 ms pour réduire la consommation CPU
    }

    return NULL;
}

int main() {
     printf("\n");
    pthread_t threads[CITIZENS_COUNT];
    int citizen_ids[CITIZENS_COUNT];
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
    sem = sem_open("/timer_sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }   
    init_citizens(memory);

    // Initialisation de la barrière
    pthread_barrier_init(&turn_barrier, NULL, CITIZENS_COUNT);
    pthread_mutex_init(&shared_memory_mutex, NULL);

    // Créer les threads de citoyens
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_ids[i] = i;
        if (pthread_create(&threads[i], NULL, &citizen_thread, &citizen_ids[i])) {
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
    sem_close(sem);
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
