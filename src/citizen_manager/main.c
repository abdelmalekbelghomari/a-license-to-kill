#include "../../include/citizen_manager.h"
#include <stdio.h>
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define SHARED_MEMORY "/SharedMemory"
#define NUM_CITIZENS 127
//extern memory_t *memory;  

int main() {
    // Initialiser la mémoire partagée
    use_shared_memory();

    // Initialiser les outils de synchronisation
    initialize_synchronization_tools();

    // Lancer les threads des citoyens
    manage_citizens(memory->citizens);

    // Boucle principale de gestion des citoyens
    while (!memory->simulation_has_ended) {
        // Attendre que tous les threads soient prêts
        pthread_barrier_wait(&start_barrier);

        // Ici, vous pouvez ajouter une logique pour gérer les actions des citoyens

        // Indiquer la fin du tour et attendre que tous les threads aient fini
        pthread_barrier_wait(&end_barrier);

        // (Optionnel) Ajouter une pause pour simuler le temps de simulation
        sleep(1);
    }

    // Nettoyer et fermer le programme proprement
    // ...

    return 0;
}