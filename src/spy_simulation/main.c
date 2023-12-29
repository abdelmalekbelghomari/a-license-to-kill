#include <stdio.h>
#include "spy_simulation.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "monitor_common.h"
#include "monitor.h"

extern WINDOW *main_window;
extern int old_cursor;

int main(int argc, char **argv)
{
    memory_t *memory = create_shared_memory("SharedMemory");
    //printf("\nDescripteur de la queue de messages: %d\n", memory->mqInfo.mq);
    start_simulation_processes();
     // Assurez-vous que toutes les opérations sur la mémoire partagée sont terminées avant de la démmaper
    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mmapping the file");
    } 

    // Supprimer l'objet de mémoire partagée
    if (shm_unlink("SharedMemory") == -1) {
        perror("Error removing the shared memory object");
        return EXIT_FAILURE;
    }
 
    return EXIT_SUCCESS;

}
