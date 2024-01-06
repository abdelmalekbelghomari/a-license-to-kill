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

    // Ouvrir le sémaphore
    // sem = sem_open(SEMAPHORE_NAME, 0);
    // if (sem == SEM_FAILED) {
    //     perror("sem_open counter_intelligence_officer opening");
    //     exit(EXIT_FAILURE);
    // }   

    init_counter_intelligence_officer(memory);
        
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
            if(last_day_checked != current_day){
                memory->counter_intelligence_officer.new_day = true;
                // assign_officer_times(&memory->case_officer);
                // printf("first leaving time : %d : %d  , second leaving_time : %d : %d , shopping_time_time : %d : %d , messaging_time : %d:%d\n", 
                // memory->case_officer.first_leaving_time.leaving_hour, memory->case_officer.first_leaving_time.leaving_minute, memory->case_officer.second_leaving_time.leaving_hour,
                // memory->case_officer.second_leaving_time.leaving_minute,memory->case_officer.shopping_time.leaving_hour,memory->case_officer.shopping_time.leaving_minute,
                // memory->case_officer.messaging_time.leaving_hour, memory->case_officer.messaging_time.leaving_minute);
            }
            state_t *next_state = memory->counter_intelligence_officer.current_state->action(&memory->counter_intelligence_officer);
            memory->counter_intelligence_officer.current_state = next_state;
            // printf("numero de l'état : %d\n",memory->counter_intelligence_officer.current_state->id);
            // sem_wait(sem);
            // sem_post(sem);
            last_day_checked = current_day;
            last_round_checked = current_round;
            // printf ("\nspy id : %d , walking_spies : %d , at_home_spies : %d at_work_spies : %d\n",spy_id, memory->walking_spies , memory->at_home_spies, memory->at_work_spies);
        }

        usleep(100000); // 100 ms pour réduire la consommation CPU
    }




    // Nettoyage

    sem_close(sem_consumer);
    sem_close(sem_producer);
    
    munmap(memory, sizeof(memory_t));
    close(shm_fd);

    return 0;
}
