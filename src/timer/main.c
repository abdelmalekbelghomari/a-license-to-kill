#include "timer.h"
#include "spy_simulation.h"
#define SHARED_MEMORY "/SharedMemory"
#define SEMAPHORE_PRODUCER "/semTimerProducer"
#define SEMAPHORE_CONSUMER "/semTimerConsumer"

memory_t *memory;
sem_t *sem_producer_timer, *sem_consumer_timer;


simulated_clock_t new_timer(){
    simulated_clock_t time;
    time.round = 0;
    time.hours = 7;
    time.minutes = 0;
    time.days = 0;
    // memory->memory_has_changed = 1;
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
        //sem_wait(sem_consumer_timer);
        update_timer(memory);
        // printf("Round: %d\n", memory->timer.round);
        // printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        memory->memory_has_changed = 1;
        //sem_post(sem_producer_timer);
        ualarm(100000,0);
    }
        
}   

void set_timer(void)
{
    struct itimerval it;

    /* Clear itimerval struct members */
    timerclear(&it.it_interval);
    timerclear(&it.it_value);

    /* Set timer */
    it.it_interval.tv_usec = TIME_STEP;
    it.it_value.tv_usec = TIME_STEP;
    setitimer(ITIMER_REAL, &it, NULL);
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

    sem_producer_timer = sem_open(SEMAPHORE_PRODUCER, 0);
    if (sem_producer_timer == SEM_FAILED) {
        perror("sem_open failed in timer process");
        exit(EXIT_FAILURE);
    }
    sem_consumer_timer = sem_open(SEMAPHORE_CONSUMER, 0);
    if (sem_consumer_timer == SEM_FAILED) {
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
    set_timer();

    // Configurer le gestionnaire de signal pour SIGALRM
    struct sigaction sa_clock;
    memset(&sa_clock, 0, sizeof(sa_clock)); 
    sa_clock.sa_handler = &tick_clock;
    sigaction(SIGALRM, &sa_clock, NULL);
    ualarm(100000,0);
    

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
    sem_close(sem_consumer_timer);
    sem_close(sem_producer_timer);
    // printf("sem_close timer\n");
    return 0;
}
