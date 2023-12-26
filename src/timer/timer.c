#include "timer.h"
#define SHARED_MEMORY "/spy_simulation"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

time_t new_timer(memory_t *memory){
    time_t time = memory->timer;
    time.round = 0;
    time.hours = 0;
    time.minutes = 0;
    time.days = 0;
    return time;
}

void update_timer(){
    memory->timer.round++;
    memory->timer.minutes += 10;
    if (memory->timer.minutes == 60){
        memory->timer.hours++;
        memory->timer.minutes = 0;
    }
    if (memory->timer.hours == 24){
        memory->timer.days++;
        memory->timer.hours = 0;
    }
}


void tick_clock(int sig){
    if(sig == SIGALRM){
        pthread_mutex_lock(&mutex);
        update_timer();
        pthread_mutex_unlock(&mutex);
        alarm(1);
    }
    
}

void access_memory(){
    memory = (memory_t *)malloc(sizeof(memory_t));
    int shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        perror("Error when shm_open");
    }
    memory = mmap(NULL,sizeof(memory_t*), PROT_READ | PROT_WRITE, 
                                                MAP_SHARED,shm_fd,0);
    //close(shmd);
}


