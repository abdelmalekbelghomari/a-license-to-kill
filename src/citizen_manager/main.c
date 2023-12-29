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
#define NUM_CITIZENS 127



int main() {
    // pthread_barrier_t start_barrier, end_barrier;
    // pthread_mutex_t mutex;
    // memory_t *memory;
    // int shm_fd;

    // // Initialiser la mémoire partagée
    // shm_fd = shm_open(SHARED_MEMORY, O_RDWR , 0666);
    // if (shm_fd == -1) {
    //     perror("Error when shm_open");
    //     exit(EXIT_FAILURE);
    // }  // Cette fonction devrait retourner le descripteur shm

    // memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // if (memory == MAP_FAILED) {
    //     perror("mmap failed");
    //     close(shm_fd);
    //     exit(EXIT_FAILURE);
    // }

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

    return 0;
}
