/**
 * @file main.c
 * @brief Main entry point for counterintelligence operations.
 *
 * This file serves as the main entry point for counterintelligence
 * operations in the spy simulation. It initializes and manages
 * counterintelligence officers and their activities.
 *
 */

#include "counter_intelligence.h"
#include "memory.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#define SHARED_MEMORY "/SharedMemory"
#define SEMAPHORE_NAME "/sem"

memory_t *memory;
sem_t *sem_consumer, *sem_producer;

int main() {
    // printf("\n");
    srand(time(NULL) ^ getpid());// la graine doit être différente du parent

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



    init_counter_intelligence_officer(memory);
        
    int last_round_checked = -1;
    int current_round = memory->timer.round;
    int last_day_checked = -1;
    int current_day = memory->timer.days;
    while(current_round != 2016 /* || memory->simulation_has_ended==0 */) {

        current_round = memory->timer.round;
        current_day = memory->timer.days;


        if (last_round_checked != current_round) {
            if(last_day_checked != current_day){
                memory->counter_intelligence_officer.new_day = true;

            }
            state_t *next_state = memory->counter_intelligence_officer.current_state->action(&memory->counter_intelligence_officer);
            memory->counter_intelligence_officer.current_state = next_state;

            last_day_checked = current_day;
            last_round_checked = current_round;
        }

        usleep(100000); // 100 ms pour réduire la consommation CPU
    }




    // Nettoyage
    free(memory);
    sem_close(sem_consumer);
    sem_close(sem_producer);
    munmap(memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}
