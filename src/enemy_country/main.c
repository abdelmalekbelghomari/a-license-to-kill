#include "memory.h"

memory_t *memory;
sem_t *sem_consumer, *sem_producer;

void caesar_decipher(char *message) {
    for (int i = 0; message[i] != '\0'; ++i) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - SHIFT;
            if (ch < 'a') ch += 26;
            message[i] = ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch = ch - SHIFT;
            if (ch < 'A') ch += 26;
            message[i] = ch;
        }
    }
}

int main() {
    mqd_t mq = mq_open("/spy_message_queue", O_RDONLY);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open("/SharedMemory", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    memory = mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    sem_consumer = sem_open("/semConsumer", 0);
    if (sem_consumer == SEM_FAILED) {
        perror("sem_open consumer");
        exit(EXIT_FAILURE);
    }

    sem_producer = sem_open("/semProducer", 0);
    if (sem_producer == SEM_FAILED) {
        perror("sem_open producer");
        exit(EXIT_FAILURE);
    }

    char received_message[MAX_MESSAGE_SIZE];
    unsigned int message_priority;
    
    while (1) {
        memset(received_message, 0, MAX_MESSAGE_SIZE); // Réinitialiser le received_message
        ssize_t bytes_read = mq_receive(mq, received_message, MAX_MESSAGE_SIZE, &message_priority);

        if (bytes_read >= 0) {
            // Traiter le message reçu
            if (message_priority != 1) {
                caesar_decipher(received_message);
                strcpy(memory->messages[memory->message_count], received_message);
                memory->message_count++;
                memory->memory_has_changed = 1;
            } else {
            }
        } else {
            perror("mq_receive"); 
            exit(EXIT_FAILURE);
        }
    }
    sem_close(sem_consumer);
    sem_close(sem_producer);
    mq_close(mq);
    mq_unlink("/spy_message_queue");
    munmap(memory, sizeof(memory_t));
    close(shm_fd);
    return 0;
}
