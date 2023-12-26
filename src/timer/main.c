#include "timer.h"
#define SHARED_MEMORY "/SharedMemory"

pthread_mutex_t mutexTimer1 = PTHREAD_MUTEX_INITIALIZER;
memory_t *memory = NULL;

int main() {

    int shm_fd;
    pthread_mutex_init(&mutexTimer1, NULL);

    // Ouvrir la mémoire partagée
    shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error when shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(memory_t)) == -1) {
        perror("Error in ftruncate");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    // Mapper la mémoire partagée
    memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Fermer le file descriptor de la mémoire partagée
    close(shm_fd);

    // Initialiser le timer
    simulated_clock_t timer = new_timer();
    pthread_mutex_lock(&mutexTimer1);
    memory->timer = timer;
    pthread_mutex_unlock(&mutexTimer1);

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

    // Configurer le gestionnaire de signal pour SIGALRM
    struct sigaction sa_clock;
    memset(&sa_clock, 0, sizeof(sa_clock)); // Initialiser la structure à 0
    sa_clock.sa_handler = &tick_clock;
    sigaction(SIGALRM, &sa_clock, NULL);
    setitimer(ITIMER_REAL, &it, NULL);
    while (1) {
        pause();
    }
    
    pthread_mutex_destroy(&mutexTimer1);

    return 0;
}
