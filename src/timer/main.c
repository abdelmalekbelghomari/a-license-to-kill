#include "timer.h"
#include "spy_simulation.h"
#define SHARED_MEMORY "/SharedMemory"
#define SEMAPHORE_PRODUCER "/semProducer"
#define SEMAPHORE_CONSUMER "/semConsumer"

memory_t *memory;
sem_t *sem_producer, *sem_consumer;


simulated_clock_t new_timer(){
    simulated_clock_t time;
    time.round = 0;
    time.hours = 8;
    time.minutes = 0;
    time.days = 0;
    memory->memory_has_changed = 1;
    return time;
}

void update_timer(memory_t *memory){
    memory->timer.round++;
    memory->timer.minutes += 10;
    if (memory->timer.minutes >= 60){
        memory->timer.hours++;
        memory->timer.minutes = 0;
    }
    if (memory->timer.hours >= 24){
        memory->timer.days++;
        memory->timer.hours = 0;
    }
}

void tick_clock(int sig){
    if(sig == SIGALRM){
        // memory->memory_has_changed = 1;
        //sem_wait(sem_consumer);
        update_timer(memory);
        // printf("Round: %d\n", memory->timer.round);
        // printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        memory->memory_has_changed = 1;
        //sem_post(sem_producer);
        alarm(1);
    }
        
}   

int main() {

    int shm_fd;

    // Ouvrir la mémoire partagée
    shm_fd = shm_open(SHARED_MEMORY, O_RDWR , 0666);
    if (shm_fd == -1) {
        perror("Error when shm_open");
        exit(EXIT_FAILURE);
    }
    // Mapper la mémoire partagée
    memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    sem_producer = sem_open(SEMAPHORE_PRODUCER, 0);
    if (sem_producer == SEM_FAILED) {
        perror("sem_open failed in timer process");
        exit(EXIT_FAILURE);
    }
    sem_consumer = sem_open(SEMAPHORE_CONSUMER, 0);
    if (sem_consumer == SEM_FAILED) {
        perror("sem_open failed in timer process");
        exit(EXIT_FAILURE);
    }
    // printf("sem_open timer\n");

    // Initialiser le timer
    simulated_clock_t timer = new_timer();

    //sem_wait(sem);
    // printf("sem_wait timer\n");
    memory->timer = timer;
    // sem_post(sem);
    // printf("sem_post timer\n");
    // printf("Timer initialized\n");

    // Configurer le timer
    struct itimerval it;
    memset(&it, 0, sizeof(it)); // Initialiser la structure à 0

    if (STEP >= 1000000) {
        it.it_interval.tv_sec = STEP / 1000000;
        it.it_value.tv_sec = STEP / 1000000;
    } else {
        it.it_interval.tv_usec = STEP;
        it.it_value.tv_usec = STEP;
    }
    setitimer(ITIMER_REAL, &it, NULL);

    // Configurer le gestionnaire de signal pour SIGALRM
    struct sigaction sa_clock;
    memset(&sa_clock, 0, sizeof(sa_clock)); 
    sa_clock.sa_handler = &tick_clock;
    sigaction(SIGALRM, &sa_clock, NULL);
    alarm(1);
    
    while(1){
        pause();
    }
    
    // while (memory->simulation_has_ended == 0) { 
    //     // if(memory->simulation_has_ended){
    //     //     break;
    //     // } else {
    //         // pthread_mutex_lock(&mutexTimer1);
    //         // update_timer(memory);
    //         // pthread_mutex_unlock(&mutexTimer1);
    //         // printf("Round: %d\n", memory->timer.round);
    //         // printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
    //     // }
    //     // sleep(1);


    //     // pause();
    // }
    // printf("Timer ended\n");
    sem_close(sem_consumer);
    sem_close(sem_producer);
    // printf("sem_close timer\n");
    return 0;
}
