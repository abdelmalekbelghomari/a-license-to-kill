#include "timer.h"
#define SHARED_MEMORY "/SharedMemory"

memory_t *memory;
sem_t *sem;


simulated_clock_t new_timer(){
    simulated_clock_t time;
    time.round = 0;
    time.hours = 0;
    time.minutes = 0;
    time.days = 0;
    return time;
}

void update_timer(memory_t *memory){
    memory->timer.round++;
    if(memory->timer.round == MAX_ROUNDS){
        memory->simulation_has_ended = 1;
    }
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
        sem_wait(sem);
        update_timer(memory);
        printf("Round: %d\n", memory->timer.round);
        printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        sem_post(sem);
        alarm(1);

    }
    
}

int main() {

    int shm_fd;

    sem = sem_open("/timer_sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed in timer process");
        exit(EXIT_FAILURE);
    }


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

    // Initialiser le timer
    simulated_clock_t timer = new_timer();

    memory->timer = timer;


    // Configurer le timer
    struct itimerval it;
    memset(&it, 0, sizeof(it));

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
    memset(&sa_clock, 0, sizeof(sa_clock)); // Initialiser la structure à 0
    sa_clock.sa_handler = &tick_clock;
    sigaction(SIGALRM, &sa_clock, NULL);
    
    while (1) { 
        if(memory->simulation_has_ended){
            break;
        } else {
            // pthread_mutex_lock(&mutexTimer1);
            // update_timer(memory);
            // pthread_mutex_unlock(&mutexTimer1);
            // printf("Round: %d\n", memory->timer.round);
            // printf("Time: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        }
        // sleep(1);


        // pause();
    }  
    sem_close(sem);

    return 0;
}
