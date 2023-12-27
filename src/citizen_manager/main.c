#include "../../include/citizen_manager.h"
#include <stdio.h>
#include "../../include/memory.h"
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
    memory_t *memory;
    int shmd;

    // Initialiser la mémoire partagée
    shmd = use_shared_memory(&memory);  // Cette fonction devrait retourner le descripteur shm

    // Initialiser les outils de synchronisation
    initialize_synchronization_tools();

    // Lancer les threads des citoyens
    manage_citizens(memory->citizens);

    // Boucle principale de gestion des citoyens
    while (!memory->simulation_has_ended) {
        pthread_barrier_wait(&start_barrier);

        // Logique de gestion des actions des citoyens

        pthread_barrier_wait(&end_barrier);
        sleep(1); // Pause pour simuler le temps de simulation
    }

    // Nettoyer et fermer le programme proprement
    // Attendre la fin des threads
    finalize_citizen_threads();  // Vous devriez avoir une fonction pour attendre/joindre les threads

    // Destruction des outils de synchronisation
    pthread_barrier_destroy(&start_barrier);
    pthread_barrier_destroy(&end_barrier);
    pthread_mutex_destroy(&mutex);

    // Libération de la mémoire partagée
    munmap(memory, sizeof(memory_t));  // Assurez-vous que la taille est correcte

    // Fermeture du descripteur de mémoire partagée
    close(shmd);

    return 0;
}
