#include "timer.h"
#define TIME_STEP 1
#define NO_PARTICULAR_REASON -1
#define MAX_ROUNDS 30

memory_t* memory;

time_s new_timer(){
    time_s timer;
    timer.hours = 0;
    timer.minutes = 0;
    timer.days = 1;
    timer.round = 0;
    return timer;
}

void update_time(time_s *t){
    t->minutes += 10;
    if(t->minutes >= 60){
        t->minutes= 0;
        t->hours += 1;
        if(t->hours >= 24){
            t->hours = 0;
            t->days++;
        }
    }
}

void handler(int sig)
{
    
    memory->timer.round += 1;
    update_time(&memory->timer);
    kill(memory->pids[0],SIGUSR1);
    kill(memory->pids[1],SIGUSR1);

    //fin du round il faut envoyer un signal aux autres process via leur pid
    /*
    for(int i=0;i<7;i++){
        kill(memory->pids[i],SIGUSR1);
    }
    */
    sig = sig;
    //printf("tour numéro:  %d \n", memory->timer.round);
    //printf("jour %d / heure:  %d:%d \n",memory->timer.days, memory->timer.hours,memory->timer.minutes);
}

void set_clock(){
    struct itimerval tv;
    tv.it_value.tv_sec = TIME_STEP; //time of first timer
    tv.it_value.tv_usec = 0; //time of first timer
    tv.it_interval.tv_sec = TIME_STEP; //time of all timers but the first one
    tv.it_interval.tv_usec = 0; //time of all timers but the first one
    setitimer(ITIMER_REAL, &tv, NULL);
}

void set_signal(){
    struct sigaction act, oact;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handler;
    sigaction(SIGALRM, &act, &oact);
}

void access_memory(){
    memory = (memory_t *)malloc(sizeof(memory_t));
    int shmd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if(shmd == -1){
        perror("Error when shm_open");
    }
    memory = mmap(NULL,
                        sizeof(memory_t*),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        shmd,
                        0
                        );
    //close(shmd);
}

int main()
{

    access_memory();

    time_s timer=new_timer();
    memory->timer = timer;

    set_clock();
    set_signal();

    while(1) {

        if(memory->timer.round == MAX_ROUNDS){
            //fin de la partie on arrête le timer
            setitimer(ITIMER_REAL,0,NULL);
            memory->simulation_has_ended = 1;
            kill(memory->pids[0],SIGUSR2);
            kill(memory->pids[1],SIGUSR2);
            //on envoie un signal de fin aux autres processus via leur pid
            /*
            for(int i=0;i<7;i++){
                kill(memory->pids[i],SIGUSR2);
            }
            */
            munmap(memory, sizeof(memory_t*));
            exit(0);
        }
        pause();
    }

    return 0;
}
